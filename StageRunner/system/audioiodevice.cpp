#include "audioiodevice.h"
#include "audioformat.h"
#include "../system/log.h"
#include "fftreal/fftreal_wrapper.h"
#include "frqspectrum.h"

#include <QTime>
#include <QByteArray>
#include <qmath.h>

AudioIODevice::AudioIODevice(AudioFormat format, QObject *parent) :
	QIODevice(parent)
{
	decoding_finished_f = false;
	audio_buffer_count = 0;
	bytes_avail = 0;
	bytes_read = 0;
	frame_energy_peak = 0;
	sample_peak = 0;
	audio_error = AUDIO::AUDIO_ERR_NONE;
	audio_format = new AudioFormat(format);
	audio_buffer = new QByteArray;

	loop_count = 0;
	loop_target = 0;

	m_leftAvg = new PsMovingAverage<qreal>(4);
	m_rightAvg = new PsMovingAverage<qreal>(4);

	// Size of FFT data block
	m_fftDim = pow(2, FFTLengthPowerOfTwo);
	qDebug("init FFT Size: %d",m_fftDim);
	m_windowDat.resize(m_fftDim);

	for (int t=0; t<4; t++) {
		m_outFFTDat[t].resize(m_fftDim);
		m_inFFTDat[t].resize(m_fftDim);
	}
	// Calculate Hann Window
	for (int t=0; t<m_fftDim; t++) {
		m_windowDat[t] = 0.5 * (1 - qCos((2 * M_PI * t) / (m_fftDim - 1)));
	}

	m_leftFFT = new FFTRealWrapper;
	m_leftFFT = new FFTRealWrapper;



#ifdef IS_QT5
	audio_decoder = new QAudioDecoder;
	connect(audio_decoder,SIGNAL(bufferReady()),this,SLOT(process_decoder_buffer()));
	connect(audio_decoder,SIGNAL(finished()),this,SLOT(on_decoding_finished()));
	connect(audio_decoder,SIGNAL(error(QAudioDecoder::Error)),this,SLOT(if_error_occurred(QAudioDecoder::Error)));
	connect(audio_decoder,SIGNAL(durationChanged(qint64)),this,SLOT(if_audio_duration_changed(qint64)));
	connect(this,SIGNAL(rawDataProcessed(const char*,int,QAudioFormat)),this,SLOT(calcVuLevel(const char*,int,QAudioFormat)),Qt::QueuedConnection);

#endif
}

AudioIODevice::~AudioIODevice()
{

#ifdef IS_QT5
	delete audio_decoder;
#endif
	delete audio_buffer;
	delete audio_format;
	delete m_leftAvg;
	delete m_rightAvg;
	delete m_leftFFT;
	delete m_rightFFT;
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
	qint64 avail = bytes_avail-bytes_read;
	// qDebug("want readData %lli",maxlen);
	if (maxlen == 0)
		return 0;

	if (avail < 0) {
		if (run_time.elapsed() > 500 && bytes_avail == 0) {
			audio_error = AUDIO::AUDIO_ERR_TIMEOUT;
			DEBUGERROR("No data available from audio IODevice after 500ms -> Cancel");
			emit readReady();
			return 0;
		}
		else if (run_time.elapsed() > 10000) {
			audio_error = AUDIO::AUDIO_ERR_TIMEOUT;
			DEBUGERROR("Seek failed! Not enough data available from audio IODevice after 10s -> Cancel");
			emit readReady();
			return 0;
		}
		// This generates NULL level audio data
		for (int t=0; t<maxlen; t++) {
			data[t] = 0;
		}
		return maxlen;
	}

	// Check for regular end of playing
	if (avail == 0 && bytes_read > 0 && decoding_finished_f) {
		if (loop_count < loop_target || loop_target < 0) {
			if (debug > 1) LOGTEXT(tr("Loop audio file '%1' -> Loop: %2").arg(current_filename).arg(loop_count));
			bytes_read = 0;
			avail = bytes_avail;
			loop_count++;
		} else {
			if (debug > 2) qDebug("maxlen %lli, bytes_read: %lli, avail %lli",maxlen,bytes_read,avail);
			emit readReady();
			return 0;
		}
	}

	if (avail == 0 && bytes_read == 0 && !decoding_finished_f) {
		if (run_time.elapsed() > 500) {
			audio_error = AUDIO::AUDIO_ERR_TIMEOUT;
			DEBUGERROR("No data available from audio IODevice after 500ms -> Cancel");
			emit readReady();
			return 0;
		}
		// This generates NULL level audio data
		for (int t=0; t<maxlen; t++) {
			data[t] = 0;
		}
		return maxlen;
	}

	// qDebug("  -> readData %lli",maxlen);
	if (maxlen>avail) {
		maxlen = avail;
	}

	memcpy(data, audio_buffer->data()+bytes_read, maxlen);
	bytes_read += maxlen;

	calcVuLevel(data,maxlen,*audio_format);
	// emit calcVuLevel(data,maxlen,*audio_format);
	// qDebug() << "emit calc vu level" << QThread::currentThread()->objectName();

	return maxlen;
}

qint64 AudioIODevice::writeData(const char *data, qint64 len)
{
	Q_UNUSED (data);
	Q_UNUSED (len);
	return 0;
}

qint64 AudioIODevice::bytesAvailable() const
{
	// qDebug("bytesAvail %lli",bytes_avail-bytes_read);
	return bytes_avail - bytes_read;
}

bool AudioIODevice::setSourceFilename(const QString &filename)
{
#ifdef IS_QT5
	audio_decoder->setSourceFilename(filename);
	audio_decoder->setAudioFormat(*audio_format);
#endif
	current_filename = filename;

	return true;
}

void AudioIODevice::examineQAudioFormat(AudioFormat &form)
{
	int samplesize = form.sampleSize();
	int channels = form.channelCount();
	int samplerate = form.sampleRate();
	QString codec = form.codec();

	if (debug) {
		qDebug("Audioformat: %dHz, %d Channels, Size per sample: %d (Codec:%s)"
			   ,samplerate,channels,samplesize,codec.toLocal8Bit().data());
	}

}


/**
 * @brief Calculate RMS level of audio signal
 * @param data
 * @param size
 * @param audioFormat
 */
void AudioIODevice::calcVuLevel(const char *data, int size, const QAudioFormat & audioFormat)
{
//	static QTime checktime;
//	qDebug("calcLast %dms",checktime.elapsed());
//	checktime.start();

	qreal left = 0;
	qreal right = 0;
	qreal energy[4] = {0,0,0,0};
	qreal peak[4] = {0,0,0,0};

	int channels = audioFormat.channelCount();
	int frames = size / channels;

	if (frames == 0) return;
	// qDebug() << "calcVuLevel" << size << QThread::currentThread()->objectName();

	switch (audioFormat.sampleType()) {
	case QAudioFormat::SignedInt:
	case QAudioFormat::UnSignedInt:
	{
		switch (audioFormat.sampleSize()) {
		case 16:
		{
			const qint16 *dat = reinterpret_cast<const qint16*>(data);
			frames /= 2;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					const qint16 val = dat[frame*channels+chan];
					const qreal valF = AudioIODevice::pcm16ToReal(val,audioFormat);
					if (valF > sample_peak)
						sample_peak = valF;
					if (valF > peak[chan])
						peak[chan] = valF;

					energy[chan] += valF * valF;
					if (chan == 0)
						m_inBuffer[chan].append(valF);
				}
			}
		}
			break;
		default:
			DEBUGERROR("Sampletype in audiostream not supported");
			break;
		}
		left = sqrt(energy[0] / frames);
		right = sqrt(energy[1] / frames);
	}
		break;
	case QAudioFormat::Float:
	{
		switch (audioFormat.sampleSize()) {
		case 32:
		{
			const float *dat = reinterpret_cast<const float*>(data);
			frames /= 4;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					const float val = dat[frame*channels+chan];
					// const qreal valF = AudioIODevice::realToRealNorm(val,audioFormat);
					const float valF = val;

					if (valF > sample_peak)
						sample_peak = valF;
					if (valF > peak[chan])
						peak[chan] = valF;

					energy[chan] += valF * valF;
					if (chan == 0)
						m_inBuffer[chan].append(valF);
				}
			}
		}
			break;
		default:
			DEBUGERROR("Sampletype in audiostream not supported");
			break;
		}
		left = sqrt(energy[0] / frames);
		right = sqrt(energy[1] / frames);
	}
		break;
	case QAudioFormat::Unknown:
		DEBUGERROR("Sampletype in audiostream unknown");
		break;
	}

	// qDebug("left:%f right:%f",left,right);
	if (left/frames > frame_energy_peak) frame_energy_peak = left/frames;

	m_leftAvg->append(left);
	m_rightAvg->append(right);

	// double vl = m_leftAvg->currentAvg();
	// double vr = m_rightAvg->currentAvg();

	emit vuLevelChanged(left * 1.8,right * 1.8);

	bool e = false;
	while (m_inBuffer[0].size() >= m_fftDim) {
		for (int t=0; t<m_fftDim; t++)
			m_inFFTDat[0][t] = m_inBuffer[0][t] * m_windowDat[t];

		m_leftFFT->calculateFFT(m_outFFTDat[0].data(), m_inFFTDat[0].data());

		m_frqSpectrum[0].fillSpectrumFFTQVectorArray(m_outFFTDat[0]);

		m_inBuffer[0].remove(0,m_fftDim/8);

		e = true;
	}

	if (e) {
		emit frqSpectrumChanged(&m_frqSpectrum[0]);
	} else {
		// qDebug("  no fft");
	}

}


qint64 AudioIODevice::currentPlayPosMs() const
{
	return audio_format->durationForBytes(bytes_read) / 1000;

	if (bytes_read >= bytes_avail && bytes_avail != 0) {
		return 0;
	} else {
	}
}

qint64 AudioIODevice::currentPlayPosUs() const
{
	return audio_format->durationForBytes(bytes_read);

	if (bytes_read >= bytes_avail && bytes_avail != 0) {
		return 0;
	} else {
	}
}

bool AudioIODevice::seekPlayPosMs(qint64 posMs)
{
	bool seekok = true;

	qint64 seekpos = audio_format->bytesForDuration(posMs * 1000);
	if (seekpos > bytes_avail && bytes_read != 0) {
		bytes_read = bytes_avail;
		seekok = false;
	} else {
		bytes_read = seekpos;
	}
	return seekok;
}

void AudioIODevice::start(int loops)
{
	if (loops < 0) {
		loop_target = -1;
	}
	else if (loops > 0) {
		loop_target = loops;
	}
	else {
		loop_target = 1;
	}
	loop_count = 1;


	if (!open(QIODevice::ReadOnly)) {
		DEBUGERROR("Could not open Audio IO device");
		return;
	}

	audio_error = AUDIO::AUDIO_ERR_NONE;
	audio_buffer_count = 0;
	audio_buffer->clear();
	bytes_read = 0;
	bytes_avail = 0;
	frame_energy_peak = 0;
	sample_peak = 0;
	run_time.start();

#ifdef IS_QT5
	// Now start reading and decoding of sound file. Signal "bufferReady" will be
	// emitted when an audio portion is decoded. Audio data will be processed in
	// process_decoder_buffer
	decoding_finished_f = false;
	audio_decoder->start();
#endif
	LOGTEXT(tr("Decoding of audio file '%1' started").arg(current_filename));

}

void AudioIODevice::stop()
{
#ifdef IS_QT5
	decoding_finished_f = true;
	if (audio_decoder->state() == QAudioDecoder::DecodingState) {
		audio_decoder->stop();
	}
#endif
	close();
}

void AudioIODevice::process_decoder_buffer()
{
#ifdef IS_QT5
	if (!isOpen()) return;

	QAudioBuffer audiobuf = audio_decoder->read();
	// AudioFormat form = audiobuf.format();
	const char *data = audiobuf.constData<char>();
	audio_buffer->append(data,audiobuf.byteCount());
	bytes_avail += audiobuf.byteCount();

	// qDebug("processAudio %d: size: %d",audio_buffer_count, frames);
	audio_buffer_count++;
#endif
}

void AudioIODevice::on_decoding_finished()
{
	decoding_finished_f = true;
	LOGTEXT(tr("Decoding of audio file '%1' finished (<font color=green>%2ms</font>")
			.arg(current_filename).arg(run_time.elapsed()));
	if (debug) qDebug("Audio decoding finished");
}

void AudioIODevice::if_audio_duration_changed(qint64 duration)
{
	if (duration > 0) {
		emit audioDurationDetected(duration);
	}
}


#ifdef IS_QT5
void AudioIODevice::if_error_occurred(QAudioDecoder::Error error)
{
	DEBUGERROR("An error occurred while decoding audio: %s (error: %d)"
			  ,audio_decoder->errorString().toLocal8Bit().data(),error);
	LOGERROR(tr("Failed file was: %1").arg(audio_decoder->sourceFilename()));
	audio_error = AUDIO::AUDIO_ERR_DECODER;
}
#endif


#include "audioiodevice.h"
#include "audioformat.h"
#include "../system/log.h"
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
	m_fftEnabled = false;
	m_currentPlaybackSamplerate = 0;

	loop_count = 0;
	loop_target = 0;

	m_leftAvg = new PsMovingAverage<qreal>(4);
	m_rightAvg = new PsMovingAverage<qreal>(4);

	// Size of FFT data block
	// qDebug("init FFT Size: %d",m_leftFFT.fftDimension());
	m_leftFFT.setOversampling(4);

#ifdef IS_QT5
	audio_decoder = new QAudioDecoder;
	if (audio_decoder->error() == QAudioDecoder::ServiceMissingError) {
		m_lastErrorText = tr("Audio decoder not initialized. No decode/play service available");
		audio_error = AUDIO::AUDIO_ERR_DECODER;
		return;
	}
	qDebug() << "audio:" << audio_decoder->error();

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

	memcpy(data, audio_buffer->data()+bytes_read, size_t(maxlen));
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
	m_currentPlaybackSamplerate = 0;
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
	qint64 frames = size / channels;

	if (frames == 0) return;
	// qDebug() << "calcVuLevel" << size << QThread::currentThread()->objectName();

	switch (audioFormat.sampleType()) {
	case QAudioFormat::SignedInt:
	case QAudioFormat::UnSignedInt:
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
						if (m_fftEnabled) {
							switch (chan) {
							case 0:
								m_leftFFT.appendToBuffer(valF);
								break;
							case 1:
								m_rightFFT.appendToBuffer(valF);
								break;
							}
						}
					}
				}
			}
			break;
		case 32:
			{
				const qint32 *dat = reinterpret_cast<const qint32*>(data);
				frames /= 4;

				for (int chan = 0; chan < channels; chan++) {
					for (int frame = 0; frame<frames; frame++) {
						const qint32 val = dat[frame*channels+chan];
						const qreal valF = AudioIODevice::pcm32ToReal(val,audioFormat);
						if (valF > sample_peak)
							sample_peak = valF;
						if (valF > peak[chan])
							peak[chan] = valF;

						energy[chan] += valF * valF;
						if (m_fftEnabled) {
							switch (chan) {
							case 0:
								m_leftFFT.appendToBuffer(valF);
								break;
							case 1:
								m_rightFFT.appendToBuffer(valF);
								break;
							}
						}
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
					if (m_fftEnabled) {
						switch (chan) {
						case 0:
							m_leftFFT.appendToBuffer(valF);
							break;
						case 1:
							m_rightFFT.appendToBuffer(valF);
							break;
						}
					}
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

	if (m_fftEnabled) {
		while (m_leftFFT.bufferFilled()) {
			m_leftFFT.calculateFFT();

			m_leftSpectrum.fillSpectrumFFTQVectorArray(m_leftFFT.fftComplexArray());
			if (audioFormat.sampleRate() > m_currentPlaybackSamplerate) {
				m_currentPlaybackSamplerate = audioFormat.sampleRate();
				m_leftSpectrum.setMaxFrequency(double(m_currentPlaybackSamplerate) / 2);
			}
			emit frqSpectrumChanged(&m_leftSpectrum);
		}
	} else {
		m_leftFFT.clearBuffer();
	}
	if (m_fftEnabled) {
		while (m_rightFFT.bufferFilled()) {
			m_rightFFT.calculateFFT();

			m_rightSpectrum.fillSpectrumFFTQVectorArray(m_rightFFT.fftComplexArray());
			if (audioFormat.sampleRate() > m_currentPlaybackSamplerate) {
				m_currentPlaybackSamplerate = audioFormat.sampleRate();
				m_leftSpectrum.setMaxFrequency(double(m_currentPlaybackSamplerate) / 2);
			}
			emit frqSpectrumChanged(&m_rightSpectrum);
		}
	} else {
		m_rightFFT.clearBuffer();
	}
}


qint64 AudioIODevice::currentPlayPosMs() const
{
	return audio_format->durationForBytes(qint32(bytes_read) / 1000);

//	if (bytes_read >= bytes_avail && bytes_avail != 0) {
//		return 0;
//	} else {
//	}
}

qint64 AudioIODevice::currentPlayPosUs() const
{
	return audio_format->durationForBytes(qint32(bytes_read));

//	if (bytes_read >= bytes_avail && bytes_avail != 0) {
//		return 0;
//	} else {
//	}
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


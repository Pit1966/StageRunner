//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "audioiodevice.h"
#include "audioformat.h"
#include "../system/log.h"
#include "frqspectrum.h"

#include <QTime>
#include <QByteArray>
#include <qmath.h>
#include <QUrl>

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
	m_currentPan = 0;
	m_maxPan = 200;

	loop_count = 0;
	loop_target = 0;

	m_leftAvg = new PsMovingAverage<qreal>(4);
	m_rightAvg = new PsMovingAverage<qreal>(4);

	// Size of FFT data block
	// qDebug("init FFT Size: %d",m_leftFFT.fftDimension());
	m_leftFFT.setOversampling(4);

	audio_decoder = new AudioDecoder;
	if (AudioDecoder::Error(audio_decoder->error()) == AudioDecoder::NotSupportedError) {
		m_lastErrorText = tr("Audio decoder not initialized. No decode/play service available");
		audio_error = AUDIO::AUDIO_ERR_DECODER;
		return;
	}
	// qDebug() << Q_FUNC_INFO << audio_decoder->error();

	qRegisterMetaType<AudioFormat>("AudioFormat");

	connect(audio_decoder,SIGNAL(bufferReady()),this,SLOT(process_decoder_buffer()));
	connect(audio_decoder,SIGNAL(finished()),this,SLOT(on_decoding_finished()));
	connect(audio_decoder,SIGNAL(error(QAudioDecoder::Error)),this,SLOT(if_error_occurred(QAudioDecoder::Error)));
	connect(audio_decoder,SIGNAL(durationChanged(qint64)),this,SLOT(if_audio_duration_changed(qint64)));
	// connect(this,SIGNAL(rawDataProcessed(const char*,int,AudioFormat)),this,SLOT(calcVuLevel(const char*,int,AudioFormat)),Qt::QueuedConnection);
}

AudioIODevice::~AudioIODevice()
{

	delete audio_decoder;
	delete audio_buffer;
	delete audio_format;
	delete m_leftAvg;
	delete m_rightAvg;
}

bool AudioIODevice::open(QIODevice::OpenMode mode)
{
	bool ok = QIODevice::open(mode);
	if (!ok) {
		LOGERROR(tr("Could not open audio file: %1!").arg(errorString()));
		audio_error = AUDIO::AUDIO_ERR_FILE_NOT_OPENED;
	}
	return ok;
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
	qint64 avail = bytes_avail-bytes_read;
	qDebug("want readData %lli",maxlen);
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

	if (m_currentPan > 0)
		calcPanning(data, maxlen, *audio_format);
	calcVuLevel(data, maxlen, *audio_format);
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
	if (isDecoding()) {
		qWarning() << "audio decoding while starting" << filename;
	} else {
		if (debug > 1)
			qWarning() << "start audio" << filename;
	}
	audio_decoder->setSourceFilename(filename);
	audio_decoder->setAudioFormat(*audio_format);
	current_filename = filename;
	m_currentPlaybackSamplerate = 0;
	return true;
}

void AudioIODevice::examineAudioFormat(AudioFormat &form)
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

void AudioIODevice::setPanning(int pan, int maxPan)
{
	m_currentPan = pan;
	m_maxPan = maxPan;
	if (pan == 0)		// deactivated
		return;

	// we have to calculate amplification factors for both the left and the right channel


	if (pan > maxPan/2) {
		// panorama right
		m_panVolRight = 1.0;
		m_panVolLeft = qreal((maxPan - pan) * 2) / maxPan;
	}
	else {
		// panorama left
		m_panVolLeft = 1.0;
		m_panVolRight = qreal((pan-1) * 2) / maxPan;
	}
	m_panVolLeft = QAudio::convertVolume(m_panVolLeft,
										 QAudio::LogarithmicVolumeScale,
										 QAudio::LinearVolumeScale);
	m_panVolRight = QAudio::convertVolume(m_panVolRight,
										 QAudio::LogarithmicVolumeScale,
										 QAudio::LinearVolumeScale);

	// panning vol calculation option 2
	// m_panVolLeft = qreal(maxPan - pan) / maxPan;
	// m_panVolRight = qreal(pan) / maxPan;

	qDebug() << "pan values" << m_panVolLeft << m_panVolRight;

}

bool AudioIODevice::isDecoding() const
{
	return audio_decoder->state() == QAudioDecoder::DecodingState;
}


/**
 * @brief Calculate RMS level of audio signal
 * @param data
 * @param size
 * @param audioFormat
 */
void AudioIODevice::calcVuLevel(const char *data, int size, const AudioFormat & audioFormat)
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

	switch (audioFormat.sampleFormat()) {
	case AudioFormat::Int16:
		{
			const qint16 *dat = reinterpret_cast<const qint16*>(data);
			frames /= 2;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					const qint16 val = dat[frame*channels+chan];
					const qreal valF = AudioFormat::pcm16ToReal(val);
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
	case AudioFormat::Int32:
		{
			const qint32 *dat = reinterpret_cast<const qint32*>(data);
			frames /= 4;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					const qint32 val = dat[frame*channels+chan];
					const qreal valF = AudioFormat::pcm32ToReal(val);
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

	case AudioFormat::Float:
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
	case AudioFormat::Uint8:
	case AudioFormat::Unknown:
		DEBUGERROR("Sampletype in audiostream unknown or unsupported");
		return;
	}

	left = sqrt(energy[0] / frames);
	right = sqrt(energy[1] / frames);

	// qDebug("left:%f right:%f",left,right);
	if (left/frames > frame_energy_peak)
		frame_energy_peak = left/frames;

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

void AudioIODevice::setLoopCount(int loops)
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
}

QAudioDeviceInfo AudioIODevice::getAudioDeviceInfo(const QString &devName, bool *found)
{
	QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int t=0; t<devList.size(); t++) {
		if (devList.at(t).deviceName() == devName) {
			if (found)
				*found = true;
			return devList.at(t);
		}
	}

	if (found)
		*found = false;

	return QAudioDeviceInfo();
}

void AudioIODevice::calcPanning(char *data, int size, const	AudioFormat &audioFormat)
{
	int channels = audioFormat.channelCount();
	qint64 frames = size / channels;

	switch (audioFormat.sampleFormat()) {
	case AudioFormat::Int16:
		{
			qint16 *dat = reinterpret_cast<qint16*>(data);
			frames /= 2;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					const qint16 val = dat[frame*channels+chan];
					const qreal valF = AudioFormat::pcm16ToReal(val);
					switch (chan) {
					case 0:
						dat[frame*channels+chan] = AudioFormat::realToPcm16(valF * m_panVolLeft);
						break;
					case 1:
						dat[frame*channels+chan] = AudioFormat::realToPcm16(valF * m_panVolRight);
						break;
					}
				}
			}

		}
		break;
	}
}

void AudioIODevice::start(int loops)
{
	QMutexLocker lock(&m_mutex);

	setLoopCount(loops);

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

	// Now start reading and decoding of sound file. Signal "bufferReady" will be
	// emitted when an audio portion is decoded. Audio data will be processed in
	// process_decoder_buffer
	decoding_finished_f = false;
	audio_decoder->start();
	LOGTEXT(tr("Decoding of audio file '%1' started").arg(current_filename));
}

void AudioIODevice::stop()
{
	m_mutex.lock();

	if (audio_decoder->state() == QAudioDecoder::DecodingState) {
		audio_decoder->stop();
		while (audio_decoder->state() == QAudioDecoder::DecodingState) {
			fprintf(stderr, "wait for stop");
		}
	}
	close();

	decoding_finished_f = true;

	m_mutex.unlock();
}

void AudioIODevice::process_decoder_buffer()
{
	m_mutex.lock();

	if (isOpen()) {
		QAudioBuffer audiobuf = audio_decoder->read();
		// AudioFormat form = audiobuf.format();
		const char *data = audiobuf.constData<char>();
		audio_buffer->append(data,audiobuf.byteCount());
		bytes_avail += audiobuf.byteCount();

		// qDebug("processAudio %d: size: %d",audio_buffer_count, frames);
		audio_buffer_count++;
	}

	m_mutex.unlock();
}

void AudioIODevice::on_decoding_finished()
{
	decoding_finished_f = true;
	LOGTEXT(tr("Decoding of audio file '%1' finished (<font color=green>%2ms</font>")
			.arg(current_filename).arg(run_time.elapsed()));
	if (debug)
		qDebug("Audio decoding finished: %s",current_filename.toLocal8Bit().data());
}

void AudioIODevice::if_audio_duration_changed(qint64 duration)
{
	if (duration > 0) {
		emit audioDurationDetected(duration);
	}
}


void AudioIODevice::if_error_occurred(QAudioDecoder::Error error)
{
	DEBUGERROR("An error occurred while decoding audio: %s (error: %d)"
			  ,audio_decoder->errorString().toLocal8Bit().data(),error);
	LOGERROR(tr("Failed file was: %1").arg(audio_decoder->sourceFilename()));
	audio_error = AUDIO::AUDIO_ERR_DECODER;
}


bool AudioDecoder::isDecoding() const
{
#ifdef IS_QT6
	return QAudioDecoder::isDecoding();
#else
	return QAudioDecoder::state() == QAudioDecoder::DecodingState;
#endif
}

void AudioDecoder::setSourceFilename(const QString &filename)
{
#ifdef IS_QT6
	QAudioDecoder::setSource(QUrl::fromLocalFile(filename));
#else
	QAudioDecoder::setSourceFilename(filename);
#endif
}

QString AudioDecoder::sourceFilename() const
{
#ifdef IS_QT6
	return QAudioDecoder::source().toLocalFile();
#else
	return QAudioDecoder::sourceFilename();
#endif
}

#include "audioiodevice.h"
#include "audioformat.h"
#include "../system/log.h"

#include <QTime>
#include <QByteArray>

AudioIODevice::AudioIODevice(AudioFormat format, QObject *parent) :
	QIODevice(parent)
{
	decoding_finished_f = false;
	audio_buffer_count = 0;
	bytes_avail = 0;
	bytes_read = 0;
	frame_energy_peak = 0;
	sample_peak = 0;
	audio_format = new AudioFormat(format);
	audio_buffer = new QByteArray;

	loop_count = 0;
	loop_target = 0;

#ifdef IS_QT5
	audio_decoder = new QAudioDecoder;
	connect(audio_decoder,SIGNAL(bufferReady()),this,SLOT(process_decoder_buffer()));
	connect(audio_decoder,SIGNAL(finished()),this,SLOT(on_decoding_finished()));
	connect(audio_decoder,SIGNAL(error(QAudioDecoder::Error)),this,SLOT(if_error_occurred(QAudioDecoder::Error)));
	connect(audio_decoder,SIGNAL(durationChanged(qint64)),this,SLOT(if_audio_duration_changed(qint64)));

#endif
}

AudioIODevice::~AudioIODevice()
{

#ifdef IS_QT5
	delete audio_decoder;
#endif
	delete audio_buffer;
	delete audio_format;
}

qint64 AudioIODevice::readData(char *data, qint64 maxlen)
{
	qint64 avail = bytes_avail-bytes_read;

	if (avail == 0 && bytes_read > 0 && decoding_finished_f) {
		loop_count++;
		if (loop_count < loop_target || loop_target < 0) {
			if (debug > 1) LOGTEXT(tr("Loop audio file '%1' -> Loop: %2").arg(current_filename).arg(loop_count));
			bytes_read = 0;
			avail = bytes_avail;
		} else {
			qDebug("maxlen %lli, bytes_read: %lli, avail %lli",maxlen,bytes_read,avail);
			emit readReady();
			return 0;
		}
	}

	if (avail == 0 && bytes_read == 0 && !decoding_finished_f) {
		if (run_time.elapsed() > 500) {
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

	// qDebug("readData %lli",maxlen);
	if (maxlen>avail) {
		maxlen = avail;
	}

	memcpy(data, audio_buffer->data()+bytes_read, maxlen);
	bytes_read += maxlen;

	calc_vu_level(data,maxlen);

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
	qDebug("bytesAvail %lli",bytes_avail-bytes_read);
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

void AudioIODevice::calc_vu_level(const char *data, int size)
{
	if (!audio_format) return;

	qint64 left = 0;
	qint64 right = 0;
	int channels = audio_format->channelCount();
	int frames = size / channels;

	if (frames == 0) return;


	switch (audio_format->sampleType()) {
	case QAudioFormat::SignedInt:
	case QAudioFormat::UnSignedInt:
	{
		qint64 energy[4] = {0,0,0,0};
		switch (audio_format->sampleSize()) {
		case 16:
		{
			const qint16 *dat = reinterpret_cast<const qint16*>(data);
			frames /= 2;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					qint16 val = dat[frame*channels+chan];
					if (val > sample_peak) sample_peak = val;
					if (val > 0) {
						energy[chan] += val;
					} else {
						energy[chan] -= val;
					}

				}
			}
		}
			break;
		default:
			DEBUGERROR("Sampletype in audiostream not supported");
			break;
		}
		left = energy[0];
		right = energy[1];
	}
		break;
	case QAudioFormat::Float:
	{
		double energy[4] = {0,0,0,0};
		switch (audio_format->sampleSize()) {
		case 32:
		{
			const float *dat = reinterpret_cast<const float*>(data);
			frames /= 4;

			for (int chan = 0; chan < channels; chan++) {
				for (int frame = 0; frame<frames; frame++) {
					float val = dat[frame*channels+chan];
					if (val > sample_peak) sample_peak = val;
					val *= 32768;
					if (val > 0) {
						energy[chan] += val;
					} else {
						energy[chan] -= val;
					}

				}
			}
		}
			break;
		default:
			DEBUGERROR("Sampletype in audiostream not supported");
			break;
		}
		left = energy[0];
		right = energy[1];
	}
		break;
	case QAudioFormat::Unknown:
		DEBUGERROR("Sampletype in audiostream unknown");
		break;
	}

	//	qDebug("left:%lli right:%lli",left/frames,right/frames);
	if (left/frames > frame_energy_peak) frame_energy_peak = left/frames;


	emit vuLevelChanged(left/frames, right/frames);
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
	loop_count = 0;


	if (!open(QIODevice::ReadOnly)) {
		DEBUGERROR("Could not open Audio IO device");
		return;
	}

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
	LOGTEXT(tr("Decoding of audio file '%1' finished").arg(current_filename));
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
}
#endif


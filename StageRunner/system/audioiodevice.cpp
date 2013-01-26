#include "audioiodevice.h"
#include "audioformat.h"
#include "system/log.h"

#include <QTime>
#include <QByteArray>

AudioIODevice::AudioIODevice(AudioFormat format, QObject *parent) :
	QIODevice(parent)
{
	decoding_finished_f = false;
	audio_buffer_count = 0;
	bytes_avail = 0;
	bytes_read = 0;
	audio_format = new AudioFormat(format);
	audio_buffer = new QByteArray;

#ifdef IS_QT5
	audio_decoder = new QAudioDecoder;
	connect(audio_decoder,SIGNAL(bufferReady()),this,SLOT(process_decoder_buffer()));
	connect(audio_decoder,SIGNAL(finished()),this,SLOT(on_decoding_finished()));

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

	// qDebug("readData %lli",maxlen);
	if (maxlen>avail) {
		qDebug("maxlen, avail, %lli, %lli",maxlen,avail);
		maxlen = avail;
		if (decoding_finished_f) emit readReady();
	}

	memcpy(data, audio_buffer->data()+bytes_read, maxlen);
	bytes_read += maxlen;
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
	audio_decoder->setSourceFilename(filename);
	audio_decoder->setAudioFormat(*audio_format);
	current_filename = filename;

	return true;
}

void AudioIODevice::examineQAudioFormat(AudioFormat &form)
{
	int samplesize = form.sampleSize();
	int channels = form.channelCount();
	int samplerate = form.sampleRate();
	QString codec = form.codec();

	// qDebug("Audioformat: %dHz, %d Channels, Size per sample: %d (Codec:%s)"
	//	   ,samplerate,channels,samplesize,codec.toLatin1().data());

}

void AudioIODevice::start()
{
	open(QIODevice::ReadOnly);

	audio_buffer_count = 0;
	audio_buffer->clear();
	bytes_read = 0;
	bytes_avail = 0;

#ifdef IS_QT5
	decoding_finished_f = false;
	audio_decoder->start();
#endif
	LOGTEXT(tr("Decoding of audio file '%1' started").arg(current_filename));

}

void AudioIODevice::stop()
{
	close();
}

void AudioIODevice::process_decoder_buffer()
{
	QAudioBuffer audiobuf = audio_decoder->read();
	AudioFormat form = audiobuf.format();
	const char *data = audiobuf.constData<char>();
	audio_buffer->append(data,audiobuf.byteCount());
	bytes_avail += audiobuf.byteCount();

	// qDebug("processAudio %d: size: %d",audio_buffer_count, frames);
	audio_buffer_count++;
}

void AudioIODevice::on_decoding_finished()
{
	decoding_finished_f = true;
	LOGTEXT(tr("Decoding of audio file '%1' finished").arg(current_filename));
	if (debug) qDebug("Audio decoding finished");
}


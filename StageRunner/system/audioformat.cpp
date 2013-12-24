#include "audioformat.h"

AudioFormat::AudioFormat() :
	QAudioFormat()
{
}

AudioFormat::AudioFormat(const AudioFormat &other)
	: QAudioFormat()
{
	*this = other;
}

AudioFormat::AudioFormat(const QAudioFormat &other)
	: QAudioFormat(other)
{
}

AudioFormat AudioFormat::defaultFormat()
{
	AudioFormat format;
	format.setSampleRate(48000);
	format.setChannelCount(2);
	format.setSampleType(QAudioFormat::SignedInt);
	format.setSampleSize(16);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");

	return format;
}

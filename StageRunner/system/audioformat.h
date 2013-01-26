#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

#include <QAudioFormat>

class AudioFormat : public QAudioFormat
{
public:
	AudioFormat();
	AudioFormat(const AudioFormat &other);
	AudioFormat(const QAudioFormat &other);

	static AudioFormat defaultFormat();

};

#endif // AUDIOFORMAT_H

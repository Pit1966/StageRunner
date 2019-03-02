#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "commandsystem.h"

#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioBuffer>

class AudioSlot;

class AudioPlayer : public QMediaPlayer
{
	Q_OBJECT

protected:
	AudioSlot & myChannel;
	int m_loopTarget;
	int m_loopCnt;
	int m_currentVolume;
	CtrlCmd m_currentCtrlCmd;
	AUDIO::AudioErrorType m_audioError;
	QString m_mediaPath;

public:
	AudioPlayer(AudioSlot &audioChannel);
	inline AUDIO::AudioErrorType audioError() const {return m_audioError;}
	inline CtrlCmd currentAudioCmd() const {return m_currentCtrlCmd;}
	inline int currentLoop() const {return m_loopCnt;}

	virtual bool setSourceFilename(const QString &path);
	virtual void start(int loops) = 0;
	virtual void stop() = 0;
	virtual void pause(bool state) = 0;
	virtual qint64 currentPlayPosUs() const = 0;
	virtual qint64 currentPlayPosMs() const = 0;
	virtual bool seekPlayPosMs(qint64 posMs) = 0;
	virtual void setVolume(int vol) = 0;

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);

};

#endif // AUDIOPLAYER_H

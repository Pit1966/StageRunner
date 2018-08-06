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
	QAudioProbe *audioProbe;
	int loopTarget;
	int loopCnt;
	QString mediaPath;
	QMediaPlayer::State currentState;
	int currentVolume;
	AUDIO::AudioErrorType m_audioError;

public:
	AudioPlayer(AudioSlot &audioChannel);
	AUDIO::AudioErrorType audioError() const {return m_audioError;}
	bool setSourceFilename(const QString &path);
	void start(int loops);
	void stop();
	inline int currentLoop() const {return loopCnt;}
	qint64 currentPlayPosUs() const;
	qint64 currentPlayPosMs() const;
	bool seekPlayPosMs(qint64 posMs);
	void setVolume(int vol);

private slots:
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_play_state_changed(QMediaPlayer::State state);
	void calculate_vu_level(QAudioBuffer buffer);

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);

};

#endif // AUDIOPLAYER_H

#ifndef MEDIAPLAYERAUDIOBACKEND_H
#define MEDIAPLAYERAUDIOBACKEND_H

#include "system/audioplayer.h"
#include "commandsystem.h"

#include <QMediaPlayer>
#include <QAudioBuffer>
#include <QObject>


class QAudioProbe;

class MediaPlayerAudioBackend : public AudioPlayer
{
	Q_OBJECT
private:
	QMediaPlayer *m_mediaPlayer;
	QAudioProbe *m_audioProbe;
	QMediaPlayer::State m_currentMediaPlayerState;
	AUDIO::AudioStatus m_currentAudioStatus;			///< This is QMediaPlayer::State and QMediaPlayer::MediaStatus translated to StageRunner status

public:
	MediaPlayerAudioBackend(AudioSlot &audioChannel);
	virtual ~MediaPlayerAudioBackend();

	bool setSourceFilename(const QString &path) override;
	void start(int loops) override;
	void stop() override;
	void pause(bool state) override;
	qint64 currentPlayPosUs() const override;
	qint64 currentPlayPosMs() const override;
	bool seekPlayPosMs(qint64 posMs) override;
	void setVolume(int vol) override;
	int volume() const override;
	AUDIO::AudioStatus state() const override;

private slots:
	void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
	void onPlayerStateChanged(QMediaPlayer::State state);
	void onMediaDuratinChanged(qint64 ms);
	void calculateVuLevel(QAudioBuffer buffer);

};

#endif // MEDIAPLAYERAUDIOBACKEND_H

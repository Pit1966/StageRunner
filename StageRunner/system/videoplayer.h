#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>

#include "commandsystem.h"

using namespace AUDIO;

class PsVideoWidget;
class FxClipItem;

class VideoPlayer : public QMediaPlayer
{
	Q_OBJECT
protected:
	PsVideoWidget *m_videoWid;
	int loopTarget;
	int loopCnt;
	QMediaPlayer::State currentState;

public:
	VideoPlayer(PsVideoWidget *videoWid);

private slots:
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_play_state_changed(QMediaPlayer::State state);

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);

	void clipCtrlMsgEmitted(AudioCtrlMsg msg);
	void clipProgressChanged(FxClipItem *fxclip, int perMille);
};

#endif // VIDEOPLAYER_H

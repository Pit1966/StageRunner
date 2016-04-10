#include "videoplayer.h"
#include "customwidget/psvideowidget.h"
#include "log.h"

#include <QMediaPlayer>

VideoPlayer::VideoPlayer(PsVideoWidget *videoWid)
	: QMediaPlayer()
	, m_videoWid(videoWid)
	, loopTarget(1)
	, loopCnt(1)
	, currentState(QMediaPlayer::StoppedState)
{
	setVideoOutput(m_videoWid);

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
}

void VideoPlayer::on_media_status_changed(QMediaPlayer::MediaStatus status)
{
	switch (status) {
	case QMediaPlayer::UnknownMediaStatus:
	case QMediaPlayer::NoMedia:
	case QMediaPlayer::LoadingMedia:
		break;
	case QMediaPlayer::LoadedMedia:
	case QMediaPlayer::StalledMedia:
		break;
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		break;
	case QMediaPlayer::EndOfMedia:
		if (loopCnt < loopTarget) return;
		break;
	case QMediaPlayer::InvalidMedia:
		POPUPERRORMSG(Q_FUNC_INFO, tr("Invalid Media: %1").arg(media().canonicalUrl().toString()));
		break;
	}

	emit statusChanged(status);

	// qDebug() << Q_FUNC_INFO << status;
}

void VideoPlayer::on_play_state_changed(QMediaPlayer::State state)
{
	if (state != currentState) {
		if (state == QMediaPlayer::StoppedState) {
			if (currentState == QMediaPlayer::PlayingState) {
				if (loopCnt < loopTarget) {
					loopCnt++;
					QMediaPlayer::play();
				}
			}
		}
		else if (state == QMediaPlayer::PlayingState) {
			// qDebug("Current volume: %d",volume());
			// QMediaPlayer::setVolume(currentVolume);
		}
		currentState = state;
	}
}

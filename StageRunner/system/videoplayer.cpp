#include "videoplayer.h"
#include "customwidget/psvideowidget.h"
#include "log.h"
#include "fxclipitem.h"

#include <QMediaPlayer>

VideoPlayer::VideoPlayer(PsVideoWidget *videoWid)
	: QMediaPlayer()
	, m_videoWid(videoWid)
	, loopTarget(1)
	, loopCnt(1)
	, currentState(QMediaPlayer::StoppedState)
	, m_currentFxClipItem(0)
{
	setVideoOutput(m_videoWid);

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
	connect(this,SIGNAL(positionChanged(qint64)),this,SLOT(on_playback_position_changed(qint64)));
	setNotifyInterval(20);

	connect(this,SIGNAL(endReached(qint64)),this,SLOT(pause()),Qt::QueuedConnection);
	connect(this,SIGNAL(seekMe(qint64)),this,SLOT(setPosition(qint64)),Qt::QueuedConnection);
}

bool VideoPlayer::playFxClip(FxClipItem *fxc)
{
	loopTarget = fxc->loopTimes;
	loopCnt = 1;

	m_currentFxClipItem = fxc;
	this->setMedia(QUrl::fromLocalFile(fxc->filePath()));
	m_videoWid->show();
	m_videoWid->raise();
	this->play();

//	qDebug() << Q_FUNC_INFO << thread();

	return true;
}

void VideoPlayer::stop()
{
	loopCnt = loopTarget;
	QMediaPlayer::stop();
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

//	qDebug() << Q_FUNC_INFO << status;
}

void VideoPlayer::on_play_state_changed(QMediaPlayer::State state)
{
	bool restart = false;

	if (state != currentState) {
//		qDebug() << Q_FUNC_INFO << state << thread();
		if (state == QMediaPlayer::StoppedState) {
			if (currentState == QMediaPlayer::PlayingState) {
				if (loopCnt < loopTarget) {
					loopCnt++;
					restart = true;
				} else {
					if (m_currentFxClipItem && m_currentFxClipItem->blackAtVideoEnd) {
						QMediaPlayer::stop();
						m_videoWid->update();
					}
				}
			}
		}
		else if (state == QMediaPlayer::PlayingState) {
			// qDebug("Current volume: %d",volume());
			// QMediaPlayer::setVolume(currentVolume);
		}
		currentState = state;
	}

	if (restart)
		QMediaPlayer::play();
}

void VideoPlayer::on_playback_position_changed(qint64 pos)
{
	if (!m_currentFxClipItem) return;

	int dur = duration();
	if (dur <= 0) return;

	int permille = pos * 1000 / dur;

//	qDebug() << "pos" << pos << dur << permille/10 << thread();

	if (!m_currentFxClipItem->blackAtVideoEnd) {
		if (pos >= dur - 120) {
			if (loopCnt < loopTarget) {
				loopCnt++;
				emit seekMe(0);
			} else {
				if (currentState != QMediaPlayer::PausedState) {
					emit endReached(pos);
					//				emit seekMe(100);
				}
			}
		}
	}

	emit clipProgressChanged(m_currentFxClipItem, permille);
}

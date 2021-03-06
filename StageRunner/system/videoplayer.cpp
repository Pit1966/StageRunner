//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "videoplayer.h"
#include "videocontrol.h"
#include "customwidget/psvideowidget.h"
#include "log.h"
#include "fxclipitem.h"

#include <QMediaPlayer>
#include <QElapsedTimer>
#include <QApplication>

VideoPlayer::VideoPlayer(VideoControl *parent, PsVideoWidget *videoWid)
	: QMediaPlayer()
	, m_videoCtrl(parent)
	, m_videoWid(videoWid)
	, loopTarget(1)
	, loopCnt(1)
	, m_slotNumber(-1)
	, m_currentVolume(-1)
	, m_currentMasterVolume(-1)
	, currentState(QMediaPlayer::StoppedState)
	, m_currentFxClipItem(nullptr)
{
	setVideoOutput(m_videoWid);

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
	connect(this,SIGNAL(positionChanged(qint64)),this,SLOT(on_playback_position_changed(qint64)));
	setNotifyInterval(20);

	connect(this,SIGNAL(seekMe(qint64)),this,SLOT(setPosition(qint64)),Qt::QueuedConnection);

	connect(this, SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)), m_videoCtrl, SIGNAL(videoCtrlMsgEmitted(AudioCtrlMsg)));

	m_videoWid->setVideoPlayer(this);
}

bool VideoPlayer::playFxClip(FxClipItem *fxc, int slotNum)
{
	loopTarget = fxc->loopTimes;
	loopCnt = 1;
	m_slotNumber = slotNum;

	m_currentFxClipItem = fxc;
	this->setMedia(QUrl::fromLocalFile(fxc->filePath()));
	m_videoWid->show();
	m_videoWid->raise();	
	this->play();

	if (m_videoWid->size().isNull())
		m_videoWid->resize(700,500);

//	qDebug() << "videowidget geo" << m_videoWid->geometry();
//	qDebug() << Q_FUNC_INFO << thread();

	return true;
}

bool VideoPlayer::isRunning() const
{
	return currentState == QMediaPlayer::PlayingState;
}

void VideoPlayer::stop()
{
	loopCnt = loopTarget;
	QMediaPlayer::stop();
}

/**
 * @brief Stop video and wait for stopped;
 * @return true, if video was stopped within 500ms.
 */
bool VideoPlayer::stopAndWait()
{
	stop();
	QElapsedTimer time;
	time.start();
	while (isRunning() && time.elapsed() < 500) {
		QApplication::processEvents();
	}

	return time.elapsed() < 500;
}

/**
 * @brief Set volume of video player
 * @param vol value in application range (0 - MAX_VOLUME)
 *
 * The volume value will be translated to QMediaPlayer range and also the master volume is taken into account
 */
void VideoPlayer::setVolume(int vol)
{
	float level = float(vol * 100) / m_videoCtrl->maxVolume();
	if (m_currentMasterVolume >= 0) {
		level = level * m_currentMasterVolume / m_videoCtrl->maxVolume();
	}

	// qDebug() << "set volume" << vol << "master" << m_currentMasterVolume << "final" << level;

	QMediaPlayer::setVolume(level);
	m_currentVolume = vol;
}

void VideoPlayer::setMasterVolume(int vol)
{
	// qDebug() << "set master volume" << vol << "current vol" << m_currentVolume;
	m_currentMasterVolume = vol;
	setVolume(m_currentVolume);
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
	bool restart = false;
	AudioStatus stat = AudioStatus::AUDIO_NO_STATE;

	if (state != currentState) {
		qDebug() << Q_FUNC_INFO << state << thread();
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
					stat = VIDEO_IDLE;
				}
			}
		}
		else if (state == QMediaPlayer::PlayingState) {
			// qDebug("Current volume: %d",volume());
			// QMediaPlayer::setVolume(currentVolume);
			stat = VIDEO_RUNNING;
		}
		else if (state == QMediaPlayer::PausedState) {
			stat = VIDEO_IDLE;
		}
		currentState = state;
	}

	if (restart)
		QMediaPlayer::play();

	if (stat > AUDIO_NO_STATE) {
		if (stat == VIDEO_IDLE) {
			onVideoEnd();
		} else {
			qDebug() << "1 video stat" << stat;
			AudioCtrlMsg msg(m_slotNumber, CMD_VIDEO_STATUS_CHANGED, stat);
			msg.fxAudio = m_currentFxClipItem;
			emit audioCtrlMsgEmitted(msg);
		}
	}
}

void VideoPlayer::on_playback_position_changed(qint64 pos)
{
	if (!m_currentFxClipItem) return;

	if (currentState != QMediaPlayer::PlayingState)
		return;

	qint64 dur = duration();
	if (dur <= 0) return;

	int permille = int(pos * 1000 / dur);

	// qDebug() << "pos" << pos << dur << permille/10 << thread();

	if (!m_currentFxClipItem->blackAtVideoEnd) {
		// This is a workaround, for holding the last video frame at video end.
		// What we do is, stopping the video 120ms before end is reached!
		if (pos >= dur - 120) {
			if (loopCnt < loopTarget) {
				loopCnt++;
				emit seekMe(0);
			} else {
				if (currentState != QMediaPlayer::PausedState) {
					QMediaPlayer::pause();
					return;
				}
			}
		}
	}

	emit clipProgressChanged(m_currentFxClipItem, permille);

	AudioCtrlMsg msg(m_currentFxClipItem, m_slotNumber, CMD_VIDEO_STATUS_CHANGED);
	msg.currentAudioStatus = currentState == QMediaPlayer::PlayingState ? VIDEO_RUNNING : VIDEO_IDLE;
	msg.progress = permille;
	msg.progressTime = int(pos);
	msg.loop = loopCnt;
	// msg.executer = nullptr;
	if (m_currentFxClipItem->loopTimes > 1) {
		msg.maxloop = m_currentFxClipItem->loopTimes;
	}

	// qDebug() << "2 video stat" << currentState << currentState << permille;

	emit audioCtrlMsgEmitted(msg);
}

void VideoPlayer::onVideoEnd()
{
	qDebug() << "3 video stat" << VIDEO_IDLE;

	AudioCtrlMsg msg(m_currentFxClipItem, m_slotNumber, CMD_VIDEO_STATUS_CHANGED);
	msg.currentAudioStatus = VIDEO_IDLE;
	msg.progress = 1000;
	// msg.progressTime = int(pos);
	msg.loop = loopCnt;
	// msg.executer = nullptr;
	emit audioCtrlMsgEmitted(msg);
}

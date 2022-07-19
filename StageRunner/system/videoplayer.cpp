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

using namespace VIDEO;

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
	, m_viewState(VIEW_UNUSED)
	, m_currentFxClipItem(nullptr)
	, m_lastFxClipItem(nullptr)
	, m_overlayFadeOutFirst(false)
	, m_stopVideoAtEventEnd(false)
{
	setVideoOutput(m_videoWid);

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
	connect(this,SIGNAL(positionChanged(qint64)),this,SLOT(on_playback_position_changed(qint64)));
	setNotifyInterval(20);

	connect(this,SIGNAL(seekMe(qint64)),this,SLOT(setPosition(qint64)),Qt::QueuedConnection);
	connect(this, SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)), m_videoCtrl, SIGNAL(videoCtrlMsgEmitted(AudioCtrlMsg)));

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		// m_overlayFadeTimeLine[i].setObjectName(QString("picover%1").arg(i));

		connect(&m_overlayFadeTimeLine[i], &QTimeLine::valueChanged, [this, i](qreal newValue) { this->setOverlayFade(newValue, i);});
		connect(&m_overlayFadeTimeLine[i], &QTimeLine::finished, [this, i]() { this->setOverlayFadeFinished(i); });
	}

	m_videoWid->setVideoPlayer(this);
}


/**
 * @brief VideoPlayer::playFxClip
 * @param fxc
 * @param slotNum
 * @return
 *
 * Not only pure videos are started here. A FxClipItem can also be a still picture.
 * This is handled differnt. The picture will be displayed in an overlay QWidget over the VideoPlayer widget.
 * An still image has set the property isPicClip.
 */
bool VideoPlayer::playFxClip(FxClipItem *fxc, int slotNum)
{
	loopTarget = fxc->loopTimes;
	loopCnt = 1;
	m_slotNumber = slotNum;
	qDebug() << "playFxClip" << fxc->name() << "viewState" << m_viewState;

	m_lastFxClipItem = m_currentFxClipItem;
	m_currentFxClipItem = fxc;

	bool ok = true;
	if (fxc->isPicClip) {
		ok = playPicClip(fxc);
	}
	else {
		this->setMedia(QUrl::fromLocalFile(fxc->filePath()));
		if (m_viewState >= VIEW_PIC_VISIBLE) {
			if (m_videoWid->isPicClipVisible(0))
				fadePicClipOverlayOut(2000, 0);
			if (m_videoWid->isPicClipVisible(1))
				fadePicClipOverlayOut(2000, 1);
		}
		else {
			m_videoWid->setPicClipOverlaysActive(false);
			m_videoWid->show();
			m_videoWid->raise();
		}
		m_viewState = VIEW_VIDEO_VISIBLE;
		this->play();
	}

	if (m_videoWid->size().isNull())
		m_videoWid->resize(700,500);

//	qDebug() << "videowidget geo" << m_videoWid->geometry();
//	qDebug() << Q_FUNC_INFO << thread();

	return ok;
}

bool VideoPlayer::isRunning() const
{
	return currentState == QMediaPlayer::PlayingState;
}

bool VideoPlayer::isCurrentPicture() const
{
	if (m_currentFxClipItem)
		return m_currentFxClipItem->isPicClip;

	return false;
}

void VideoPlayer::stop()
{
	loopCnt = loopTarget;
	if (m_currentFxClipItem && m_currentFxClipItem->isPicClip) {
		onVideoEnd();
	}
	else {
		QMediaPlayer::stop();
	}
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

void VideoPlayer::setPicClipOverlaysDisabled()
{
	if (m_videoWid->isPicClipOverlaysActive())
		m_videoWid->setPicClipOverlaysActive(false);
}

bool VideoPlayer::isPicClipOverlaysActive()
{
	return m_videoWid->isPicClipOverlaysActive();
}

bool VideoPlayer::fadePicClipOverlayOut(int ms, int layer)
{
	if (!m_videoWid->isPicClipOverlaysActive())
		return false;

	if (m_viewState < VIEW_BLACK)
		return false;

	if (ms < 0)
		ms = 0;

	if (m_videoWid->isPicClipVisible(layer)) {
		if (ms == 0) {
			m_overlayFadeTimeLine[layer].stop();
			m_videoWid->setPicClipOverlayOpacity(0.0, layer);
			m_viewState = VIEW_BLACK;
		}
		else {
			m_viewState = VIEW_PIC_FADEOUT;
			m_overlayFadeTimeLine[layer].setDirection(QTimeLine::Backward);
			m_overlayFadeTimeLine[layer].setDuration(ms);
			m_overlayFadeTimeLine[layer].start();
		}

		return true;
	}

	return false;
}

bool VideoPlayer::fadePicClipOverlayIn(int ms, int layer)
{
	if (!m_videoWid->isPicClipOverlaysActive())
		return false;

	if (ms < 0)
		ms = 0;

	m_viewState = VIEW_PIC_FADEIN;
	m_overlayFadeTimeLine[layer].setDirection(QTimeLine::Forward);
	m_overlayFadeTimeLine[layer].setDuration(ms);
	m_overlayFadeTimeLine[layer].start();

	return true;
}

bool VideoPlayer::fadeVideoToBlack(int ms)
{
	if (!isRunning())
		return false;

	m_videoWid->setPicClipOverlayOpacity(0, 0);	// set opacity for all overlay layers to 0
	m_videoWid->setPicClipOverlayOpacity(0, 1);	// set opacity for all overlay layers to 0
	m_videoWid->clearPicClipOverlayImage();		// remove all overlay PicClips
	m_videoWid->setPicClipOverlaysActive(true);
	bool ok = true;
	ok &= m_videoCtrl->fadeOutFxClipAudio(m_slotNumber, 0, ms);
	ok &= fadePicClipOverlayIn(ms, 0);

	m_stopVideoAtEventEnd = true;
	m_viewState = VIEW_VIDEO_FADEOUT;

	return ok;
}

bool VideoPlayer::playPicClip(FxClipItem *fxc)
{
	// pause playback, if it is a picture in order to get a still image


	if (m_viewState & VIEW_FADE_ACTIVE) {
		stopAllOverlayFades();
	}

	if (m_videoWid->isPicClipOverlaysActive()) {
		if (m_videoWid->isPicPathVisible(fxc->filePath())) {
			LOGTEXT(tr("<font color=green>Set Overlay</font> <b>%1</b> <font color=orange> already visible</font>")
					.arg(fxc->fileName()));
			return false;
		}

		// crossfade
		int fadetime = 4000;
		// m_overlayFadeOutFirst = true;

		if (m_videoWid->isPicClipVisible(1)) {
			fadePicClipOverlayOut(fadetime, 1);

			if (!m_overlayFadeOutFirst) {
				m_videoWid->setPicClipOverlayImage(fxc->filePath(), 0, 0.0);
				if (!fadePicClipOverlayIn(fadetime, 0))
					return false;
				LOGTEXT(tr("<font color=green>Crossfade Overlay</font> <b>%1</b> <font color=green> to back layer</font>")
						.arg(fxc->fileName()));
			}
			else {
				QString oldname;
				if (m_lastFxClipItem)
					oldname = m_lastFxClipItem->fileName();
				LOGTEXT(tr("<font color=green>Fade out back overlay</font> <b>%1</b>").arg(oldname));
			}

		}
		else {
			fadePicClipOverlayOut(fadetime, 0);

			if (!m_overlayFadeOutFirst) {

				m_videoWid->setPicClipOverlayImage(fxc->filePath(), 1, 0.0);
				if (!fadePicClipOverlayIn(fadetime, 1))
					return false;
				LOGTEXT(tr("<font color=green>Crossfade overlay</font> <b>%1</b> <font color=green> to top layer</font>")
						.arg(fxc->fileName()));
			}
			else {
				QString oldname;
				if (m_lastFxClipItem)
					oldname = m_lastFxClipItem->fileName();
				LOGTEXT(tr("<font color=green>Fade out top overlay</font> <b>%1</b>").arg(oldname));
			}
		}

	} else {
		// this is first initialisation of a picture as overlay over videoplayer
		// set picture in overlay number 1 and fade in

		int fadetime = 2000;

		m_videoWid->setPicClipOverlayImage(fxc->filePath(), 1, 0.6);
		m_videoWid->setPicClipOverlaysActive(true);
		m_videoWid->raisePicClipOverlay();
		if (!fadePicClipOverlayIn(fadetime, 1))
			return false;

		LOGTEXT(tr("<font color=green>Set Overlay</font> <b>%1</b> <font color=green> to top layer</font>")
				.arg(fxc->fileName()));

	}

	return true;
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
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		POPUPERRORMSG(Q_FUNC_INFO, tr("Invalid Media: %1").arg(media().request().url().toString()));
#else
		POPUPERRORMSG(Q_FUNC_INFO, tr("Invalid Media: %1").arg(media().canonicalUrl().toString()));
#endif
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
	msg.loop = loopCnt;
	emit audioCtrlMsgEmitted(msg);
}

/**
 * @brief Set opacity of pic overlay
 * @param val [0:1.0f]   0 means fully transparent
 * @param layer
 *
 * @note this function is called by QTimeLine events
 */
void VideoPlayer::setOverlayFade(qreal val, int layer)
{
	m_videoWid->setPicClipOverlayOpacity(val, layer);
}

/**
 * @brief VideoPlayer::setOverlayFadeFinished
 * @param layer
 *
 * @note this function is called by QTimeLine events
 */
void VideoPlayer::setOverlayFadeFinished(int layer)
{
	int cur_state = m_viewState;

	if (m_stopVideoAtEventEnd) {
		m_stopVideoAtEventEnd = false;
		// stop();
		QMediaPlayer::stop();
	}

	if (m_overlayFadeOutFirst) {
		m_overlayFadeOutFirst = false;

		if (m_currentFxClipItem)
			m_videoWid->setPicClipOverlayImage(m_currentFxClipItem->filePath(), layer, 0);

		if (fadePicClipOverlayIn(3000, layer)) {
			LOGTEXT(tr("<font color=green>Fade in %1 overlay</font> <b>%2</b>")
					.arg(layer == 1 ? QString("top") : QString("back"), m_currentFxClipItem->fileName()));
		}

	}
	else {
		if (m_viewState == VIEW_PIC_FADEIN) {
			m_viewState = VIEW_PIC_VISIBLE;
		}
		else if (m_viewState == VIEW_PIC_FADEOUT) {
			m_viewState = VIEW_BLACK;
		}
		else if (m_viewState == VIEW_VIDEO_FADEOUT) {
			m_viewState = VIEW_BLACK;
			if (m_currentFxClipItem) {
				LOGTEXT(tr("<font color=green>VIDEO fade out end</font> <b>%1</b>").arg(m_currentFxClipItem->fileName()));
				m_lastFxClipItem = m_currentFxClipItem;
				m_currentFxClipItem = nullptr;
			}
		}
	}
	qDebug() << "layer" << layer << "set overlay finished" << cur_state << m_viewState;
}

void VideoPlayer::stopAllOverlayFades()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlayFadeTimeLine[i].stop();
	}
}

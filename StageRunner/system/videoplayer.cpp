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
#include "appcontrol/usersettings.h"
#include "appcontrol/appcentral.h"

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
	, m_ctrlFxItem(nullptr)
	, m_overlayFadeOutFirst(false)
	, m_stopVideoAtEventEnd(false)
{
	setVideoOutput(m_videoWid);

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
	connect(this,SIGNAL(positionChanged(qint64)),this,SLOT(on_playback_position_changed(qint64)));
	setNotifyInterval(20);

	connect(this,SIGNAL(seekMe(qint64)),this,SLOT(setPosition(qint64)),Qt::QueuedConnection);
	connect(this, SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)), m_videoCtrl, SIGNAL(videoCtrlMsgEmitted(AUDIO::AudioCtrlMsg)));

	m_ctrlTimer.setSingleShot(true);
	connect(&m_ctrlTimer, SIGNAL(timeout()), this, SLOT(onCtrlTimerFinished()));
	connect(this, SIGNAL(viewStateChanged(int,int)), m_videoCtrl, SIGNAL(mainVideoViewStateChanged(int,int)));

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		// m_overlayFadeTimeLine[i].setObjectName(QString("picover%1").arg(i));

		connect(&m_overlayFadeTimeLine[i], &QTimeLine::valueChanged, [this, i](qreal newValue) { this->setOverlayFade(newValue, i);});
		connect(&m_overlayFadeTimeLine[i], &QTimeLine::finished, [this, i]() { this->setOverlayFadeFinished(i); });
	}

	m_videoWid->setVideoPlayer(this);
}

FxClipItem *VideoPlayer::currentFxClipItem()
{
	if (!FxItem::exists(m_currentFxClipItem))
		m_currentFxClipItem = nullptr;

	return m_currentFxClipItem;
}

QString VideoPlayer::viewStateToString(VideoViewStatus viewState)
{
	switch (viewState) {
	case VIEW_UNUSED:
		return "UNUSED";
	case VIEW_ERROR:
		return "ERROR";
	case VIEW_BLACK:
		return "BLACK";
	case VIEW_VIDEO_VISIBLE:
		return "VIDEO visible";
	case VIEW_VIDEO_FADEIN:
		return "VIDEO fadein";
	case VIEW_VIDEO_FADEOUT:
		return "VIDEO fadeout";
	case VIEW_PIC_VISIBLE:
		return "PIC visible";
	case VIEW_PIC_FADEIN:
		return "PIC fadein";
	case VIEW_PIC_FADEOUT:
		return "PIC fadeout";
	case VIEW_FADE_STOPPED:
		return "fade stopped";
	case VIEW_PRE_DELAY:
		return "pre delay";
	case VIEW_HOLD:
		return "hold";

	default:
		int state = viewState;
		int bit = 0;
		QString status;
		while (state > 0) {
			int val = 1<<bit;
			if (state & val) {
				state -= val;
				if (!status.isEmpty())
					status += ";";
				status += viewStateToString(VideoViewStatus(val));
			}

			bit++;
		}

		return status;
	}
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
	qDebug() << "playFxClip" << fxc->name() << "viewState" << m_viewState << thread();

	if (m_ctrlFxItem != fxc) {
		if (m_ctrlFxItem == nullptr) {
			if (fxc->preDelay() > 0) {
				setViewState(VIEW_PRE_DELAY);
				m_ctrlTimer.start(fxc->preDelay());
				m_ctrlFxItem = fxc;

				LOGTEXT(tr("<font color=green>Start</font> <b>%1</b> with pre delay of %2ms")
						.arg(fxc->name())
						.arg(fxc->preDelay()));

				return true;
			}
		}
		else {
			LOGTEXT(tr("<font color=orange>Start</font> <b>%1</b> canceled, since a ctrl command is running")
					.arg(fxc->name()));
			return false;
		}
	}

	bool ok = true;
	if (fxc->isPicClip) {
		ok = _playPicClip(fxc, m_currentFxClipItem);
	}
	else {
		ok = _playVideoClip(fxc, m_currentFxClipItem, slotNum);
	}

	if (ok)
		m_currentFxClipItem = fxc;

	if (m_videoWid->size().isNull())
		m_videoWid->resize(700,500);

	return ok;
}

bool VideoPlayer::isRunning() const
{
	return currentState == QMediaPlayer::PlayingState;
}

bool VideoPlayer::isCurrentFxClipAPicClip() const
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

	if (!isRunning())
		setViewState(VIEW_BLACK);
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
			setViewState(VIEW_PIC_FADEOUT);
			m_overlayFadeTimeLine[layer].stop();
			m_videoWid->setPicClipOverlayOpacity(0.0, layer);
			setOverlayFadeFinished(layer);
		}
		else {
			setViewState(VIEW_PIC_FADEOUT);
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

	if (ms == 0) {
		setViewState(VIEW_PIC_FADEIN);
		m_overlayFadeTimeLine[layer].stop();
		m_videoWid->setPicClipOverlayOpacity(1.0, layer);
		setOverlayFadeFinished(layer);
	}
	else {
		setViewState(VIEW_PIC_FADEIN);
		m_overlayFadeTimeLine[layer].setDirection(QTimeLine::Forward);
		m_overlayFadeTimeLine[layer].setDuration(ms);
		m_overlayFadeTimeLine[layer].start();
	}

	return true;
}

bool VideoPlayer::fadeVideoToBlack(int ms)
{
	if (!isRunning())
		return false;

	m_videoWid->clearPicClipOverlayImage();		// remove all overlay PicClips
	m_videoWid->setPicClipOverlayOpacity(0);	// set opacity for all overlay layers to 0
	// m_videoWid->setPicClipOverlayImage(":/gfx/pics/stonechip_01.png", 0, 0.0);
	// m_videoWid->setPicClipOverlayImage(":/gfx/pics/screen_black.png", 0, 0.0);
	m_videoWid->setPicClipOverlaysActive(true);
	bool ok = true;
	ok &= m_videoCtrl->fadeOutFxClipAudio(m_slotNumber, 0, ms);
	ok &= fadePicClipOverlayIn(ms, 0);

	m_stopVideoAtEventEnd = true;
	setViewState(VIEW_VIDEO_FADEOUT);

	return ok;
}

void VideoPlayer::clearViewState()
{
	setViewState(VIDEO::VIEW_UNUSED);
}

void VideoPlayer::setViewState(VIDEO::VideoViewStatus state, bool addState)
{
	if (addState)
		state = VIDEO::VideoViewStatus( int(m_viewState) | int(state) );

	if (state != m_viewState) {
		int oldstate = m_viewState;
		m_viewState = state;
		emit viewStateChanged(state, oldstate);
	}
}

void VideoPlayer::fadeOutCurrent(int ms)
{
	if (m_viewState & VIEW_VIDEO_VISIBLE) {
		fadeVideoToBlack(ms);
	}
	else if (m_viewState & VIEW_PIC_VISIBLE) {
		if (m_videoWid->isPicClipVisible(0))
			fadePicClipOverlayOut(ms, 0);
		if (m_videoWid->isPicClipVisible(1))
			fadePicClipOverlayOut(ms, 1);
	}
}

bool VideoPlayer::_playPicClip(FxClipItem *fxc, FxClipItem *old_fxc)
{
	if (m_viewState & VIEW_FADE_ACTIVE) {
		stopAllOverlayFades();
		setViewState(VIEW_FADE_STOPPED);
	}

	if (m_videoWid->isPicClipOverlaysActive()) {
		if (m_videoWid->isPicPathVisible(fxc->filePath())) {
			LOGTEXT(tr("<font color=green>Set Overlay</font> <b>%1</b> <font color=orange> already visible</font>")
					.arg(fxc->fileName()));
			return false;
		}

		// crossfade
		int fadeintime = fxc->fadeInTime();
		int fadeouttime = fadeintime;
		if (old_fxc && old_fxc->fadeOutTime() > 0)
			fadeouttime = old_fxc->fadeOutTime();

		// m_overlayFadeOutFirst = true;

		if (m_videoWid->isPicClipVisible(1)) {
			fadePicClipOverlayOut(fadeouttime, 1);

			if (!m_overlayFadeOutFirst) {
				m_videoWid->setPicClipOverlayImage(fxc->filePath(), 0, 0.0);
				if (!fadePicClipOverlayIn(fadeintime, 0))
					return false;
				LOGTEXT(tr("<font color=green>Crossfade Overlay</font> <b>%1</b> <font color=green> to back layer</font>")
						.arg(fxc->fileName()));
			}
			else {
				QString oldname;
				if (old_fxc)
					oldname = old_fxc->fileName();
				LOGTEXT(tr("<font color=green>Fade out back overlay</font> <b>%1</b>").arg(oldname));
			}

		}
		else {
			fadePicClipOverlayOut(fadeouttime, 0);

			if (!m_overlayFadeOutFirst) {

				m_videoWid->setPicClipOverlayImage(fxc->filePath(), 1, 0.0);
				if (!fadePicClipOverlayIn(fadeintime, 1))
					return false;
				LOGTEXT(tr("<font color=green>Crossfade overlay</font> <b>%1</b> <font color=green> to top layer</font>")
						.arg(fxc->fileName()));
			}
			else {
				QString oldname;
				if (old_fxc)
					oldname = old_fxc->fileName();
				LOGTEXT(tr("<font color=green>Fade out top overlay</font> <b>%1</b>").arg(oldname));
			}
		}

	}
	else if (isRunning()) {
		// video is running ....

		int fadeintime = 4000;		// fxc->fadeInTime();
		int audiofadeouttime = 2000;

		qDebug() << "video is running";
		if (m_viewState & VIEW_VIDEO_SHOWN_OR_DELAY) {
			m_videoWid->setPicClipOverlayImage(fxc->filePath(), 1, 0.0);
			m_videoWid->setPicClipOverlaysActive(true);
			bool ok = true;
			ok &= m_videoCtrl->fadeOutFxClipAudio(m_slotNumber, 0, audiofadeouttime);
			m_stopVideoAtEventEnd = true;
			ok &= fadePicClipOverlayIn(fadeintime, 1);
			if (!ok)
				return false;
		}
		else {
			qWarning() << "VideoPlayer::playPicClip: video is running but mode is not VIDEO_VISIBLE";
		}

	}
	else {
		// this is first initialisation of a picture as overlay over videoplayer
		// set picture in overlay number 1 and fade in

		int fadeintime = fxc->fadeInTime();

		m_videoWid->setPicClipOverlayImage(fxc->filePath(), 1, 0.0);
		m_videoWid->setPicClipOverlaysActive(true);
		m_videoWid->raisePicClipOverlay();
		if (!fadePicClipOverlayIn(fadeintime, 1))
			return false;

		LOGTEXT(tr("<font color=green>Set Overlay</font> <b>%1</b> <font color=green> to top layer</font>")
				.arg(fxc->fileName()));

	}

	return true;
}

bool VideoPlayer::_playVideoClip(FxClipItem *fxc, FxClipItem *old_fxc, int slotNum)
{
	Q_UNUSED(old_fxc)

	// handle audio track of video first
	QMultimedia::AvailabilityStatus astat = availability();
	Q_UNUSED(astat)

	// set volume here in media player
	setVolume(fxc->initialVolume);
	// and propagate audio volume to AudioControl unit and audio level widgets
	if (!m_videoCtrl->startFxClipItemInSlot(fxc, slotNum))
		return false;


	// now set media file
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

		UserSettings *set = AppCentral::ref().userSettings;
		if (set->pVideoOutXSize > 0 && set->pVideoOutYSize > 0) {
			m_videoWid->resize(set->pVideoOutXSize, set->pVideoOutYSize);
		}

		if (set->pVideoForceSecondDesktop && AppCentral::ref().hasSecondScreen()) {
			QPoint secCenterPos = AppCentral::ref().secondScreenCenterPoint();
			secCenterPos -= QPoint(m_videoWid->width() / 2, m_videoWid->height() / 2);
			m_videoWid->move(secCenterPos);
			// qDebug() << "final video win pos" << m_videoWid->pos();
		}
	}
	setViewState(VIEW_VIDEO_VISIBLE);
	this->play();

	return true;
}

bool VideoPlayer::fadeInCurrentFxClipItem(int ms, int layer)
{
	if (FxItem::exists(m_currentFxClipItem)) {
		m_videoWid->setPicClipOverlaysActive(true);
		m_videoWid->setPicClipOverlayImage(m_currentFxClipItem->filePath(), layer, 0);
		int fadeintime = 3000;
		if (ms < 0)
			fadeintime = m_currentFxClipItem->fadeInTime();

		if (fadePicClipOverlayIn(fadeintime, layer)) {
			LOGTEXT(tr("<font color=green>Fade in %1 overlay</font> <b>%2</b>")
					.arg(layer == 1 ? QString("top") : QString("back"), m_currentFxClipItem->fileName()));
			return true;
		}
	}

	return false;
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
						setViewState(VIEW_BLACK);
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
	switch (m_viewState) {
	case VIEW_PRE_DELAY:
	case VIEW_HOLD:
		break;
	default:
		m_videoWid->setPicClipOverlayOpacity(val, layer);
	}

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
		fadeInCurrentFxClipItem(3000, layer);
	}
	else {
		if (m_viewState == VIEW_PIC_FADEIN) {
			setViewState(VIEW_PIC_VISIBLE);
			if (FxItem::exists(m_currentFxClipItem) && m_currentFxClipItem->holdTime() > 0) {
				setViewState(VIEW_HOLD, true);
				m_ctrlTimer.start(m_currentFxClipItem->holdTime());
				LOGTEXT(tr("<font color=green>HOLD</font> <b>%1</b> for %2ms")
						.arg(m_currentFxClipItem->name()).arg(m_currentFxClipItem->holdTime()));
			}
		}
		else if (m_viewState == VIEW_PIC_FADEOUT) {
			setViewState(VIEW_BLACK);
		}
		else if (m_viewState == VIEW_VIDEO_FADEOUT) {
			setViewState(VIEW_BLACK);
			if (FxItem::exists(m_currentFxClipItem) && m_currentFxClipItem) {
				LOGTEXT(tr("<font color=green>VIDEO fade out end</font> <b>%1</b>").arg(m_currentFxClipItem->fileName()));
				m_currentFxClipItem = nullptr;
			}
		}

		if (m_viewState == VIEW_VIDEO_VISIBLE) {
			if (!m_videoWid->isPicClipVisible())
				m_videoWid->setPicClipOverlaysActive(false);
		}

	}

	qDebug() << "layer" << layer << "set overlay finished" << cur_state << m_viewState;
}

void VideoPlayer::onCtrlTimerFinished()
{
	if (m_viewState & VIEW_PRE_DELAY) {
		if (FxItem::exists(m_ctrlFxItem)) {
			playFxClip(m_ctrlFxItem, m_slotNumber);
			m_ctrlFxItem = nullptr;
			return;
		}
	}
	else if (m_viewState & VIEW_HOLD) {
		if (FxItem::exists(m_currentFxClipItem)) {
			fadeOutCurrent(m_currentFxClipItem->fadeOutTime());
			return;
		}
	}

	setViewState(VIEW_ERROR);
}


void VideoPlayer::stopAllOverlayFades()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlayFadeTimeLine[i].stop();
	}
}

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

#include "videocontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "fxclipitem.h"
#include "audiocontrol.h"
#include "audioslot.h"
#include "customwidget/psvideowidget.h"

#ifdef IS_QT5
#	include "videoplayer.h"
#endif

#include <QElapsedTimer>
#include <QApplication>

VideoControl::VideoControl(AppCentral &app_central)
	: QObject()
	, myApp(app_central)
{
}

/**
 * @brief Get master volume from audio control
 * @return 0 - MAX_VOLUME
 */
int VideoControl::masterVolume() const
{
	return myApp.unitAudio->masterVolume();
}

/**
 * @brief Get max volume range (convenience function)
 * @return MAX_VOLUME
 */
int VideoControl::maxVolume() const
{
	return MAX_VOLUME;
}

VideoPlayer *VideoControl::videoSlot(int slotNum)
{
	Q_UNUSED(slotNum)

	return myApp.unitAudio->videoPlayer();
}

bool VideoControl::startFxClipById(qint32 id)
{
	FxItem *fx = FxItem::findFxById(id);
	if (!fx) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("FX Id does not exist in start command!"));
		return false;
	}
	FxClipItem *fxclip = dynamic_cast<FxClipItem*>(fx);
	if (!fxclip) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("FX Id %1 does not represent a FxClip in start command\n"
									 "It is fx with name: %2").arg(id).arg(fx->name()));
		return false;
	}

	return startFxClip(fxclip);
}


/**
 * @brief VideoControl::startFxClip
 * @param fxc
 * @return
 *
 * There are two types of video clips now. A normal video, played by QVideoPlayer, on one hand and still pictures, which are
 * overlayed in an extra widget on the other hand.
 * Both are based on functions and the FxItem of an FxAudio item.
 */
bool VideoControl::startFxClip(FxClipItem *fxc)
{
	qDebug() << "VideoControl::Start FxClip"<< fxc->name();

	bool videoRunning = false;
	int slot = myApp.unitAudio->selectFreeVideoSlot(&videoRunning);
	if (slot < 0) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("No free slot available for video playback!"));
		return false;
	}

	VideoPlayer *vp = videoSlot(slot);
	if (!vp)
		return false;

	// check if there is already a video running in this slot.
	// If so, we have to stop the video first.
	if (videoRunning && !fxc->isPicClip) {
		if (vp->isCurrentFxClipAPicClip()) {
			vp->stop();
		}
		else if (!vp->stopAndWait())
			return false;

		// select it
		if (!myApp.unitAudio->selectVideoSlot(slot))
			return false;
	}

	vp->playFxClip(fxc, slot);

	return true;
}

void VideoControl::videoBlack(qint32 time_ms)
{
	if (!myApp.unitAudio->isValid())
		return;

	VideoPlayer *vp = myApp.unitAudio->videoPlayer();
	Q_ASSERT(vp);

	qDebug() << "videoBlack PicOverlayActive" << vp->isPicClipOverlaysActive();

	if (vp->currentFxClipItem())
		LOGTEXT(tr("<font color=green>BLACK video screen</font> <b>%1</b>")
				.arg(vp->currentFxClipItem()->fileName()));

	if (vp->isPicClipOverlaysActive()) {

		if (vp->fadePicClipOverlayOut(time_ms, 0)) {
			if (time_ms > 0) {
				LOGTEXT(tr("<font color=green>Fade to BLACK back overlay</font>: %1ms").arg(time_ms));
			} else {
				LOGTEXT(tr("<font color=green>BLACK back overlay</font>"));
			}
		}
		if (vp->fadePicClipOverlayOut(time_ms, 1)) {
			if (time_ms > 0) {
				LOGTEXT(tr("<font color=green>Fade to BLACK top overlay</font>: %1ms").arg(time_ms));
			} else {
				LOGTEXT(tr("<font color=green>BLACK top overlay</font>"));
			}
		}

		if (vp->isRunning()) {
			LOGTEXT(tr("<font color=orange>BLACK video</font>"));
			vp->stop();
		}
	}
	else {
		if (time_ms < 1) {
			vp->stop();
			vp->setPicClipOverlaysDisabled();		// ??? Should be disabled at this time ???
		}
		else {
			vp->fadeVideoToBlack(time_ms);
		}
	}

	myApp.unitAudio->videoWidget()->update();

}

/**
 * @brief VideoControl::setVideoVolume
 * @param slotnum
 * @param vol
 * @return
 *
 * @note there is only one video player for now, therefor slotnum is not used
 */
bool VideoControl::setVideoVolume(int slotnum, int vol)
{
	Q_UNUSED(slotnum)

	VideoPlayer *vp = myApp.unitAudio->videoPlayer();
	if (vp) {
		vp->setVolume(vol);
		return true;
	}

	return false;
}

bool VideoControl::setVideoMasterVolume(int vol)
{
	VideoPlayer *vp = myApp.unitAudio->videoPlayer();
	if (vp) {
		vp->setMasterVolume(vol);
		return true;
	}

	return false;
}

/**
 * @brief Fadeout audio for a video clip
 * @param slotnum
 * @param targetVolume
 * @param time_ms
 * @return
 *
 * @note this function calls fadeout functions from AudioSlot.
 */
bool VideoControl::fadeOutFxClipAudio(int slotnum, int targetVolume, int time_ms)
{
	Q_UNUSED(slotnum)

	VideoPlayer *vp = videoSlot(slotnum);
	AudioSlot *as = myApp.unitAudio->audioSlot(slotnum);
	if (!vp || !as)
		return false;

	return as->fadeoutFxClip(targetVolume, time_ms);
}

bool VideoControl::startFxClipItemInSlot(FxClipItem *fxc, int slotnum, Executer *exec, qint64 atMs)
{
	if (!FxItem::exists(fxc))
		return false;

	emit videoStartedWithAudio(fxc, slotnum, exec, atMs, fxc->initialVolume);
	return myApp.unitAudio->startFxClipItemInSlot(fxc, slotnum, exec, atMs, fxc->initialVolume);
}

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
#include "videoplayer.h"
#include "customwidget/psvideowidget.h"

VideoControl::VideoControl(AppCentral &app_central)
	: QObject()
	, myApp(app_central)
{
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

bool VideoControl::startFxClip(FxClipItem *fxc)
{
	qDebug() << "Start FxAudio as FxClip"<< fxc->name();

	return myApp.unitAudio->startFxClip(fxc);
}

void VideoControl::videoBlack(qint32 time_ms)
{
	Q_UNUSED(time_ms)

	if (!myApp.unitAudio->isValid()) return;

	VideoPlayer *vp = myApp.unitAudio->videoPlayer();
	vp->stop();

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

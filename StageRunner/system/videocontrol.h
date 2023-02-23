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

#ifndef VIDEOCONTROL_H
#define VIDEOCONTROL_H

#include <QObject>

#include "commandsystem.h"

class AppCentral;
class FxClipItem;
class QMediaPlayer;
class QMediaPlaylist;
class PsVideoWidget;
class VideoPlayer;

using namespace AUDIO;

class VideoControl : public QObject
{
	Q_OBJECT
private:
	AppCentral &myApp;

public:
	VideoControl(AppCentral &app_central);
	int masterVolume() const;
	int maxVolume() const;

	VideoPlayer * videoSlot(int slotNum);

	bool startFxClipById(qint32 id);
	bool startFxClip(FxClipItem *fxc);

	void videoBlack(qint32 time_ms);
	bool setVideoVolume(int slotnum, int vol);
	bool setVideoMasterVolume(int vol);
	bool fadeOutFxClipAudio(int slotnum, int targetVolume, int time_ms);
	bool startFxClipItemInSlot(FxClipItem *fxc, int slotnum, Executer *exec = nullptr, qint64 atMs = -1);

signals:
	void videoCtrlMsgEmitted(const AUDIO::AudioCtrlMsg &msg);
	void videoStartedWithAudio(FxClipItem *fxc, int slotNum, Executer *exec, qint64 atMs, int initialVol);
	void mainVideoViewStateChanged(int viewState, int oldViewState);

public slots:

};

#endif // VIDEOCONTROL_H

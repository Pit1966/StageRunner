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

#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include "../tool/varset.h"
#include "../config.h"

class UserSettings : public VarSet
{
public:
	pstring pApplicationGuiStyle;
	pstring pDialKnobStyle;
	pstring pLastAudioFxImportPath;
	pstring pLastVideoFxImportPath;
	pstring pLastProjectSavePath;
	pstring pLastProjectLoadPath;
	pstring pLastAudioTrackImportPath;
	pstring pLastCondolidatePath;
	pint32 pAudioBufferSize;
	pint32 pDefaultAudioFadeoutTimeMs;
	pbool pProhibitAudioDoubleStart;
	pbool pLogarithmicVolDials;
	pint32 pAudioAllowReactivateTime;
	pbool pNoInterfaceDmxFeedback;
	qint32 mapAudioToDmxUniv[MAX_AUDIO_SLOTS];
	qint32 mapAudioToDmxChan[MAX_AUDIO_SLOTS];
	pstring pSlotAudioDevice[MAX_AUDIO_SLOTS];
	pint32 pAudioPlayListChannel;
	pbool pAltAudioEngine;
	pbool pUseSDLAudio;
	pint32 pVolumeDialMask;
	pint32 pFFTAudioMask;							///< The bits of this mask corresponds to the FFT enabled status of the channels
	pstring pFxTemplatePath;						///< This is the default Template File
	pbool pIsDarkGuiTheme;							///< Should be set to true, if GUI Style is dark
	pbool pShowVirtualDMXMonitor;
	pbool pVideoForceSecondDesktop;					///< open video output on second desktop/monitor if there is one
	pint32 pStartCount;								///< counter incremented every start
	pint32 pVideoOutXSize;
	pint32 pVideoOutYSize;

public:
	UserSettings();
	~UserSettings();

private:
	void init();
};

#endif // USERSETTINGS_H

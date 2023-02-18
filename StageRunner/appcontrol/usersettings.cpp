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

#include "usersettings.h"
#include "log.h"
#include "config.h"
#include "configrev.h"

#include <QDir>

UserSettings::UserSettings()
	: VarSet()
{
	init();

	readFromPref();

	pStartCount = pStartCount + 1;


#ifdef __APPLE__
	pAltAudioEngine = true;
#endif
}

UserSettings::~UserSettings()
{
	writeToPref();
}

void UserSettings::init()
{
	QString templatePath = QString("%1/.config/%2/%3.templates")
			.arg(QDir::homePath(), QStringLiteral(APP_CONFIG_PATH), QStringLiteral(APPNAME));


	setClass(PrefVarCore::USER_CONFIG,"UserSettings");
	registerQSetGlobal("User and GUI settings for StageRunner Application");

	addExistingVar(pApplicationGuiStyle,"ApplicationGuiStyle");
	addExistingVar(pDialKnobStyle,"DialKnobStyle");
	addExistingVar(pLastAudioFxImportPath,"LastAudioFxImportPath");
	addExistingVar(pLastVideoFxImportPath,"LastVideoFxImportPath");
	addExistingVar(pLastAudioTrackImportPath,"LastAudioTrackImportPath");
	addExistingVar(pLastProjectLoadPath,"LastProjectLoadPath");
	addExistingVar(pLastProjectSavePath,"LastProjectSavePath");
	addExistingVar(pLastCondolidatePath,"LastConsolidatePath");
	addExistingVar(pAudioBufferSize,"AudioBufferSize",100,100000,18000);
//				   ,QObject::tr("Audio buffer size for audio playback in the audio slots"));
	addExistingVar(pDefaultAudioFadeoutTimeMs,"DefaultAudioFadeoutTimeMs",0,10000,5001);
	addExistingVar(pProhibitAudioDoubleStart,"ProhibitAudioDoubleStart",true);
	addExistingVar(pAudioAllowReactivateTime,"AudioAllowReactivationTime",0,10000,800);
	addExistingVar(debug,"DebugLevel");
	addExistingVar(pNoInterfaceDmxFeedback,"EnableNoInterfaceDmxFeedback");

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		addExistingVar(mapAudioToDmxUniv[t],QString("MapAudioToDmxUniv%1").arg(t+1));
		addExistingVar(mapAudioToDmxChan[t],QString("MapAudioToDmxChan%1").arg(t+1));
		addExistingVar(pSlotAudioDevice[t],QString("SlotAudioDevice%1").arg(t+1));
	}
	addExistingVar(pAudioPlayListChannel,"AudioPlayListChannel",0,3,3);
	addExistingVar(pAltAudioEngine,"AlternativeAudioEngine");
	addExistingVar(pUseSDLAudio,"UseSDLAudio");
	addExistingVar(pFFTAudioMask,"FFTAudioMask",0,15,7);
	addExistingVar(pFxTemplatePath,"FxTemplatePath",templatePath);
	addExistingVar(pVolumeDialMask,"ShowVolumeDialMask",0,15,8);
	addExistingVar(pIsDarkGuiTheme,"IsDarkGuiTheme",true);
	addExistingVar(pShowVirtualDMXMonitor,"ShowVirtualDMXMonitor",false);
	addExistingVar(pStartCount,"AppStartCount");
}

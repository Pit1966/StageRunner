#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include "../tool/varset.h"
#include "config.h"

class UserSettings : public VarSet
{
public:
	pstring pApplicationGuiStyle;
	pstring pDialKnobStyle;
	pstring pLastAudioFxImportPath;
	pstring pLastProjectSavePath;
	pstring pLastProjectLoadPath;
	pstring pLastAudioTrackImportPath;
	pint32 pAudioBufferSize;
	pbool pProhibitAudioDoubleStart;
	pint32 pAudioAllowReactivateTime;
	pbool pNoInterfaceDmxFeedback;
	qint32 mapAudioToDmxUniv[MAX_AUDIO_SLOTS];
	qint32 mapAudioToDmxChan[MAX_AUDIO_SLOTS];
	pint32 pAudioPlayListChannel;
	pbool pAltAudioEngine;


public:
	UserSettings();
	~UserSettings();

private:
	void init();
};

#endif // USERSETTINGS_H

#include "usersettings.h"
#include "log.h"

#include <QDir>

UserSettings::UserSettings()
	: VarSet()
{
	init();

	readFromPref();

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
			.arg(QDir::homePath()).arg(APP_ORG_STRING).arg(APP_NAME);


	setClass(PrefVarCore::USER_CONFIG,"UserSettings");
	registerQSetGlobal("User and GUI settings for StageRunner Application");

	addExistingVar(pApplicationGuiStyle,"ApplicationGuiStyle");
	addExistingVar(pDialKnobStyle,"DialKnobStyle");
	addExistingVar(pLastAudioFxImportPath,"LastAudioFxImportPath");
	addExistingVar(pLastVideoFxImportPath,"LastVideoFxImportPath");
	addExistingVar(pLastAudioTrackImportPath,"LastAudioTrackImportPath");
	addExistingVar(pLastProjectLoadPath,"LastProjectLoadPath");
	addExistingVar(pLastProjectSavePath,"LastProjectSavePath");
	addExistingVar(pAudioBufferSize,"AudioBufferSize",100,100000,5000);
//				   ,QObject::tr("Audio buffer size for audio playback in the audio slots"));
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
	addExistingVar(pFFTAudioMask,"FFTAudioMask",0,15,15);
	addExistingVar(pFxTemplatePath,"FxTemplatePath",templatePath);
	addExistingVar(pVolumeDialMask,"ShowVolumeDialMask",0,15,15);


}

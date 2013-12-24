#include "usersettings.h"
#include "log.h"

UserSettings::UserSettings()
	: VarSet()
{
	init();

	readFromPref();
}

UserSettings::~UserSettings()
{
	writeToPref();
}

void UserSettings::init()
{
	setClass(PrefVarCore::USER_CONFIG,"UserSettings");
	registerQSetGlobal("User and GUI settings for StageRunner Application");

	addExistingVar(pApplicationGuiStyle,"ApplicationGuiStyle");
	addExistingVar(pDialKnobStyle,"DialKnobStyle");
	addExistingVar(pLastAudioFxImportPath,"LastAudioFxImportPath");
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
	}
}

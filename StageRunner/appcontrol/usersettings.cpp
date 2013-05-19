#include "usersettings.h"

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

	addExistingVar(pLastAudioFxImportPath,"LastAudioFxImportPath");
	addExistingVar(pLastProjectLoadPath,"LastProjectLoadPath");
	addExistingVar(pLastProjectSavePath,"LastProjectSavePath");
	addExistingVar(pAudioBufferSize,"AudioBufferSize",100,100000,5000);
//				   ,QObject::tr("Audio buffer size for audio playback in the audio slots"));
	addExistingVar(debug,"DebugLevel");
}

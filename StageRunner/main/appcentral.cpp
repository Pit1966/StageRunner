#include "appcentral.h"
#include "system/commandsystem.h"
#include "system/audiocontrol.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"
#include "fx/fxaudioitem.h"
#include "project.h"
#include "usersettings.h"

using namespace AUDIO;

AppCentral *AppCentral::myinstance = 0;

AppCentral *AppCentral::instance()
{
	if (!myinstance) {
		myinstance = new AppCentral;
	}

	return myinstance;
}

bool AppCentral::destroyInstance()
{
	if (myinstance) {
		delete myinstance;
		myinstance = 0;
		return true;
	} else {
		return false;
	}
}

void AppCentral::clearProject()
{
	project->clear();
}

void AppCentral::stopAllFxAudio()
{
	unitAudio->stopAllFxAudio();
}

void AppCentral::executeFxCmd(FxItem *fx, int cmd)
{
	switch (fx->fxType()) {
	case FX_AUDIO:
		unitAudio->startFxAudio(reinterpret_cast<FxAudioItem*>(fx));
		break;
	default:
		break;
	}

}

AppCentral::AppCentral()
{
	init();
}

AppCentral::~AppCentral()
{
	delete userSettings;
	delete project;
	delete unitAudio;
}

void AppCentral::init()
{
	userSettings = new UserSettings;

	unitAudio = new AudioControl;
	project = new Project;

	qRegisterMetaType<AudioCtrlMsg>("AudioCtrlMsg");
}

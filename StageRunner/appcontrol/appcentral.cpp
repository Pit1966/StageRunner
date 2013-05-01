#include "config.h"
#include "appcentral.h"
#include "system/audiocontrol.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"
#include "fx/fxaudioitem.h"
#include "project.h"
#include "usersettings.h"
#include "fx/fxlist.h"
#include "ioplugincentral.h"
#include "plugins/interfaces/qlcioplugin.h"

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

void AppCentral::fadeoutAllFxAudio()
{
	unitAudio->fadeoutAllFxAudio();
}

int AppCentral::registerFxList(FxList *fxlist)
{
	if (fxlist->regId() > 0) {
		DEBUGERROR("FxList is already registered with Id: %d",fxlist->regId());
		return 0;
	}

	int id = 0;
	for (int t=0; t<registered_fx_lists.size(); t++) {
		FxList *fxl = registered_fx_lists.at(t);
		if (fxl->regId() > id) id = fxl->regId();
	}
	id++;

	fxlist->setRegId(id);
	registered_fx_lists.append(fxlist);
	return id;
}

FxList *AppCentral::getRegisteredFxList(int id)
{
	for (int t=0; t<registered_fx_lists.size(); t++) {
		if (id == registered_fx_lists.at(t)->regId()) {
			return registered_fx_lists.at(t);
		}
	}
	DEBUGERROR("FxList with Id: %d is not registered",id);

	return 0;
}

void AppCentral::loadPlugins()
{
#ifdef WIN32
	pluginCentral->loadQLCPlugins("../plugins");
#else
	pluginCentral->loadQLCPlugins(IOPluginCentral::sysPluginDir());
#endif
}

void AppCentral::openPlugins()
{
	pluginCentral->openPlugins();
}

void AppCentral::closePlugins()
{
	pluginCentral->closePlugins();
}


void AppCentral::executeFxCmd(FxItem *fx, CtrlCmd cmd)
{
	if (!FxItem::exists(fx)) {
		DEBUGERROR("Execute FX: FxItem not found in FX list");
		return;
	}

	switch (fx->fxType()) {
	case FX_AUDIO:
		switch (cmd) {
		case CMD_AUDIO_START:
			unitAudio->startFxAudio(reinterpret_cast<FxAudioItem*>(fx));
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d",cmd);
			break;
		}
		break;
	default:
		break;
	}
}

void AppCentral::executeNextFx(int listID)
{
	if (debug) DEBUGTEXT("Execute next in Sequence for list Id: %d",listID);
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;

	FxItem *fx = fxlist->stepToSequenceNext();
	if (!fx) return;

	executeFxCmd(fx,CMD_FX_START);
}

void AppCentral::testSetDmxChannel(int val, int channel)
{
	bool changed = false;
	val = val * 255 / MAX_DMX_FADER_RANGE;

	if (dmx_direct_data.at(channel) != val) {
		dmx_direct_data[channel] = val;
		changed = true;
	}

	if (changed) {
		QList<QLCIOPlugin*>plugins = pluginCentral->qlcPlugins();
		for (int t=0; t<plugins.size(); t++) {
			QLCIOPlugin *plugin = plugins.at(t);
			plugin->writeUniverse(0,dmx_direct_data);

		}
	}
}

AppCentral::AppCentral()
{
	init();
}

AppCentral::~AppCentral()
{
	delete pluginCentral;
	delete userSettings;
	delete project;
	delete unitAudio;
}

void AppCentral::init()
{
	edit_mode_f = false;
	dmx_direct_data.resize(512);
	memset(dmx_direct_data.data(),0,512);

	userSettings = new UserSettings;

	unitAudio = new AudioControl;
	project = new Project;
	pluginCentral = new IOPluginCentral;

	int id = registerFxList(project->fxList);
	qDebug("Registered Project FX list with Id:%d",id);

	qRegisterMetaType<AudioCtrlMsg>("AudioCtrlMsg");

}

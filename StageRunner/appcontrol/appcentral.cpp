#include "config.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "project.h"
#include "usersettings.h"
#include "fxlist.h"
#include "ioplugincentral.h"
#include "qlcioplugin.h"
#include "lightcontrol.h"
#include "pluginmapping.h"

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

bool AppCentral::setLightLoopEnabled(bool state)
{
	return unitLight->setLightLoopEnabled(state);
}

void AppCentral::stopAllFxAudio()
{
	unitAudio->stopAllFxAudio();
}

void AppCentral::fadeoutAllFxAudio()
{
	unitAudio->fadeoutAllFxAudio();
}

void AppCentral::lightBlack(qint32 time_ms)
{
	int num = unitLight->black(time_ms);
	LOGTEXT(tr("BLACK: Fade %1 scenes to level 0 in %2s")
			.arg(num).arg(float(time_ms)/1000));
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

void AppCentral::setEditMode(bool state)
{
	if (state != edit_mode_f) {
		edit_mode_f = state;
		emit editModeChanged(state);
	}
}

void AppCentral::loadPlugins()
{
	// Load Plugin Configuration and DMX Mapping
	pluginCentral->pluginMapping->loadFromDefaultFile();
	// Load the Plugins
	pluginCentral->loadQLCPlugins(IOPluginCentral::sysPluginDir());

	pluginCentral->updatePluginMappingInformation();
}

void AppCentral::openPlugins()
{
	pluginCentral->openPlugins();
}

void AppCentral::closePlugins()
{
	pluginCentral->closePlugins();
}

DmxMonitor * AppCentral::openDmxInMonitor(int universe)
{
	QLCIOPlugin *plugin;
	int input;
	if ( pluginCentral->getPluginAndInputForDmxUniverse(universe,plugin,input) ) {
		if (plugin->capabilities() & QLCIOPlugin::Monitor) {
			return plugin->openInputMonitor(input);
		}
	}
	return 0;
}

DmxMonitor * AppCentral::openDmxOutMonitor(int universe)
{
	QLCIOPlugin *plugin;
	int output;
	if ( pluginCentral->getPluginAndOutputForDmxUniverse(universe,plugin,output) ) {
		if (plugin->capabilities() & QLCIOPlugin::Monitor) {
			return plugin->openOutputMonitor(output);
		}
	}
	return 0;
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
		case CMD_AUDIO_STOP:
			unitAudio->stopFxAudio(reinterpret_cast<FxAudioItem*>(fx));
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d for audio",cmd);
		}
		break;
	case FX_SCENE:
		switch (cmd) {
		case CMD_SCENE_START:
			unitLight->startFxSceneSimple(reinterpret_cast<FxSceneItem*>(fx));
			break;
		case CMD_SCENE_STOP:
			unitLight->stopFxScene(reinterpret_cast<FxSceneItem*>(fx));
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d for scene",cmd);
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


	FxItem *cur_fx = fxlist->currentFx();
	FxItem *next_fx = fxlist->stepToSequenceNext();

	if (cur_fx && cur_fx != next_fx) {
		executeFxCmd(cur_fx,CMD_FX_STOP);
	}
	if (next_fx) {
		executeFxCmd(next_fx,CMD_FX_START);
	}
}

void AppCentral::moveToFollowerFx(int listID)
{
	if (debug) DEBUGTEXT("Move to follower FX in Sequence for list Id: %d",listID);
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;

	fxlist->setNextFx(fxlist->findSequenceFollower());
}

void AppCentral::moveToForeRunnerFx(int listID)
{
	if (debug) DEBUGTEXT("Move to previous FX in Sequence for list Id: %d",listID);
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;

	fxlist->setNextFx(fxlist->findSequenceForerunner());
}

void AppCentral::testSetDmxChannel(int val, int channel)
{
	val = val * 255 / MAX_DMX_FADER_RANGE;

	if (unitLight->dmxOutputValues[0].at(channel) != val) {
		unitLight->dmxOutputValues[0][channel] = val;
		unitLight->dmxOutputChanged[0] = true;
	}
}

/**
 * @brief Process Signal from input channel
 * @param universe The universe number
 * @param channel The channel in the universe that has changed
 * @param value The actual value of the changed channel
 *
 * This slot usualy is called when a DMX interface raises a signal
 */
void AppCentral::onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value)
{
	/// @implement me: Here we have to provide the selection for different receivers for the signal.
 /// e.g. an Audio Volume slot
 /// For now we forward the signal to the light unit
	unitLight->onInputUniverseChannelChanged(universe,channel,value);

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
	delete unitLight;
	delete unitAudio;
}

void AppCentral::init()
{
	edit_mode_f = false;

	userSettings = new UserSettings;

	unitAudio = new AudioControl(this);
	unitLight = new LightControl(this);
	project = new Project;
	pluginCentral = new IOPluginCentral;

	int id = registerFxList(project->fxList);
	qDebug("Registered Project FX list with Id:%d",id);
	unitLight->addFxListToControlLoop(project->fxList);



	qRegisterMetaType<AudioCtrlMsg>("AudioCtrlMsg");

	// Do some connects
	connect(pluginCentral,SIGNAL(universeValueChanged(quint32,quint32,uchar)),this,SLOT(onInputUniverseChannelChanged(quint32,quint32,uchar)));

	// AppCentral -> AudioControl Thread (unitAudio)
	connect(this,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));


}

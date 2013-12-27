#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "fxcontrol.h"
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
#include "messagehub.h"
#include "execcenter.h"
#include "executer.h"

#include <QFileDialog>

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

bool AppCentral::setFxExecLoopEnabled(bool state)
{
	return unitFx->setExecLoopEnabled(state);
}

/**
 * @brief Stop all Audio FXs and set running FxAudioPlaylists into pause mode
 */
void AppCentral::stopAllFxAudio()
{
	unitFx->pauseAllFxPlaylist();
	unitAudio->stopAllFxAudio();
}

/**
 * @brief Fadeout all Audio FXs and set running FxAudioPlaylists into pause mode
 *
 * The fadeouttime is the default time that can be configured in setup
 */
void AppCentral::fadeoutAllFxAudio()
{
	unitFx->pauseAllFxPlaylist();
	unitAudio->fadeoutAllFxAudio();
}

/**
 * @brief Stop all running audio playlists and all running sequences
 */
void AppCentral::stopAllSequencesAndPlaylists()
{
	unitFx->stopAllFxPlaylists();
	unitFx->stopAllFxSequences();
}


void AppCentral::lightBlack(qint32 time_ms)
{
	int num = unitLight->black(time_ms);
	LOGTEXT(tr("BLACK: Fade %1 scenes to level 0 in %2s")
			.arg(num).arg(float(time_ms)/1000));
}

/**
 * @brief Stop all or dedicated sequence
 * @param fxseq Pointer to FxSequence item or 0, if all Sequences should be stopped
 */
void AppCentral::sequenceStop(FxItem *fxseq)
{
	if (fxseq) {
		if (fxseq->fxType() == FX_SEQUENCE)
			unitFx->stopFxSequence(reinterpret_cast<FxSeqItem*>(fxseq));
	} else {
		unitFx->stopAllFxSequences();
	}
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

void AppCentral::setExperimentalAudio(bool state)
{
	if (state != experimental_audio_f) {
		experimental_audio_f = state;
	}
}

void AppCentral::setInputAssignMode(bool state)
{
	if (state != input_assign_mode_f) {
		input_assign_mode_f = state;
		emit inputAssignModeChanged(state);
	}
}

void AppCentral::setInputAssignMode(FxItem *fx)
{
	setInputAssignMode(true);
	input_assign_target_fxitem = fx;
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

void AppCentral::reOpenPlugins()
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

void AppCentral::assignInputToSelectedFxItem(qint32 universe, qint32 channel, int value)
{
	Q_UNUSED(value);

	static qint32 last_universe = -1;
	static qint32 last_channel = -1;

	if (universe == last_universe && channel == last_channel) return;
	last_universe = universe;
	last_channel = channel;

	bool is_assigned = false;

	FxItem *assign_target;
	if (input_assign_target_fxitem) {
		assign_target = input_assign_target_fxitem;
	} else {
		assign_target = current_selected_project_fx;
	}


	FxItem *fx;
	foreach (fx, FxItem::globalFxList()) {
		if (fx->isHookedToInput(universe, channel)) {
			is_assigned = true;
			break;
		}
	}

	if (is_assigned) {
		QString maintext = tr("Input is already assigned to scene");
		QString sub = tr("Universe %1, Channel %2 is hooked to scene '%3'")
				.arg(universe+1).arg(channel+1).arg(fx->name());

		Ps::Button but = MessageHub::question(maintext,sub);
		if (but & Ps::CONTINUE) {
			is_assigned = false;
			fx->hookToInput(-1,-1);
		} else {
			last_channel = -1;
			last_universe = -1;
		}
	}

	if (!is_assigned) {
		if (FxItem::exists(assign_target)) {
			assign_target->hookToInput(universe,channel);
			assign_target->setModified(true);
			LOGTEXT(tr("Universe %1, Channel %2 now hooked to scene '%3'")
					.arg(universe+1).arg(channel+1).arg(assign_target->name()));
			emit inputAssigned(assign_target);
		} else {
			emit inputAssigned(0);
		}

		if (input_assign_target_fxitem) {
			setInputAssignMode(false);
		}
	}
}

bool AppCentral::addFxAudioDialog(FxList *fxlist, QWidget *widget, int row)
{
	if (!fxlist) return false;

	QString path = QFileDialog::getOpenFileName(widget,tr("Choose Audio File")
												,userSettings->pLastAudioFxImportPath);

	if (path.size()) {
		userSettings->pLastAudioFxImportPath = path;
		fxlist->addFxAudioSimple(path,row);
		return true;
	}
	return false;
}


void AppCentral::executeFxCmd(FxItem *fx, CtrlCmd cmd, Executer * exec)
{
	if (!FxItem::exists(fx)) {
		DEBUGERROR("Execute FX: FxItem not found in FX list");
		return;
	}

	switch (fx->fxType()) {
	case FX_AUDIO:
		switch (cmd) {
		case CMD_AUDIO_START:
			unitAudio->startFxAudio(reinterpret_cast<FxAudioItem*>(fx), exec);
			break;
		case CMD_AUDIO_START_AT:
			unitAudio->startFxAudioAt(reinterpret_cast<FxAudioItem*>(fx), exec);
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

	case FX_AUDIO_PLAYLIST:
		switch (cmd) {
		case CMD_AUDIO_START:
			unitFx->startFxAudioPlayList(reinterpret_cast<FxPlayListItem*>(fx));
			break;
		case CMD_AUDIO_START_AT:
			DEBUGERROR("Execute FX: FX_AUDIO_PLAYLIST START AT not implemented: %d for audio",cmd);
			unitFx->startFxAudioPlayList(reinterpret_cast<FxPlayListItem*>(fx));
			break;
		case CMD_AUDIO_STOP:
			// unitAudio->stopFxAudioPlayList(reinterpret_cast<FxPlayListItem*>(fx));
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d for audio",cmd);
		}
		break;

	case FX_SEQUENCE:
		switch (cmd) {
		case CMD_FX_START:
			unitFx->startFxSequence(reinterpret_cast<FxSeqItem*>(fx));
			break;
		case CMD_FX_STOP:
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d for sequence",cmd);
		}

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
		executeFxCmd(cur_fx,CMD_FX_STOP,0);
	}
	if (next_fx) {
		executeFxCmd(next_fx,CMD_FX_START,0);
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

void AppCentral::storeSelectedFxListWidgetFx(FxItem *item)
{
	current_selected_project_fx = item;
}


AppCentral::AppCentral()
{
	init();
}

AppCentral::~AppCentral()
{
	delete unitFx;
	delete execCenter;
	delete unitLight;
	delete unitAudio;
	delete pluginCentral;
	delete project;
	delete userSettings;

	MessageHub::destroy();
}

void AppCentral::init()
{
	edit_mode_f = false;
	experimental_audio_f = false;
	input_assign_mode_f = false;
	input_assign_target_fxitem = 0;

	current_selected_project_fx = 0;

	mainWinObj = 0;
	userSettings = new UserSettings;
	project = new Project;
	pluginCentral = new IOPluginCentral;
	unitAudio = new AudioControl(*this);
	unitLight = new LightControl(*this);
	execCenter = new ExecCenter(*this);
	unitFx = new FxControl(*this, *execCenter);

	int id = registerFxList(project->mainFxList());
	if (debug > 1) DEBUGTEXT("Registered Project FX list with Id:%d",id);
	unitLight->addFxListToControlLoop(project->mainFxList());



	qRegisterMetaType<AudioCtrlMsg>("AudioCtrlMsg");
	qRegisterMetaType<CtrlCmd>("CtrlCmd");

	// Load Message Rules
	MessageHub::instance();

	// Do some connects
	connect(pluginCentral,SIGNAL(universeValueChanged(quint32,quint32,uchar)),this,SLOT(onInputUniverseChannelChanged(quint32,quint32,uchar)));

	// AppCentral -> AudioControl Thread (unitAudio)
	connect(this,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));

}

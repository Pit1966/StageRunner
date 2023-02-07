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

#include "log.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "fxcontrol.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxclipitem.h"
#include "fxscriptitem.h"
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
#include "fxlistvarset.h"
#include "fxlistwidget.h"
#include "videocontrol.h"
#include "dmxuniverseproperty.h"
#include "../plugins/yadi/src/dmxmonitor.h"
#include "netserver.h"

#include <QFileDialog>

using namespace AUDIO;

AppCentral *AppCentral::myinstance = nullptr;

AppCentral *AppCentral::instance()
{
	if (!myinstance) {
		myinstance = new AppCentral;
	}

	return myinstance;
}

AppCentral &AppCentral::ref()
{
	if (!myinstance) {
		myinstance = new AppCentral;
	}
	return *myinstance;
}

bool AppCentral::destroyInstance()
{
	if (myinstance) {
		delete myinstance;
		myinstance = nullptr;
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
void AppCentral::fadeoutAllFxAudio(int fadeoutTimeMs)
{
	unitFx->pauseAllFxPlaylist();
	unitAudio->fadeoutAllFxAudio(fadeoutTimeMs);
}

/**
 * @brief Stop all running audio playlists and all running sequences
 */
void AppCentral::stopAllSequencesAndPlaylists()
{
	unitFx->stopAllFxPlaylists();
	unitFx->stopAllFxSequences();
	unitFx->stopAllFxScripts();
}


void AppCentral::lightBlack(qint32 time_ms)
{
	int num = unitLight->black(time_ms);
	LOGTEXT(tr("BLACK: Fade %1 scenes to level 0 in %2s")
			.arg(num).arg(double(time_ms)/1000));
}

void AppCentral::videoBlack(qint32 time_ms)
{
	if (unitVideo)
		unitVideo->videoBlack(time_ms);
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

	return nullptr;
}

bool AppCentral::isExperimentalAudio() const
{
	return userSettings->pAltAudioEngine;
}

void AppCentral::setEditMode(bool state)
{
	if (state != edit_mode_f) {
		edit_mode_f = state;
		emit editModeChanged(state);
	}
}

void AppCentral::setAudioOutputType(AudioOutputType type)
{
	userSettings->pAltAudioEngine = (type == OUT_MEDIAPLAYER || type == OUT_NONE);
	userSettings->pUseSDLAudio = (type == OUT_SDL2);
}

void AppCentral::setFFTAudioChannelMask(qint32 mask)
{
	if (mask != userSettings->pFFTAudioMask) {
		userSettings->pFFTAudioMask = mask;
		unitAudio->setFFTAudioChannelFromMask(mask);
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

bool AppCentral::startTcpServer()
{
	return tcpServer->startTCPServer();
}

void AppCentral::loadPlugins()
{
	// Load Plugin Configuration and DMX Mapping
	pluginCentral->pluginMapping->loadFromDefaultFile();
	// Load the Plugins

	QString appdir = QApplication::applicationDirPath();

#ifdef IS_MAC
	QString loadfrom = QString("%1/../../../../plugins").arg(appdir);
	int loadcnt = pluginCentral->loadQLCPlugins(loadfrom);
	if (loadcnt == 0) {
		loadfrom = QString("%1/../PlugIns/stagerunner").arg(appdir);
		loadcnt += pluginCentral->loadQLCPlugins(loadfrom);
	}
#else
	QString loadfrom = QString("%1/../plugins").arg(appdir);
	int loadcnt = pluginCentral->loadQLCPlugins(loadfrom);
#endif
	if (loadcnt < 2) {
		loadcnt += pluginCentral->loadQLCPlugins(IOPluginCentral::sysPluginDir());
	}

	qDebug() << "load plugins" << IOPluginCentral::sysPluginDir();
	pluginCentral->updatePluginMappingInformation();
}


void AppCentral::openPlugins()
{
	pluginCentral->openPlugins();
}

void AppCentral::reOpenPlugins()
{
	pluginCentral->reOpenPlugins();
}

void AppCentral::closePlugins()
{
	pluginCentral->closePlugins();
}

DmxMonitor * AppCentral::openDmxInMonitor(int universe)
{
	if (m_openedInputDmxMonitorWidgets[universe]) {
		m_openedInputDmxMonitorWidgets[universe]->close();
		m_openedInputDmxMonitorWidgets[universe] = nullptr;
	}

	QLCIOPlugin *plugin;
	int input;
	if ( pluginCentral->getPluginAndInputForDmxUniverse(universe,plugin,input) ) {
		if (plugin->capabilities() & QLCIOPlugin::Monitor) {
			m_openedInputDmxMonitorWidgets[universe] = plugin->openInputMonitor(uint(input));
			return m_openedInputDmxMonitorWidgets[universe];
		}
	}
	return nullptr;
}

/**
 * @brief AppCentral::openDmxOutMonitor
 * @param universe
 * @return
 *
 * @todo this opens the monitor for the first output only! Multiple outputs per universe is not supported here
 */
DmxMonitor * AppCentral::openDmxOutMonitor(int universe)
{
	if (m_openedOutputDmxMonitorWidgets[universe]) {
		m_openedOutputDmxMonitorWidgets[universe]->close();
		m_openedOutputDmxMonitorWidgets[universe] = nullptr;
	}

	QLCIOPlugin *plugin;
	int output;
	int conNum = 0;
	while ( pluginCentral->getPluginAndOutputForDmxUniverse(universe,conNum,plugin,output) ) {
		if (plugin->capabilities() & QLCIOPlugin::Monitor) {
			m_openedOutputDmxMonitorWidgets[universe] = plugin->openOutputMonitor(uint(output));
			return m_openedOutputDmxMonitorWidgets[universe];
		}
		conNum++;
	}
	return nullptr;
}

int AppCentral::closeAllDmxMonitors()
{
	int cnt = 0;
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (m_openedOutputDmxMonitorWidgets[t]) {
			m_openedOutputDmxMonitorWidgets[t]->close();
			m_openedOutputDmxMonitorWidgets[t] = nullptr;
			cnt++;
		}
		if (m_openedInputDmxMonitorWidgets[t]) {
			m_openedInputDmxMonitorWidgets[t]->close();
			m_openedInputDmxMonitorWidgets[t] = nullptr;
			cnt++;
		}
	}
	return cnt;
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
		assign_target = last_global_selected_fxitem;
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
			emit inputAssigned(nullptr);
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

	/* load the song */
//	if (testsdl=Mix_LoadMUS(path.toLocal8Bit().data())) {
//		Mix_MusicType type=Mix_GetMusicType(testsdl);
//		qDebug("Music type: %s\n",
//			   type==MUS_NONE?"MUS_NONE":
//							  type==MUS_CMD?"MUS_CMD":
//											type==MUS_WAV?"MUS_WAV":
//														  type==MUS_MOD?"MUS_MOD":
//																		type==MUS_MID?"MUS_MID":
//																					  type==MUS_OGG?"MUS_OGG":
//																									type==MUS_MP3?"MUS_MP3":
//																												  type==MUS_MP3_MAD?"MUS_MP3_MAD":
//																																	type==MUS_FLAC?"MUS_FLAC":
//																																				   "Unknown");
//		Mix_PlayMusic(testsdl,1);
//		Mix_VolumeMusic(MIX_MAX_VOLUME);
//	}



	if (path.size()) {
		userSettings->pLastAudioFxImportPath = path;
		fxlist->addFxAudioSimple(path,row);
		return true;
	}
	return false;
}

FxItem *AppCentral::addDefaultSceneToFxList(FxList *fxlist)
{
	if (!fxlist)
		return nullptr;

	FxItem *addfx = nullptr;


	FxList *templates = templateFxList->fxList();
	FxListWidget *listwid = FxListWidget::findFxListWidget(templates);
	if (listwid && listwid->currentSelectedFxItem()) {
		FxItem *fx = listwid->currentSelectedFxItem();
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = reinterpret_cast<FxSceneItem *>(fx);
			FxSceneItem *newscene = new FxSceneItem(*scene);
			fxlist->addFx(newscene);
			addfx = newscene;
		}
	} else {
		fxlist->addFxScene(28,&addfx);
	}

	return addfx;
}

void AppCentral::setModuleError(MODUL_ERROR e, bool state)
{
	if (state) {
		m_moduleErrorMask |= e;
	} else {
		m_moduleErrorMask &= ~e;
	}
}

AppCentral::MODUL_ERROR AppCentral::moduleErrors() const
{
	return MODUL_ERROR(m_moduleErrorMask);
}

bool AppCentral::hasModuleError() const
{
	return m_moduleErrorMask != E_NO_ERROR;
}

QString AppCentral::moduleErrorText(AppCentral::MODUL_ERROR e)
{
	QString estr;
	if (e & E_NO_AUDIO_DECODER) {
		estr += tr("<b>Audio decoder service not available</b><br>"
				   "Application will not play audio fx items.<br><br>"
				   "Check if your gstreamer installation is sufficient<br><br>");
	}
	if (e & E_AUDIO_DEVICE_NOT_FOUND) {
		estr += tr("<b>Audio device not available</b><br>"
				   "One or more configured audio devices could not been found.<br><br>"
				   "This could mean multi audio output is not possible<br>(See log for details)<br><br>");
	}
	if (e & E_AUDIO_MULTI_OUT_FAIL) {
		estr += tr("<b>Multi audio device output not possible!</b><br>"
				   "Seems like there are special audio devices configured for some audio slots.<br>"
				   "This does work only in CLASSIC audio mode, but it seems we are using a different mode.<br><br>");
	}

	return estr;
}

AudioOutputType AppCentral::usedAudioOutputType() const
{
	if (m_isSDLAvailable && userSettings->pUseSDLAudio) {
		return AUDIO::OUT_SDL2;
	}
	else if (userSettings->pAltAudioEngine) {
		return AUDIO::OUT_MEDIAPLAYER;
	}
	else {
		return AUDIO::OUT_DEVICE;
	}
}

void AppCentral::closeVideoWidget()
{
	unitAudio->closeVideoWidget();
}

bool AppCentral::isVideoWidgetVisible() const
{
	return unitAudio->isVideoWidgetVisible();
}


void AppCentral::executeFxCmd(FxItem *fx, CtrlCmd cmd, Executer * exec)
{
	if (!FxItem::exists(fx)) {
		DEBUGERROR("Execute FX: FxItem not found in FX list");
		return;
	}

	switch (fx->fxType()) {
	case FX_AUDIO:
		if (static_cast<FxAudioItem*>(fx)->isFxClip) {
			FxClipItem *fxc = static_cast<FxClipItem*>(fx);
			switch (cmd) {
			case CMD_FX_STOP:
				unitVideo->videoBlack(fx->fadeOutTime());
				break;
			default:
				unitVideo->startFxClip(fxc);
			}
		}
		else {
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);

			switch (cmd) {
			case CMD_AUDIO_START:
				if (fxa->initialSeekPos) {
					unitAudio->startFxAudio(fxa, exec, fxa->initialSeekPos);
				} else {
					unitAudio->startFxAudio(fxa, exec);
				}
				break;
			case CMD_AUDIO_START_AT:
				unitAudio->startFxAudio(fxa, exec);
				break;
			case CMD_AUDIO_STOP:
				unitAudio->stopFxAudio(fxa);
				break;
			default:
				DEBUGERROR("Execute FX: Unimplemented Command: %d for audio",cmd);
			}
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
			unitFx->stopFxPlayList(reinterpret_cast<FxPlayListItem*>(fx));
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
			unitFx->stopFxSequence(reinterpret_cast<FxSeqItem*>(fx));
			break;
		default:
			DEBUGERROR("Execute FX: Unimplemented Command: %d for sequence",cmd);
			break;
		}
		break;

	case FX_SCRIPT:
		switch (cmd) {
		case CMD_FX_START:
			unitFx->startFxScript(static_cast<FxScriptItem*>(fx));
			break;
		case CMD_FX_STOP:
			unitFx->stopFxScript(static_cast<FxScriptItem*>(fx));
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}
}

void AppCentral::executeFxCmd(qint32 id, CtrlCmd cmd, Executer *exec)
{
	FxItem *fx = FxItem::findFxById(id);
	if (!fx) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("FX Id %1 does not exist!").arg(id));
		return;
	}

	executeFxCmd(fx, cmd, exec);
}

void AppCentral::executeNextFx(int listID)
{
	if (debug) DEBUGTEXT("Execute next in Sequence for list Id: %d",listID);
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;


	FxItem *cur_fx = fxlist->currentFx();
	FxItem *next_fx = fxlist->stepToSequenceNext();

	if (next_fx == nullptr && fxlist->size() > 0) {
		fxlist->setNextFx(fxlist->getFirstFx());
		executeNextFx(listID);
		return;
	}

	if (cur_fx && cur_fx != next_fx) {
		if (cur_fx->holdTime() == 0) {
			LOGTEXT(tr("<font color=green>SEQ NEXT: Stop current FX</font> <b>%1</b> since HOLD time is 0").arg(cur_fx->name()));
			executeFxCmd(cur_fx,CMD_FX_STOP,nullptr);
		}
	}
	if (next_fx) {
		LOGTEXT(tr("<font color=green>SEQ NEXT: Start FX</font> <b>%1</b>").arg(next_fx->name()));
		executeFxCmd(next_fx,CMD_FX_START,nullptr);
		if (next_fx->preDelay() == -1) {
			QTimer::singleShot(next_fx->postDelay(),this,SLOT(executeNextFx()));
		}
	}
}

void AppCentral::executePrevFx(int listID)
{
	if (debug) DEBUGTEXT("Execute previous FX in Sequence for list Id: %d",listID);
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;


	FxItem *cur_fx = fxlist->currentFx();
	FxItem *prev_fx = fxlist->stepToSequencePrev();

	if (cur_fx && cur_fx != prev_fx) {
		if (cur_fx->holdTime() == 0) {
			LOGTEXT(tr("<font color=green>SEQ NEXT: Stop current FX</font> <b>%1</b> since HOLD time is 0").arg(cur_fx->name()));
			executeFxCmd(cur_fx,CMD_FX_STOP,nullptr);
		}
	}
	if (prev_fx) {
		LOGTEXT(tr("<font color=green>SEQ NEXT: Start FX</font> <b>%1</b>").arg(prev_fx->name()));
		executeFxCmd(prev_fx,CMD_FX_START,nullptr);
		if (prev_fx->preDelay() == -1) {
			QTimer::singleShot(prev_fx->postDelay(),this,SLOT(executeNextFx()));
		}
	}
}

void AppCentral::clearCurrentFx(int listID)
{
	FxList *fxlist = getRegisteredFxList(listID);
	if (!fxlist) return;
	fxlist->setCurrentFx(nullptr);
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
		unitLight->dmxOutputValues[0][channel] = char(val);
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
	/// some functions like setAudioVol are already implemented in LightControl::onInputUniverseChanged( ... )
	unitLight->onInputUniverseChannelChanged(universe,channel,value);
}

void AppCentral::setGlobalSelectedFx(FxItem *item)
{
	last_global_selected_fxitem = item;
}

void AppCentral::deleteFxSceneItem(FxSceneItem *scene)
{
	bool ok = FxItem::exists(scene);
	if (ok) {
		delete scene;
		emit fxSceneDeleted(scene);
	}
}

void AppCentral::connectToRemote(const QString &host, quint16 port, const QString &cmd)
{
	if (!logThread->isMainThread()) {
		QMetaObject::invokeMethod(this, "connectToRemote", Qt::QueuedConnection,
								  Q_ARG(QString, host),
								  Q_ARG(quint16, port),
								  Q_ARG(QString, cmd));
		return;
	}

	if (m_remoteSocket->state() == QAbstractSocket::ConnectedState && host == m_lastRemote) {
		m_remoteSocket->write(cmd.toLocal8Bit());
		return;
	}

	if (cmd.size())
		m_pendingRemoteCmdList.append(cmd);

	m_lastRemote = host;
	m_remoteSocket->connectToHost(host, port);
}

void AppCentral::onRemoteConnected()
{
}

void AppCentral::onRemoteStateChanged(QAbstractSocket::SocketState state)
{
	LOGTEXT(tr("Remote socket state: %1").arg(state));

	if (state == QAbstractSocket::ConnectedState) {
		m_remoteOk = true;
		while (!m_pendingRemoteCmdList.isEmpty()) {
			QString cmd = m_pendingRemoteCmdList.takeFirst();
			m_remoteSocket->write(cmd.toLocal8Bit());
		}
	}
	else {
		m_remoteOk = false;
	}
}

AppCentral::AppCentral()
{
	init();
}

AppCentral::~AppCentral()
{
#ifdef USE_SDL
	Mix_CloseAudio();
	SDL_Quit();
#endif

	delete templateFxList;
	delete tcpServer;
	delete unitVideo;
	delete unitFx;
	delete execCenter;
	delete unitLight;
	delete unitAudio;
	delete pluginCentral;
	delete universeLayout;
	delete project;
	delete userSettings;

	MessageHub::destroy();
}

void AppCentral::init()
{
	m_mainThread = QThread::currentThread();
	m_mainThread->setObjectName("MainThread");
	edit_mode_f = false;
	input_assign_mode_f = false;
	input_assign_target_fxitem = nullptr;
	m_moduleErrorMask = E_NO_ERROR;
	last_global_selected_fxitem = nullptr;
#ifdef USE_SDL
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) >= 0) {
		/* initialize sdl mixer, open up the audio device */
		if (Mix_OpenAudio(44100,AUDIO_S16SYS,2,1024) >=0) {
			LOGTEXT("SDL init ok");
			m_isSDLAvailable = true;
		} else {
			LOGERROR("SDL init failed");
			m_isSDLAvailable = false;
		}
	}
#else
	m_isSDLAvailable = false;
#endif // ifdef USE_SDL


	mainwinWidget = nullptr;
	mainWinObj = nullptr;
	m_remoteSocket = nullptr;

	userSettings = new UserSettings;
	project = new Project;
	universeLayout = new DmxUniverseProperty;
	pluginCentral = new IOPluginCentral;
	unitVideo = new VideoControl(*this);
	unitAudio = new AudioControl(*this,false);		// do not init in thread.
	unitLight = new LightControl(*this);
	tcpServer = new NetServer(*this);

	execCenter = new ExecCenter(*this);
	unitFx = new FxControl(*this, *execCenter);
	templateFxList = new FxListVarSet;
	templateFxList->fxList()->setProtected(true);

	int id = registerFxList(project->mainFxList());
	if (debug > 1) DEBUGTEXT("Registered Project FX list with Id:%d",id);
	unitLight->addFxListToControlLoop(project->mainFxList());

	m_remoteSocket = new QTcpSocket(this);
	connect(m_remoteSocket, SIGNAL(connected()), this, SLOT(onRemoteConnected()));
	connect(m_remoteSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onRemoteStateChanged(QAbstractSocket::SocketState)));

	qRegisterMetaType<AudioCtrlMsg>("AudioCtrlMsg");
	qRegisterMetaType<CtrlCmd>("CtrlCmd");
	qRegisterMetaType<AudioStatus>("AUDIO::AudioStatus");
	qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
	QThread::currentThread()->setObjectName("MainThread");

	// Load Message Rules
	MessageHub::instance();

	// Do some connects
	connect(pluginCentral,SIGNAL(universeValueChanged(quint32,quint32,uchar)),this,SLOT(onInputUniverseChannelChanged(quint32,quint32,uchar)), Qt::DirectConnection);

	// AppCentral -> AudioControl Thread (unitAudio)
	connect(this,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));

	///todo video	move signal/slots to videoctrl
	connect(unitVideo,SIGNAL(videoCtrlMsgEmitted(AudioCtrlMsg)),unitAudio,SLOT(audioCtrlRepeater(AudioCtrlMsg)));

	// TCP server -> Script Executer
	connect(tcpServer, SIGNAL(remoteCmdReceived(QString)), execCenter, SLOT(executeScriptCmd(QString)));

	m_uptime.start();
}


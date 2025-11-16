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

#include "lightcontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "lightloopthreadinterface.h"
#include "lightloop.h"
#include "fxitem.h"
#include "fx/fxsceneitem.h"
#include "fx/fxseqitem.h"
#include "fx/fxaudioitem.h"
#include "fx/fxlist.h"
#include "appcontrol/fxlistvarset.h"
#include "qlcioplugin.h"
#include "ioplugincentral.h"
#include "usersettings.h"
#include "toolclasses.h"
#include "execcenter.h"
#include "executer.h"
#include "audiocontrol.h"
#include "dmxchannel.h"
#include "qtstatictools.h"


LightControl::LightControl(AppCentral &app_central)
	: QObject()
	,myApp(app_central)
{
	init();
}

LightControl::~LightControl()
{
	delete lightLoopInterface;

	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (hiddenScannerScenes[t]) {
			myApp.deleteFxSceneItem(hiddenScannerScenes[t]);
			hiddenScannerScenes[t] = nullptr;
		}

		for (int i=0; i<MAX_STATIC_SCENES; i++) {
			if (staticScenes[t][i]) {
				myApp.deleteFxSceneItem(staticScenes[t][i]);
				staticScenes[t][i] = nullptr;
			}
		}
	}
}

bool LightControl::setLightLoopEnabled(bool state)
{
	bool ok = true;
	if (state) {
		// update the universe dmx channel layout before start
		populateUniverseLayoutScenes();

		ok = lightLoopInterface->startThread();
		if (ok) {
			connect(lightLoopInterface->getLightLoopInstance(),SIGNAL(sceneStatusChanged(FxSceneItem*,quint32))
				,this,SLOT(onSceneStatusChanged(FxSceneItem*,quint32)));
			connect(lightLoopInterface->getLightLoopInstance(),SIGNAL(sceneFadeProgressChanged(FxSceneItem*,int,int))
				,this,SLOT(onSceneFadeProgressChanged(FxSceneItem*,int,int)));
			connect(lightLoopInterface->getLightLoopInstance(),SIGNAL(sceneCueReady(FxSceneItem*))
					,this,SLOT(onSceneCueReady(FxSceneItem*)));
		}

	} else {
		lightLoopInterface->getLightLoopInstance()->disconnect();
		ok = lightLoopInterface->stopThread();
	}
	return ok;
}

/**
 * @brief Add a FxList to be processed in ControlLoop
 * @param list Pointer to an existing FxList
 * @return true if added or already existing
 */
bool LightControl::addFxListToControlLoop(const FxList *list)
{
	bool ok = true;
	int id = list->regId();
	if (id < 1) {
		DEBUGERROR("%s: We cannot add a list without Id",Q_FUNC_INFO);
		return false;
	}

	lightFxLists.lock();
	bool id_exists = false;
	for (int t=0; t<lightFxLists.size(); t++) {
		if (lightFxLists.at(t)->regId() == id) id_exists = true;
	}

	if (!id_exists) {
		lightFxLists.append(list);
	}

	lightFxLists.unlock();
	return ok;
}


/**
 * @brief This function sends universe output data to the connected plugins and emits signals for both output
 * universe changed or input universe changed
 * @return true, if something has been sent to the outputs (in the meaning of data has changed)
 */
bool LightControl::sendChangedDmxData()
{
	bool sent = false;

	// Send output universe data
	for (int uni=0; uni<MAX_DMX_UNIVERSE; uni++) {
		if (dmxOutputChanged[uni]) {
			QLCIOPlugin *plugin;
			int output;
			bool univsent = false;
			if (myApp.pluginCentral->hasOutputs(uni)) {
				int conNum = 0;
				while (myApp.pluginCentral->getPluginAndOutputForDmxUniverse(uni, conNum, plugin, output)) {
					plugin->writeUniverse(uni,output,dmxOutputValues[uni]);
					sent = true;
					univsent = true;
					conNum++;
				}
			}
			if (univsent || myApp.userSettings->pNoInterfaceDmxFeedback) {
				emit outputUniverseChanged(uni,dmxOutputValues[uni]);
			}
			dmxOutputChanged[uni] = false;
		}
	}

	// Emit input universe data changed signal if something has changed
	for (int uni=0; uni<MAX_DMX_UNIVERSE; uni++) {
		if (dmxInputChanged[uni]) {
			emit inputUniverseChanged(uni,dmxInputValues[uni]);
			dmxInputChanged[uni] = false;
		}
	}

	return sent;
}

/**
 * @brief start/stop a scene
 * @param scene Pointer to FxSceneItem that should will be processed
 * @return
 *
 * If the Scene is idle (BLACK) the default fadein time will be used to activate the scene
 * If the Scene is on stage the default fadeout time will be used to fade the scene out
 *
 */
bool LightControl::startFxSceneSimple(FxSceneItem *scene)
{
	bool active;
	if (scene->preDelay() == -1) {
		SceneExecuter *exec = myApp.execCenter->newSceneExecuter(scene,scene->parentFxItem());
		active = exec->activateProcessing();
	}
	else if (scene->fadeInTime() > 0 && scene->holdTime() > 0 && scene->fadeOutTime() > 0) {
		SceneExecuter *exec = myApp.execCenter->newSceneExecuter(scene,scene->parentFxItem());
		active = exec->activateProcessing();
		LOGTEXT(tr("<font color=green>Start FADE IN -> HOLD -> FADE OUT scene %1</font>, since all values are set!")
				.arg(scene->name()));
	}
	else if (!scene->isOnStageIntern()) {
		if (myApp.currentKeyModifiers() & Qt::SHIFT) {
			// Test Test Test
			active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN);
		} else {
			active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN);
		}
		if (active) {
			setSceneActive(scene);
			LOGTEXT(tr("<font color=green>Start FADE IN scene %1</font> Duration: %2")
					.arg(scene->name(),
						 QtStaticTools::msToTimeString(scene->fadeInTime())));

		}
	}
	else {
		active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT);
		LOGTEXT(tr("<font color=green>Start FADE OUT scene %1</font> Duration: %2")
				.arg(scene->name(),
					 QtStaticTools::msToTimeString(scene->fadeOutTime())));
	}

	return active;
}

bool LightControl::startFxScene(FxSceneItem *scene)
{
	if (scene->isOnStageIntern())
		return false;

	bool active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN);
	if (active) {
		setSceneActive(scene);
	}

	return active;
}

bool LightControl::stopFxScene(FxSceneItem *scene)
{
	return scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT);
}

/**
 * @brief Add Scene FxItem to active scene list
 * @param scene
 * @return true if added, false if it was in list before
 *
 * The function assures that the scene is not already in the list. So the scene will not be
 * added double.
 */
bool LightControl::setSceneActive(FxSceneItem *scene) const
{
	if (activeScenes.lockContains(scene->id()))
		return false;

	activeScenes.lockInsert(scene->id(),scene);
	return true;
}

/**
 * @brief Remove Scene FxItem from active scene list
 * @param scene
 * @return true: if found in list and removed
 *
 * The function assures that the scene is not already in the list. So the scene will not be
 * added double.
 */
bool LightControl::setSceneIdle(FxSceneItem *scene)
{
	if (!activeScenes.lockContains(scene->id())) return false;

	activeScenes.lockRemove(scene->id());
	return true;
}

/**
 * @brief BLACK Command. Fade all scenes to zero level.
 * @param time_ms
 * @return Amount of scenes that were on stage before black
 */
qint32 LightControl::black(qint32 time_ms)
{
	int num = 0;
	foreach (FxItem * fx, FxItem::globalFxList()) {
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
			if (scene->initSceneCommand(MIX_INTERN,CMD_SCENE_BLACK, time_ms)) {
				num++;
			}
			if (scene->initSceneCommand(MIX_EXTERN,CMD_SCENE_BLACK, time_ms)) {
				num++;
			}
			scene->setLive(false);
		}
	}
	return num;
}

qint32 LightControl::blackFxScene(FxSceneItem *scene, qint32 time_ms)
{
	if (!FxItem::exists(scene))
		return 0;

	if (scene->isOnStageIntern())
		if (scene->initSceneCommand(MIX_INTERN,CMD_SCENE_BLACK, time_ms))
			return 1;

	return 0;
}

qint32 LightControl::blackFxSequence(FxSeqItem *seq, qint32 time_ms)
{
	int num = 0;

	for (int t=0; t<seq->seqList->size(); t++) {
		num += blackFxItem(seq->seqList->at(t), time_ms);
	}
	return num;
}

/**
 * @brief Blacks (or fade to Black) all Members of the FxItem
 * @param time_ms
 * @return amount of Scenes that were affected
 */
qint32 LightControl::blackFxItem(FxItem *fx, qint32 time_ms)
{
	int num = 0;
	if (fx->fxType() == FX_SCENE) {
		if (blackFxScene(static_cast<FxSceneItem*>(fx), time_ms))
			num = 1;
	}
	else if (fx->fxType() == FX_SEQUENCE) {
		num = blackFxSequence(static_cast<FxSeqItem*>(fx), time_ms);
	}
	return num;
}

bool LightControl::fillSceneFromInputUniverses(FxSceneItem *scene, int *feedbackActiveChannels)
{
	bool ok = true;
	feedbackActiveChannels = 0;
	for (int i=0; i<scene->tubeCount(); i++) {
		DmxChannel *dmxchan = scene->tube(i);
		quint32 univ = dmxchan->dmxUniverse;
		quint32 chan = dmxchan->dmxChannel;
		if (univ >= MAX_DMX_UNIVERSE || chan > 511) {
			ok = false;
			continue;
		}

		// scale dmx value to DmxChannel (tube) output value
		qint32 dmxVal = uchar(dmxInputValues[univ][chan]);
		qint32 targetVal = dmxchan->targetFullValue * dmxVal / 255;

		dmxchan->targetValue = targetVal;
		if (targetVal > 0 && feedbackActiveChannels)
			*feedbackActiveChannels = *feedbackActiveChannels + 1;
	}

	return ok;
}

int LightControl::populateUniverseLayoutScenes()
{
	FxList *templateList = AppCentral::ref().templateFxList->fxList();

	int universeFoundCount = 0;
	for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
		FxItem *fx = templateList->findFxItemBySubId(i+1);
		if (fx && fx->fxType() == FX_SCENE) {
			universeLayoutScenes[i] = static_cast<FxSceneItem*>(fx);
			universeFoundCount++;
		}
	}
	return universeFoundCount;
}

/**
 * @brief LightControl::setYadiInOutMergeMode
 * @param input
 * @param mode 0: HTP, 1: 2: 3: force app output, 4: force dmx input
 * @return
 */
bool LightControl::setYadiInOutMergeMode(quint32 input, quint32 mode)
{
	QLCIOPlugin *yadiplugin = myApp.pluginCentral->yadiPlugin();
	if (!yadiplugin)
		return false;

	if (input >= quint32(yadiplugin->inputs().size())) {
		LOGERROR(tr("Yadi input #%1 not available for MERGEMODE").arg(input+1));
		return false;
	}

	quint32 universe = 0;

	bool ok = false;
	QMetaObject::invokeMethod(yadiplugin, "setInOutMergeMode", Qt::DirectConnection,
							  Q_RETURN_ARG(bool, ok),
							  Q_ARG(quint32, input),
							  Q_ARG(quint32, universe),
							  Q_ARG(quint32, mode));


	return ok;
}

bool LightControl::setYadiInOutMergeMode(quint32 mode)
{
	QLCIOPlugin *yadiplugin = myApp.pluginCentral->yadiPlugin();
	if (!yadiplugin)
		return false;

	quint32 universe = 0;
	int incnt = yadiplugin->inputs().size();

	for (int i=0; i<incnt; i++) {
		bool ok = false;
		QMetaObject::invokeMethod(yadiplugin, "setInOutMergeMode", Qt::DirectConnection,
								  Q_RETURN_ARG(bool, ok),
								  Q_ARG(quint32, i),
								  Q_ARG(quint32, universe),
								  Q_ARG(quint32, mode));

	}

	return true;
}

FxSceneItem *LightControl::staticScene(int universe, int number)
{
	Q_ASSERT(universe < MAX_DMX_UNIVERSE);
	Q_ASSERT(number < MAX_STATIC_SCENES);

	if (staticScenes[universe][number] == nullptr) {
		staticScenes[universe][number] = new FxSceneItem;
		staticScenes[universe][number]->setTubeCount(512, universe);
		staticScenes[universe][number]->setName(QString("StaticUNI%1NO%2").arg(universe+1).arg(number+1));
	}

	return staticScenes[universe][number];
}

DmxChannel *LightControl::globalDmxChannel(quint32 universe, qint32 dmxChan)
{
	if (universe < MAX_DMX_UNIVERSE && universeLayoutScenes[universe]) {
		return universeLayoutScenes[universe]->tube(dmxChan);
	}

	return nullptr;
}

DmxChannelType LightControl::globalDmxType(quint32 universe, qint32 dmxChan)
{
	if (universe < MAX_DMX_UNIVERSE && universeLayoutScenes[universe]) {
		DmxChannel *tube = universeLayoutScenes[universe]->tube(dmxChan);
		if (tube)
			return DmxChannelType(tube->dmxType);
	}

	return DmxChannelType::DMX_GENERIC;
}

qint32 LightControl::globalDmxScalerNumerator(quint32 universe, qint32 dmxChan)
{
	if (universe < MAX_DMX_UNIVERSE && universeLayoutScenes[universe]) {
		DmxChannel *tube = universeLayoutScenes[universe]->tube(dmxChan);
		if (tube)
			return tube->scalerNumerator;
	}

	return 1;
}

qint32 LightControl::globalDmxScalerDenominator(quint32 universe, qint32 dmxChan)
{
	if (universe < MAX_DMX_UNIVERSE && universeLayoutScenes[universe]) {
		DmxChannel *tube = universeLayoutScenes[universe]->tube(dmxChan);
		if (tube)
			return tube->scalerDenominator;
	}

	return 1;
}

void LightControl::init()
{
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		dmxOutputChanged[t] = true;
		dmxInputChanged[t] = true;
		dmxOutputValues[t].fill(0, 512);
		dmxInputValues[t].fill(0, 512);

		universeLayoutScenes[t] = nullptr;
	}
	// initialize the hidden scene that will catch all scanner events.
	FxItem::setLowestID(10000);
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		hiddenScannerScenes[t] = new FxSceneItem;
		hiddenScannerScenes[t]->setTubeCount(512, t);
		hiddenScannerScenes[t]->setName(QString("ScanMove%1").arg(t+1));

		for (int i=0; i<MAX_STATIC_SCENES; i++) {
			staticScenes[t][i] = nullptr;
		}
	}
	FxItem::setLowestID(1);

	lightLoopInterface = new LightLoopThreadInterface(*this);
	connect(lightLoopInterface,SIGNAL(wantedDeleteFxScene(FxSceneItem*)),&myApp,SLOT(deleteFxSceneItem(FxSceneItem*)));
}

void LightControl::startFxSceneExecuter(FxSceneItem *scene)
{
	SceneExecuter *exec = myApp.execCenter->newSceneExecuter(scene,scene->parentFxItem());
	exec->activateProcessing();
}

void LightControl::onSceneStatusChanged(FxSceneItem *scene, quint32 status)
{
	if (debug > 1) qDebug() << "Scene" << scene->name() << "INTERN active:" << (status & SCENE_ACTIVE_INTERN)
			 << "INTERN:" << (status & SCENE_STAGE_INTERN)
			 << "LIVE:" << (status & SCENE_STAGE_LIVE)
			 << "EXTERN active:" << (status & SCENE_ACTIVE_EXTERN) << "EXTERN:" << (status & SCENE_STAGE_EXTERN);

	emit sceneChanged(scene);
}

void LightControl::onSceneFadeProgressChanged(FxSceneItem *scene, int perMilleA, int perMilleB)
{
	emit sceneFadeChanged(scene, perMilleA, perMilleB);
}

void LightControl::onSceneCueReady(FxSceneItem *scene)
{
	emit sceneCueReady(scene);
}


/**
 * @brief Handler für reinkommende Inputsignale
 * @param universe
 * @param channel dmx-kanal (0-511)
 * @param value dmx-wert (0-255)
 *
 * Diese Funktion stellt die Verbindung einer Eingangsleitung mit einem FxSceneItem dar
 *
 * @todo make thread safe
 */
void LightControl::onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value)
{
	if (universe >= MAX_DMX_UNIVERSE || channel > 511) return;

	// store incoming value to dmx input universe array.
	if (dmxInputValues[universe][channel] != char(value)) {
		dmxInputValues[universe][channel] = value;
		dmxInputChanged[universe] = true;
	}

	// qDebug() << "dmx in changed" << universe << channel << value;

	if (myApp.isApplicationStart())
		return;

	if (myApp.isInputAssignMode())
		return myApp.assignInputToSelectedFxItem(universe, channel, value);

	/// @todo faster lookup of fxitems that are hooked to an dmx channel
	for (FxItem * fx : std::as_const(FxItem::globalFxList())) {
		if (fx->isHookedToInput(universe,channel)) {
			if (fx->fxType() == FX_SCENE) {
				FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
				int response_time = myApp.pluginCentral->fastMapInUniverse[universe].responseTime;
				// qDebug("Direct Fade Scene: %s to %d (time:%d)",scene->name().toLocal8Bit().data(),value,response_time);

				// This is to force the operator to draw the input slider to Null-Level, when a BLACK command was emitted
				if (scene->isBlacked(MIX_EXTERN)) {
					if (value < 10) {
						scene->setBlacked(MIX_EXTERN,false);
					}
				}
				else if (scene->directFadeToDmx(value,response_time)) {
					setSceneActive(scene);
				}
			}
			else if (fx->fxType() == FX_AUDIO) {
				FxAudioItem *audio = static_cast<FxAudioItem*>(fx);
				myApp.unitAudio->handleDmxInputAudioEvent(audio, value);
			}
			else if (fx->fxType() == FX_SCRIPT) {
				// todo start fxscript

			}
		}
	}

	// change volume of audio slot, if DMX input is mapped to an audio channel
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (myApp.userSettings->mapAudioToDmxUniv[t] == qint32(universe + 1)
				&& myApp.userSettings->mapAudioToDmxChan[t] == qint32(channel + 1) ) {
			emit audioSlotVolChanged(t, value);

		}
	}
}

void LightControl::setValueInHiddenScannerScene(const TubeData &tubeDat)
{
	if ((tubeDat.universe < 0) || (tubeDat.universe > MAX_DMX_UNIVERSE))
		return;
	qDebug() << "changed in hidden scanner scene uni:" << tubeDat.universe+1 << "channel:" << tubeDat.dmxChan+1
			 << "val[0]:" << tubeDat.curValue[0] << "val[1]:" << tubeDat.curValue[1];
	FxSceneItem *scanscene = hiddenScannerScenes[tubeDat.universe];
	DmxChannel *scantube = scanscene->tube(tubeDat.dmxChan);
	if (tubeDat.dmxType >= 0)
		scantube->dmxType = tubeDat.dmxType;
	if (tubeDat.targetValue >= 0)
		scantube->targetValue = tubeDat.targetValue;
	if (tubeDat.fullValue > 0)
		scantube->targetFullValue = tubeDat.fullValue;

	scantube->curValue[0] = tubeDat.curValue[0];
	scantube->curValue[1] = tubeDat.curValue[1];
}

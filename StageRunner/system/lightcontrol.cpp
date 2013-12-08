#include "lightcontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "lightloopthreadinterface.h"
#include "lightloop.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "fxlist.h"
#include "qlcioplugin.h"
#include "ioplugincentral.h"
#include "usersettings.h"


LightControl::LightControl(AppCentral *app_central)
	: QObject()
	,myApp(app_central)
{
	init();
}

LightControl::~LightControl()
{
	delete lightLoopInterface;

}

bool LightControl::setLightLoopEnabled(bool state)
{
	bool ok = true;
	if (state) {
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

bool LightControl::sendChangedDmxData()
{
	/// @implement me: Plugin <-> output <-> universe Zuordnung
	bool sent = false;

	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (dmxOutputChanged[t]) {
			QLCIOPlugin *plugin;
			int output;
			if (myApp->pluginCentral->getPluginAndOutputForDmxUniverse(t,plugin,output)) {
				plugin->writeUniverse(t,dmxOutputValues[t]);
				emit outputUniverseChanged(t,dmxOutputValues[t]);
				sent = true;
			} else {
				if (myApp->userSettings->pNoInterfaceDmxFeedback) {
					emit outputUniverseChanged(t,dmxOutputValues[t]);
				}
			}
			dmxOutputChanged[t] = false;
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

	if (!scene->isOnStageIntern()) {
		active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN);
		if (active) {
			setSceneActive(scene);
		}
	} else {
		active = scene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT);
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
bool LightControl::setSceneActive(FxSceneItem *scene)
{
	if (activeScenes.lockContains(scene->id())) return false;

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

//			if (scene->directFadeToDmx(0,time_ms)) {
//				num++;
//			}
			scene->setLive(false);
		}
	}
	return num;
}

void LightControl::init()
{
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		dmxOutputChanged[t] = true;
		dmxOutputValues[t].resize(512);
		memset(dmxOutputValues[t].data(),0,512);
	}
	lightLoopInterface = new LightLoopThreadInterface(*this);
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
 * @param channel
 * @param value dmx-wert (0-255)
 *
 * Diese Funktion stellt die Verbindung einer Eingangsleitung mit einem FxSceneItem dar
 */
void LightControl::onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value)
{
	if (universe >= MAX_DMX_UNIVERSE) return;

	if (myApp->isInputAssignMode())
		return myApp->assignInputToSelectedFxItem(universe, channel, value);

	foreach (FxItem * fx, FxItem::globalFxList()) {
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
			if (scene->isHookedToInput(universe,channel)) {
				int response_time = myApp->pluginCentral->fastMapInUniverse[universe].responseTime;
				qDebug("Direct Fade Scene: %s to %d (time:%d)",scene->name().toLocal8Bit().data(),value,response_time);

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
		}
	}

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (myApp->userSettings->mapAudioToDmxUniv[t] == qint32(universe + 1)
				&& myApp->userSettings->mapAudioToDmxChan[t] == qint32(channel + 1) ) {
			emit audioSlotVolChanged(t, value);

		}
	}
}

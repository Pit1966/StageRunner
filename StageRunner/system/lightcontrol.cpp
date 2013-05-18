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
	bool sent = false;
	QList<QLCIOPlugin*>plugins = myApp->pluginCentral->qlcPlugins();
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (dmxOutputChanged[t]) {
			for (int i=0; i<plugins.size(); i++) {
				plugins.at(i)->writeUniverse(t,dmxOutputValues[t]);
				emit outputUniverseChanged(t,dmxOutputValues[t]);
				sent = true;
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

	if (!scene->isOnStage()) {
		active = scene->initSceneCommand(CMD_SCENE_FADEIN);
		if (active) {
			setSceneActive(scene);
		}
	} else {
		active = scene->initSceneCommand(CMD_SCENE_FADEOUT);
	}

	return active;
}

bool LightControl::stopFxScene(FxSceneItem *scene)
{
	return scene->initSceneCommand(CMD_SCENE_FADEOUT);
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
			if (scene->directFadeToDmx(0,time_ms)) {
				num++;
			}
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
	qDebug() << "Scene" << scene->name() << "status: active:" << (status & SCENE_ACTIVE)
			 << "stage:" << (status & SCENE_STAGE)
			 << "live:" << (status & SCENE_LIVE);

	emit sceneChanged(scene);
}

void LightControl::onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value)
{
	foreach (FxItem * fx, FxItem::globalFxList()) {
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
			if (scene->hookedToInputUniverse == qint32(universe) && scene->hookedToInputDmxChannel == qint32(channel)) {
				// qDebug("Direct Fade Scene: %s to %d",scene->name().toLocal8Bit().data(),value);
				if (scene->directFadeToDmx(value,200)) {
					setSceneActive(scene);
				}
			}
		}
	}
}

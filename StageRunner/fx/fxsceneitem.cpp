#include "fxsceneitem.h"
#include "dmxchannel.h"

FxSceneItem::FxSceneItem()
{
	myFxType = FX_SCENE;

	init();
}

FxSceneItem::~FxSceneItem()
{
	delete sceneMaster;
}

void FxSceneItem::createDefaultTubes(int tubecount)
{
	for (int t=0; t<tubecount; t++) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tube = t;
		dmx->dmxUniverse = 0;
		dmx->dmxChannel = t;
		dmx->dmxType = DMX_INTENSITY;
		tubes.append(dmx);
	}
}

void FxSceneItem::setTubeCount(int tubecount)
{
	while (tubes.size() > tubecount) {
		delete tubes.takeAt(tubes.size()-1);
	}

	while (tubes.size() < tubecount) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tube = tubes.size();
		dmx->dmxUniverse = 0;
		dmx->dmxChannel = tubes.size();
		dmx->dmxType = DMX_INTENSITY;
		tubes.append(dmx);
	}
}

bool FxSceneItem::initSceneCommand(CtrlCmd cmd)
{
	int cmd_time = 0;

	switch(cmd) {
	case CMD_SCENE_BLACK:
		cmd_time = 0;
		myStatus &= ~SCENE_STAGE;
		break;
	case CMD_SCENE_FADEIN:
		cmd_time = defaultFadeInTime;
		myStatus |= SCENE_STAGE;
		break;
	case CMD_SCENE_FADEOUT:
		cmd_time = defaultFadeOutTime;
		myStatus &= ~SCENE_STAGE;
		break;
	case CMD_SCENE_FADETO:
		cmd_time = defaultFadeInTime;
		myStatus |= SCENE_STAGE;
		break;
	default:
		return false;
	}

	bool active = false;

	// Iterate over all tubes and set parameters
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		if (tube->dmxType == DMX_INTENSITY) {
			if (tube->initFadeCmd(cmd,cmd_time)) {
				active = true;
			}
		}
	}

	if (active) {
		myStatus |= SCENE_ACTIVE;
	} else {
		myStatus &= ~SCENE_ACTIVE;
	}

	return active;
}

/**
 * @brief Execute Scene
 * @return true: if at least one channel is active in the scene
 *
 * This function prosseses activ CtrlCmds in the tubes of the scene
 * Usualy loopFunction() is called from a timer function or a event loop;
 */
bool FxSceneItem::loopFunction()
{
	bool active = false;

	for (int t=0; t<tubeCount(); t++) {
		active |= tubes.at(t)->loopFunction();
	}

	if (active) {
		myStatus |= SCENE_ACTIVE;
	} else {
		myStatus &= ~SCENE_ACTIVE;
	}

	return active;
}


/**
 * @brief Check if SceneStatus has changed
 * @return true, if status is different from status of last function call
 *
 * You have to call this function regulary to keep track of status changes
 */
bool FxSceneItem::statusHasChanged()
{
	if (myStatus != my_last_status) {
		my_last_status = myStatus;
		return true;
	}
	return false;
}

void FxSceneItem::init()
{
	myStatus = SCENE_IDLE;
	my_last_status = SCENE_IDLE;

	sceneMaster = new DmxChannel;

	addExistingVar(defaultFadeInTime,"DefFadeInTime");
	addExistingVar(defaultFadeOutTime,"DefFadeOutTime");
	addExistingVar(*sceneMaster,"DmxChannelDummy");
	addExistingVarSetList(tubes,"SceneTubes",PrefVarCore::DMX_CHANNEL);

}

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

#include "fxsceneitem.h"
#include "dmxchannel.h"
#include "lightcontrol.h"
#include "appcentral.h"
#include "lightloop.h"
#include "lightloopthreadinterface.h"

FxSceneItem::FxSceneItem()
	:FxItem()
	,tubes(this)
{
	init();
}

FxSceneItem::FxSceneItem(FxList *fxList)
	:FxItem(fxList)
	,tubes(this)
{
	init();
}

FxSceneItem::FxSceneItem(const FxSceneItem &o)
	: FxItem(o.myParentFxList)
	, tubes(this)
{
	init();
	cloneFrom(o);

	for (int t=0; t<o.tubes.size(); t++) {
		DmxChannel *tube = new DmxChannel(*o.tubes.at(t));
		tubes.append(tube);
	}
}

void FxSceneItem::init()
{
	myFxType = FX_SCENE;
	myclass = PrefVarCore::FX_SCENE_ITEM;
	myStatus = SCENE_IDLE;
	mySeqStatus = SCENE_OFF;
	my_last_status = SCENE_IDLE;
	my_last_active_flag = false;

	m_deleteMeOnFinished = false;

	for (int t=0; t<MIX_LINES; t++) {
		wasBlacked[t] = false;
	}

	sceneMaster = new DmxChannel;
	sceneMaster->targetValue = 1000;

	addExistingVar(widgetPos,"WidgetPos");
	addExistingVar(*sceneMaster,"DmxChannelDummy");
	addExistingVarSetList(tubes,"SceneTubes",PrefVarCore::DMX_CHANNEL);

}

FxSceneItem::~FxSceneItem()
{
	delete sceneMaster;

	while (tubes.size()) {
		delete tubes.takeFirst();
	}

	// qDebug() << "FxSceneItem deleted" << name();
}

void FxSceneItem::setLoopValue(qint32 val)
{
	Q_UNUSED(val);
}

void FxSceneItem::resetFx()
{
	mySeqStatus = SCENE_OFF;
}

void FxSceneItem::createDefaultTubes(int tubecount)
{
	for (int t=0; t<tubecount; t++) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tube = t;
		dmx->dmxUniverse = 0;
		dmx->dmxChannel = t;
		dmx->dmxType = DMX_INTENSITY_DIMMER;
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
		dmx->dmxType = DMX_INTENSITY_DIMMER;
		tubes.append(dmx);
	}
}
/**
 * @brief Init command for FxScene
 * @param mixline MIX_INTERN, MIX_EXTERN to distinguish between manualy fades and automatic fades
 * @param cmd The command that should be performed (@see CtrlCmd)
 * @param cmdTime The command time parameter in ms (e.g. 1000ms for CMD_SCENE_FADEIN)
 * @return true, if scene becomes activ. false, if scene has already the desired stage and levels
 */
bool FxSceneItem::initSceneCommand(int mixline, CtrlCmd cmd, int cmdTime)
{
	LightControl *lightctrl = AppCentral::instance()->unitLight;

	int cmd_time = 0;

	quint32 STAGE_FLAG = SCENE_STAGE_INTERN;
	quint32 ACTIVE_FLAG = SCENE_ACTIVE_INTERN;
	if (mixline == MIX_EXTERN) {
		STAGE_FLAG = SCENE_STAGE_EXTERN;
		ACTIVE_FLAG = SCENE_ACTIVE_EXTERN;
	}

	switch(cmd) {
	case CMD_SCENE_BLACK:
		cmd_time = 0;
		if (myStatus & STAGE_FLAG) {
			myStatus &= ~STAGE_FLAG;
			wasBlacked[mixline] = true;
		}
		break;
	case CMD_SCENE_FADEIN:
		cmd_time = defaultFadeInTime;
		myStatus |= STAGE_FLAG;
		wasBlacked[mixline] = false;
		break;
	case CMD_SCENE_FADEOUT:
		cmd_time = defaultFadeOutTime;
		myStatus &= ~STAGE_FLAG;
		wasBlacked[mixline] = false;
		break;
	case CMD_SCENE_FADETO:
		cmd_time = defaultFadeInTime;
		myStatus |= STAGE_FLAG;
		wasBlacked[mixline] = false;
		break;
	default:
		return false;
	}


	if (cmdTime)
		cmd_time = cmdTime;

	// qDebug("initSceneCommand: %d, status: %d for scene: %s",cmd,myStatus,name().toLocal8Bit().data());
	bool active = false;

	FxSceneItem *scanscene = 0;

	// Iterate over all tubes and set parameters
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		switch (tube->dmxType) {
		case DMX_INTENSITY_DIMMER:
			if (tube->initFadeCmd(mixline,cmd,cmd_time)) {
				active = true;
			}
			break;
		case DMX_POSITION_PAN:
		case DMX_POSITION_TILT:
			if (cmd == CMD_SCENE_FADETO || cmd == CMD_SCENE_FADEIN) {
				scanscene = lightctrl->hiddenScannerScenes[tube->dmxUniverse];
				DmxChannel *scantube = scanscene->tube(tube->dmxChannel);
				qint32 movetime = moveTime();
				// copy channel parameter to hidden scanner scene
				scantube->targetFullValue = tube->targetFullValue;
				scantube->targetValue = tube->targetValue;
				scantube->dmxType = tube->dmxType;

				if (scantube->initFadeCmd(mixline,CMD_SCENE_FADETO,movetime,tube->targetValue)) {
					scanscene->setActiveIntern();
					lightctrl->setSceneActive(scanscene);
				}
			}
			break;
		default:
			break;
		}

	}

	// This is for Progress only
	sceneMaster->initFadeCmd(mixline,cmd,cmd_time);

	if (active) {
		myStatus |= ACTIVE_FLAG;
	} else {
		myStatus &= ~ACTIVE_FLAG;
	}

	return active;
}


/**
 * @brief Fade a Scene to explicit value
 * @param dmxval Target Value (0 to 255: is 0% to 100%)
 * @param time_ms Reaction time in ms;
 * @return true, if scene is active for the moment
 */
bool FxSceneItem::directFadeToDmx(qint32 dmxval, qint32 time_ms)
{
	bool active = false;
	bool onstage = false;

	// Iterate over all tubes and set parameters
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		if (tube->dmxType == DMX_INTENSITY_DIMMER) {
			qint32 target_value = tube->targetValue * dmxval / 255;
			if (tube->initFadeCmd(MIX_EXTERN, CMD_SCENE_FADETO,time_ms,target_value)) {
				active = true;
				if (target_value > 0) {
					onstage = true;
				}
			}
		}
	}
	// This is for Progress only
	sceneMaster->initFadeCmd(MIX_EXTERN,CMD_SCENE_FADETO,time_ms, 1000 * dmxval / 255);


	if (active) {
		myStatus |= SCENE_ACTIVE_EXTERN;
	} else {
		myStatus &= ~SCENE_ACTIVE_EXTERN;
	}

	if (onstage) {
		myStatus |= SCENE_STAGE_EXTERN;
	} else {
		myStatus &= ~SCENE_STAGE_EXTERN;
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
	bool active_auto = false;
	bool active_direct = false;

	for (int t=0; t<tubeCount(); t++) {
		active_auto |= tubes.at(t)->loopFunction(MIX_INTERN);
		active_direct |= tubes.at(t)->loopFunction(MIX_EXTERN);
	}

	// This is for Progress indication
	// a Channel that is invisible in DMX Universe. Fade is done on activation from 0 to 1000
	sceneMaster->loopFunction(MIX_INTERN);
	sceneMaster->loopFunction(MIX_EXTERN);

	if (active_auto) {
		myStatus |= SCENE_ACTIVE_INTERN;
	} else {
		myStatus &= ~SCENE_ACTIVE_INTERN;
	}
	if (active_direct) {
		myStatus |= SCENE_ACTIVE_EXTERN;
	} else {
		myStatus &= ~SCENE_ACTIVE_EXTERN;
	}

	if (myStatus == SCENE_IDLE && m_deleteMeOnFinished)
		qDebug() << __func__ << "delete this scene now!" << name();

	return active_auto || active_direct;
}

void FxSceneItem::setLive(bool state) const
{
	if (state) {
		myStatus |= SCENE_STAGE_LIVE;
	} else {
		myStatus &= ~SCENE_STAGE_LIVE;
	}
}

void FxSceneItem::setBlacked(int mixline, bool state)
{
	wasBlacked[mixline] = state;
}


/**
 * @brief Check if SceneStatus has changed
 * @return true, if status is different from status of last function call
 *
 * You have to call this function regulary to keep track of status changes
 * Note that a call to this function resets the hasChanged flag to false for subsequent calls
 */
bool FxSceneItem::getClearStatusHasChanged()
{
	if (myStatus != my_last_status) {
		my_last_status = myStatus;
		return true;
	}
	return false;
}

bool FxSceneItem::getClearActiveHasChanged()
{
	bool active = isActive();
	if (active != my_last_active_flag) {
		my_last_active_flag = active;
		return true;
	}
	return false;
}

QString FxSceneItem::statusString()
{
	QString str;
	quint32 stat = myStatus;
	if (stat & SCENE_STAGE_LIVE)
		str += "LIVE ";
	if (stat & SCENE_STAGE_INTERN)
		str += "INT ";
	if (stat & SCENE_ACTIVE_INTERN)
		str += "act ";
	if (stat & SCENE_STAGE_EXTERN)
		str += "EXT ";
	if (stat & SCENE_ACTIVE_EXTERN)
		str += "act ";

	if (str.size())
		str.chop(1);

	return str;
}

bool FxSceneItem::postLoadInitTubes(bool restore_light)
{
	bool now_on_stage = false;
	bool was_on_stage = false;

	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		if (tube->curValue[MIX_INTERN] > 0 || tube->curValue[MIX_EXTERN] > 0) {
			was_on_stage = true;
			if (restore_light) {
				now_on_stage = true;
				// tube->curValue = 0;
			} else {
				for (int i=0; i<MIX_LINES; i++) {
					tube->curValue[i] = 0;
				}
				tube->dmxValue = 0;
			}
		}
	}

	if (now_on_stage) {
		qDebug("Post process loaded project -> Bring Scene to stage");
		// initSceneCommand(CMD_SCENE_FADEIN);
		AppCentral::instance()->unitLight->setSceneActive(this);
	}

	if (sceneMaster->curValue[MIX_INTERN]) {
		sceneMaster->curValue[MIX_INTERN] = 0;
		qDebug("Scene: %s reset scene internal master",name().toLocal8Bit().data());
	}

	if (sceneMaster->curValue[MIX_EXTERN]) {
		sceneMaster->curValue[MIX_EXTERN] = 0;
		qDebug("Scene: %s reset scene external master",name().toLocal8Bit().data());
	}

	return was_on_stage;
}

DmxChannel *FxSceneItem::tube(int id) const
{
	if (id < 0 || id >= tubes.size())
		return 0;

	return tubes.at(id);
}

bool FxSceneItem::removeTube(int id)
{
	if (id < 0 || id >= tubes.size()) return false;

	DmxChannel *tube = tubes.takeAt(id);
	delete tube;

	return true;
}


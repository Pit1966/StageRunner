//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include "fx/fxsceneitem.h"
#include "system/dmxchannel.h"
#include "system/lightcontrol.h"
#include "system/dmx/dmxhelp.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/lightloop.h"
#include "appcontrol/lightloopthreadinterface.h"

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
	VarSet::cloneFrom(o);
	for (int t=0; t<o.tubes.size(); t++) {
		DmxChannel *tube = new DmxChannel(*o.tubes.at(t));
		tubes.append(tube);
	}
}

void FxSceneItem::cloneFrom(const FxSceneItem &o)
{
	VarSet::cloneFrom(o);
	for (int t=0; t<o.tubes.size(); t++) {
		DmxChannel *tube = new DmxChannel(*o.tubes.at(t));
		if (tubes.size() > t) {
			delete tubes[t];
			tubes[t] = tube;
		}
		else {
			tubes.append(tube);
		}
	}

	while (tubes.size() > o.tubes.size())
		delete tubes.takeLast();
}

void FxSceneItem::init()
{
	myFxType = FX_SCENE;
	myclass = PrefVarCore::FX_SCENE_ITEM;
	myStatus = SCENE_IDLE;
	mySeqStatus = SCENE_OFF;
	mLastStatus = SCENE_IDLE;
	mLastActiveFlag = false;

	m_deleteMeOnFinished = false;

	for (int t=0; t<MIX_LINES; t++) {
		m_wasBlacked[t] = false;
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

bool FxSceneItem::isUsed() const
{
	return myStatus != SCENE_IDLE;
}

qint32 FxSceneItem::durationHint() const
{
	return fadeInTime() + holdTime() + fadeOutTime();
}

qint32 FxSceneItem::getNewTubeId() const
{
	int newid = -1;
	for (int t=0; t<tubes.size(); t++) {
		newid = qMax(tubes.at(t)->tubeId, newid);
	}
	return newid + 1;
}

qint32 FxSceneItem::getNextUnusedDmxChannel(int universe) const
{
	int newchan = -1;
	for (int t=0; t<tubes.size(); t++) {
		if (tubes.at(t)->dmxUniverse == universe) {
			newchan = qMax(tubes.at(t)->dmxChannel, newchan);
		}
	}
	return newchan + 1;
}

qint32 FxSceneItem::guessUniverse() const
{
	return tubes.size() ? tubes.last()->dmxUniverse : 0;
}

qint32 FxSceneItem::firstUsedDmxChannel() const
{
	int chan = 511;
	for (int t=0; t<tubes.size(); t++) {
		if (tubes.at(t)->dmxChannel < chan)
			chan = tubes.at(t)->dmxChannel;
	}

	return chan;
}

qint32 FxSceneItem::lastUsedDmxChannel() const
{
	int chan = 0;
	for (int t=0; t<tubes.size(); t++) {
		if (tubes.at(t)->dmxChannel > chan)
			chan = tubes.at(t)->dmxChannel;
	}

	return chan;
}

void FxSceneItem::createDefaultTubes(int tubecount, uint universe)
{
	if (universe >= MAX_DMX_UNIVERSE)
		universe = MAX_DMX_UNIVERSE-1;

	for (int t=0; t<tubecount; t++) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tubeId = t;
		dmx->dmxUniverse = universe;
		dmx->dmxChannel = t;
		dmx->dmxType = DMX_GENERIC;
		tubes.append(dmx);
	}
}

void FxSceneItem::setTubeCount(int tubecount, int defaultUniverse)
{
	if (tubes.size() == tubecount)
		return;

	while (tubes.size() > tubecount) {
		delete tubes.takeAt(tubes.size()-1);
	}

	while (tubes.size() < tubecount) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tubeId = getNewTubeId();
		dmx->dmxUniverse = defaultUniverse;
		dmx->dmxChannel = getNextUnusedDmxChannel(defaultUniverse);
		dmx->dmxType = DMX_GENERIC;
		tubes.append(dmx);
	}

	setModified();
}

/**
 * @brief Set tubes for scene to given dmx channel range.
 * @param firstDmxChan
 * @param lastDmxChan
 * @param defaultUniverse
 */
bool FxSceneItem::setTubeChannelRange(int firstDmxChan, int lastDmxChan, int defaultUniverse)
{
	if (firstDmxChan > lastDmxChan)
		qSwap(firstDmxChan, lastDmxChan);

	int tubes_wanted = lastDmxChan - firstDmxChan + 1;
	if (tubes_wanted <= 0 || firstDmxChan < 1)
		return false;

	// Do we have more tubes as wanted in scene, than remove
	while (tubes.size() > tubes_wanted) {
		delete tubes.takeAt(tubes.size()-1);
	}

	// Do we have now enough tubes, than add new tubes
	while (tubes.size() < tubes_wanted) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tubeId = getNewTubeId();
		dmx->dmxUniverse = defaultUniverse;
		tubes.append(dmx);
	}

	// renumber all tubes
	// not clear, if tubeID must match the dmxChannel number - 1
	int dmxchan = firstDmxChan;
	for (int i=0; i<tubes.size(); i++) {
		DmxChannel *dmx = tubes.at(i);
		dmx->dmxUniverse = defaultUniverse;
		dmx->dmxChannel = dmxchan;
		dmx->deskPositionIndex = -1;
		dmx->tubeId = i;

		dmxchan++;
	}

	setModified();

	return true;
}

void FxSceneItem::addTube()
{
	int univ = tubes.size() ? tubes.last()->dmxUniverse : 0;
	return setTubeCount(tubes.size() + 1, univ);
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
			m_wasBlacked[mixline] = true;
		}
		break;
	case CMD_SCENE_FADEIN:
		cmd_time = defaultFadeInTime;
		myStatus |= STAGE_FLAG;
		m_wasBlacked[mixline] = false;
		break;
	case CMD_SCENE_FADEOUT:
		cmd_time = defaultFadeOutTime;
		myStatus &= ~STAGE_FLAG;
		m_wasBlacked[mixline] = false;
		break;
	case CMD_SCENE_FADETO:
		cmd_time = defaultFadeInTime;
		myStatus |= STAGE_FLAG;
		m_wasBlacked[mixline] = false;
		break;
	default:
		return false;
	}


	if (cmdTime)
		cmd_time = cmdTime;

	// qDebug("initSceneCommand: %d, status: %d for scene: %s",cmd,myStatus,name().toLocal8Bit().data());
	bool active = false;
	bool onStage = false;

	FxSceneItem *scanscene = nullptr;

	// Iterate over all tubes and set parameters
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		if (tube->isPairedSub)
			continue;
		if (tube->deskVisibleFlag == false)
			continue;

		DmxChannelType dmxtype = tube->dmxChannelType();
		qint32 dmxUniv = tube->dmxUniverse;
		qint32 dmxChan = tube->dmxChannel;
		DmxChannel *gTube = lightctrl->globalDmxChannel(dmxUniv, dmxChan);

		if (t+1 < tubeCount()) {
			// detect paired Channels (eg. for 16 bit access)
			// We assume that the XXXFine channel is the next channel
			switch (dmxtype) {
			case DMX_POSITION_PAN:
				if (tubes.at(t+1)->dmxChannelType() == DMX_POSITION_PAN_FINE) {
					tube->setPairedWith(tubes.at(t+1));
				} else {
					tube->clrPairedWith();
				}
				break;
			case DMX_POSITION_TILT:
				if (tubes.at(t+1)->dmxChannelType() == DMX_POSITION_TILT_FINE) {
					tube->setPairedWith(tubes.at(t+1));
				} else {
					tube->clrPairedWith();
				}
				break;
			default:
				break;
			}
		}

		switch (dmxtype) {
		case DMX_POSITION_PAN:
		case DMX_POSITION_TILT:
			if (cmd == CMD_SCENE_FADETO || cmd == CMD_SCENE_FADEIN) {
				scanscene = lightctrl->hiddenScannerScenes[tube->dmxUniverse];
				DmxChannel *scantube = scanscene->tube(tube->dmxChannel);

				qint32 movetime = moveTime();
				// copy channel parameter to hidden scanner scene
				scantube->targetFullValue = tube->targetFullValue;
				scantube->targetValue = tube->targetValue;

				scantube->dmxType = dmxtype;
				if (tube->scalerNumerator > 1 || tube->scalerDenominator > 1 || gTube == nullptr) {
					// set local scale values
					scantube->scalerNumerator = tube->scalerNumerator;
					scantube->scalerDenominator = tube->scalerDenominator;
				} else {
					// set global scale values
					scantube->scalerNumerator = gTube->scalerNumerator;
					scantube->scalerDenominator = gTube->scalerDenominator;
				}

				qint32 target_value = tube->targetValue;
				if (tube->isPairedMain) {
					// We have to pair the channel in the hidden scanner scene
					// We assume, that paired channel is the next channel in dmx universe.
					/// @todo we could check the dmx channel numbers here
					DmxChannel *scanpaired = scanscene->tube(tube->dmxChannel + 1);
					scantube->setPairedWith(scanpaired);    ///< @todo needed? haben wir oben schon gemacht
					// now copy channel parameter of the paired channel.
					DmxChannel *tubePaired = tube->pairedDmxChannel;
					scanpaired->targetFullValue = tubePaired->targetFullValue;
					scanpaired->targetValue = tubePaired->targetValue;

					// find global tube for pairedDmxChannel for global defined attributes
					DmxChannel *gpTube = lightctrl->globalDmxChannel(tubePaired->dmxUniverse, tubePaired->dmxChannel);
					if (tubePaired->dmxType > DMX_GENERIC || gpTube == nullptr) {
						scanpaired->dmxType = tubePaired->dmxType;
					} else {
						scanpaired->dmxType = gpTube->dmxType;
					}
					if (tubePaired->scalerNumerator > 1 || tubePaired->scalerDenominator > 1 || gpTube == nullptr) {
						scanpaired->scalerNumerator = tubePaired->scalerNumerator;
						scanpaired->scalerDenominator = tubePaired->scalerDenominator;
					} else {
						scanpaired->scalerNumerator = gpTube->scalerNumerator;
						scanpaired->scalerDenominator = gpTube->scalerDenominator;
					}

					target_value *= tubePaired->targetFullValue;
					target_value += tubePaired->targetValue;
				}
				else {
					scantube->clrPairedWith();
				}

				if (scantube->initFadeCmd(mixline, CMD_SCENE_FADETO, movetime, target_value)) {
					if (!scanscene->isActiveIntern()) {
						scanscene->setActiveIntern();
						lightctrl->setSceneActive(scanscene);
					}
				}
			}
			break;

		case DMX_SPEED_PAN_TILT_FAST_SLOW:
		case DMX_SPEED_PAN_TILT_SLOW_FAST:

			if (cmd == CMD_SCENE_FADETO || cmd == CMD_SCENE_FADEIN) {
				scanscene = lightctrl->hiddenScannerScenes[tube->dmxUniverse];
				DmxChannel *scantube = scanscene->tube(tube->dmxChannel);

				qint32 movetime = moveTime();
				qint32 target_value = tube->targetValue;

				// copy channel parameter to hidden scanner scene
				scantube->targetFullValue = tube->targetFullValue;
				scantube->targetValue = target_value;
				scantube->dmxType = dmxtype;

				scantube->clrPairedWith();

				if (scantube->initFadeCmd(mixline, CMD_SCENE_FADETO, movetime, target_value)) {
					if (!scanscene->isActiveIntern()) {
						scanscene->setActiveIntern();
						lightctrl->setSceneActive(scanscene);
					}
				}
			}
			break;

		default:
			// all other types are handled like dimmers
			if (tube->initFadeCmd(mixline,cmd,cmd_time)) {
				active = true;
				if (tube->dmxType < DMX_POSITION_PAN)
					onStage = true;
			}
			break;

		}

	}

	// This is for Progress only
	sceneMaster->initFadeCmd(mixline, cmd, cmd_time);
	if (scanscene) {
		int activeTime = qMax(defaultMoveTime, cmd_time);
		scanscene->sceneMaster->curValue[mixline] = 0;
		scanscene->sceneMaster->initFadeCmd(mixline, cmd, activeTime);
	}

	// qDebug() << "init scene" << name() << "now: onstage" << onStage << "active" << active;

	if (mixline == MIX_INTERN && cmd >= CMD_SCENE_FADEIN && onStage == false)
		myStatus &= ~SCENE_STAGE_INTERN;

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
		if (DMXHelp::isTypeDimmer(tube->dmxChannelType())) {
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
		// if fade has finished, check, if Scene is on Stage.
		if (mLastStatus & SCENE_ACTIVE_INTERN/* && mLastStatus & SCENE_STAGE_INTERN*/) {
			qDebug() << myName;
			if (!evaluateOnStageIntern())
				myStatus &= ~SCENE_STAGE_INTERN;
		}
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
	m_wasBlacked[mixline] = state;
}

bool FxSceneItem::isOnStageIntern() const
{
	bool onstage = myStatus & SCENE_STAGE_INTERN;
	// bool evalonstage = evaluateOnStageIntern();
	// qDebug() << "scene" << name() << "onstage" << onstage << "eval" << evalonstage;

	if (onstage == true && !evaluateOnStageIntern()) {
		myStatus &= ~SCENE_STAGE_INTERN;
		qWarning() << "scene" << myName << "marked active, but has no active intensity dimmers";
		return false;
	}

	return onstage;
}

/**
 * @brief Check all tubes of the scene and decide, if the scene is on stage
 * @return
 *
 * on stage means, that there are dimmers with levels > 0.
 * PAN/TILT and similar DMX channel types are not taken into account
 */
bool FxSceneItem::evaluateOnStageIntern() const
{
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		// this gets local dmxType in tube, if set, otherwise the global DMX type
		// from universe layout. May result in DMX_GENERIC(0)
		DmxChannelType type = tube->dmxChannelType();
		if (type < DMX_POSITION_PAN && 0 < tube->curValue[MIX_INTERN]) {
			return true;
		}
	}

	return false;
}

/**
 * @brief Check if scene has no active dimmers, but only PAN/TILT/MACRO channels
 * @return
 *
 * if a scene has no dimmers, it is most likely a scanner position scene.
 */
bool FxSceneItem::evaluateHasNoDimmers() const
{
	for (int t=0; t<tubeCount(); t++) {
		DmxChannel *tube = tubes.at(t);
		// this gets local dmxType in tube, if set, otherwise the global DMX type
		// from universe layout. May result in DMX_GENERIC(0)
		DmxChannelType type = tube->dmxChannelType();
		if (type < DMX_POSITION_PAN && 0 < tube->targetValue) {
			return false;
		}
	}

	return true;
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
	if (myStatus != mLastStatus) {
		mLastStatus = myStatus;
		return true;
	}
	return false;
}

bool FxSceneItem::getClearActiveHasChanged()
{
	bool active = isActive();
	if (active != mLastActiveFlag) {
		mLastActiveFlag = active;
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

bool FxSceneItem::updateSceneFromOlderProjectVersion(int oldVer)
{
	if (oldVer < 3) {
		for (int t = 0; t < tubeCount(); ++t) {
			DmxChannel *tu = tube(t);
			switch (tu->dmxType) {
			case 0: tu->dmxType = DMX_GENERIC; break;
			case 1: tu->dmxType = DMX_GENERIC; break;
			case 2: tu->dmxType = DMX_POSITION_PAN; break;
			case 3: tu->dmxType = DMX_POSITION_TILT; break;
			case 4: tu->dmxType = DMX_GOBO_INDEX; break;
			case 5: tu->dmxType = DMX_COLOR_WHEEL; break;
			default:
				qWarning() << "Unknown dmxType while upgrading a scene from project format" << oldVer;
				return false;
			}

		}
	}
	return true;
}

bool FxSceneItem::isEqual(const FxSceneItem *o) const
{
	if (tubes.size() != o->tubes.size())
		return false;

	for (int i=0; i<tubes.size(); i++) {
		if (tubes.at(i)->targetValue != o->tubes.at(i)->targetValue)
			return false;
		if (tubes.at(i)->dmxUniverse != o->tubes.at(i)->dmxUniverse)
			return false;
		if (tubes.at(i)->dmxChannel != o->tubes.at(i)->dmxChannel)
			return false;
		if (tubes.at(i)->dmxType != o->tubes.at(i)->dmxType)
			return false;
		if (tubes.at(i)->labelText != o->tubes.at(i)->labelText)
			return false;
		if (tubes.at(i)->scalerNumerator != o->tubes.at(i)->scalerNumerator)
			return false;
		if (tubes.at(i)->scalerDenominator != o->tubes.at(i)->scalerDenominator)
			return false;

	}

	return true;
}

DmxChannel *FxSceneItem::tube(int idx) const
{
	if (idx < 0 || idx >= tubes.size())
		return nullptr;

	return tubes.at(idx);
}

DmxChannel *FxSceneItem::findTube(int tubeId) const
{
	for (int i=0; i<tubes.size(); i++) {
		if (tubes.at(i)->tubeId == tubeId)
			return tubes.at(i);
	}

	return nullptr;
}

DmxChannel *FxSceneItem::findTube(int univ, int dmxchan) const
{
	for (int t=0; t<tubes.size(); t++) {
		DmxChannel *dmx = tubes.at(t);
		if (dmx->dmxChannel == dmxchan && dmx->dmxUniverse == univ) {
			return dmx;
		}
	}
	return nullptr;
}

bool FxSceneItem::removeTubeById(int tubeId)
{
	for (int i=0; i<tubes.size(); i++) {
		if (tubes.at(i)->tubeId == tubeId) {
			DmxChannel *tube = tubes.takeAt(i);
			delete tube;
			setModified();
			return true;
		}
	}
	return false;
}

bool FxSceneItem::removeTube(DmxChannel *tube)
{
	for (int i=0; i<tubes.size(); i++) {
		if (tubes.at(i) == tube) {
			DmxChannel *tube = tubes.takeAt(i);
			delete tube;
			setModified();
			return true;
		}
	}
	return false;
}


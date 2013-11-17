#include "dmxchannel.h"
#include "config.h"

DmxChannel::DmxChannel()
	: VarSet()
{
	init();
}

DmxChannel::DmxChannel(const DmxChannel &o)
	: VarSet()
{
	init();

	tube = o.tube;
	dmxType = o.dmxType;
	dmxUniverse = o.dmxUniverse;
	dmxChannel = o.dmxChannel;
	dmxValue = o.dmxValue;
	curValue[MIX_INTERN] = o.curValue[MIX_INTERN];
	curValue[MIX_EXTERN] = o.curValue[MIX_EXTERN];
	targetFullValue = o.targetFullValue;
	targetValue = o.targetValue;
	deskVisibleFlag = o.deskVisibleFlag;
	deskPositionIndex = o.deskPositionIndex;
	labelText = o.labelText;
}


void DmxChannel::init()
{
	tempDeskPosIdx = 0;
	tempTubeListIdx = 0;
	for (int i=0; i<MIX_LINES; i++) {
		fadeStartValue[i] = 0;
		fadeTargetValue[i] = 0;
		fadeValue[i] = 0;
		fadeStep[i] = 0;
		curCmd[i] = CMD_NONE;
		curValueChanged[i] = false;
	}

	setClass(PrefVarCore::DMX_CHANNEL,"DmxChannel");
	setDescription("Output mapping from tube number to dmx channel and configuration");
	addExistingVar(tube,"TubeNumber",0,1000,0);
	addExistingVar(dmxType,"DmxType",DMX_GENERIC,DMX_TYPES,DMX_GENERIC);
	addExistingVar(dmxUniverse,"DmxUniverse",0,3,0);
	addExistingVar(dmxChannel,"DmxChannel",0,511,0);
	addExistingVar(dmxValue,"DmxValue",0,255,0);
	addExistingVar(curValue[MIX_INTERN],"Value",0,255,0);
	addExistingVar(curValue[MIX_EXTERN],"DirectValue",0,255,0);
	addExistingVar(targetFullValue,"TargetFullValue",255,100000,10000);
	addExistingVar(targetValue,"TargetValue",0,100000,0);
	addExistingVar(deskVisibleFlag,"DeskVisible",true);
	addExistingVar(deskPositionIndex,"DeskPosIndex",-1,511,-1);
	addExistingVar(labelText,"LabelText");
}


DmxChannel::~DmxChannel()
{
}

/**
 * @brief Execute fade command for channel
 * @param cmd Fade command @see CtrlCmd
 * @param time_ms time the fade will run in ms
 * @param target_value some commands need a target fade value (e.g. CMD_SCENE_FADETO)
 * @return True if a fade will occur. False if current output value is already the target value
 *
 * The Command will be saved in 'curCmd'
 */
bool DmxChannel::initFadeCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 target_value)
{
	fadeStartValue[mixline] = curValue[mixline];
	fadeValue[mixline] = curValue[mixline];

	switch(cmd) {
	case CMD_SCENE_BLACK:
		if (curValue[mixline] == 0) return false;
		fadeTargetValue[mixline] = 0;
		// time_ms = 0;
		break;
	case CMD_SCENE_FADEIN:
		if (curValue[mixline] >= targetValue) return false;
		fadeTargetValue[mixline] = targetValue;
		break;
	case CMD_SCENE_FADEOUT:
		if (curValue[mixline] <= 0) return false;
		fadeTargetValue[mixline] = 0;
		break;
	case CMD_SCENE_FADETO:
		if (curValue[mixline] == target_value) return false;
		fadeTargetValue[mixline] = target_value;
		break;
	default:
		return false;
	}


	// Calculate the fade step size as a function of the LightLoop time interval
	qint32 steps = time_ms / LIGHT_LOOP_INTERVAL_MS;
	if (steps < 1) {
		fadeStep[mixline] = fadeTargetValue[mixline] - fadeStartValue[mixline];
	} else {
		fadeStep[mixline] = (fadeTargetValue[mixline] - fadeStartValue[mixline]) / steps;
	}

	curCmd[mixline] = cmd;
	return true;
}

/**
 * @brief Execute running commands
 * @return true: if command is (still) running, false: if channel is idle (fade has ended)
 *
 * loopFunction() should be called in the EventLoop
 */
bool DmxChannel::loopFunction(int mixline)
{
	int i = mixline;

	if (curCmd[i] == CMD_NONE) return false;

	fadeValue[i] += fadeStep[i];
	int val = fadeValue[i];
	if (val != curValue[i]) {
		curValue[i] = fadeValue[i];
		curValueChanged[i] = true;
	}

	switch(curCmd[i]) {
	case CMD_SCENE_BLACK:
		if (fadeValue[i] <= 0) {
			curValue[i] = 0;
			curCmd[i] = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADEIN:
		if (fadeValue[i] >= fadeTargetValue[i]) {
			curValue[i] = targetValue;
			curCmd[i] = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADEOUT:
		if (fadeValue[i] <= 0) {
			curValue[i] = 0;
			curCmd[i] = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADETO:
		if (fadeStep[i] > 0 && fadeValue[i] >= fadeTargetValue[i]) {
			curValue[i] = fadeTargetValue[i];
			curCmd[i] = CMD_NONE;
			return false;
		}
		else if (fadeStep[i] < 0 && fadeValue[i] <= fadeTargetValue[i]) {
			curValue[i] = fadeTargetValue[i];
			curCmd[i] = CMD_NONE;
			return false;
		}
		break;
	default:
		return false;
	}



	return true;
}



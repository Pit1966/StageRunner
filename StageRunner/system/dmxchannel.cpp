#include "dmxchannel.h"
#include "config.h"

DmxChannel::DmxChannel()
	: VarSet()
{
	init();
}

DmxChannel::~DmxChannel()
{
}

/**
 * @brief Execute fade command for channel
 * @param cmd Fade command @see CtrlCmd
 * @param time_ms time the fade will run in ms
 * @param target_value some commands need a target fade value (e.g. CMD_SCENE_FADETO)
 * @return true if initialized ok
 *
 * The Command will be saved in 'curCmd'
 */
bool DmxChannel::initFadeCmd(CtrlCmd cmd, qint32 time_ms, qint32 target_value)
{
	fadeStartValue = curValue;
	fadeValue = curValue;

	switch(cmd) {
	case CMD_SCENE_BLACK:
		fadeTargetValue = 0;
		time_ms = 0;
		break;
	case CMD_SCENE_FADEIN:
		fadeTargetValue = targetValue;
		break;
	case CMD_SCENE_FADEOUT:
		fadeTargetValue = 0;
		break;
	case CMD_SCENE_FADETO:
		fadeTargetValue = target_value;
		break;
	default:
		return false;
	}


	// Calculate the fade step size as a function of the LightLoop time interval
	qint32 steps = time_ms / LIGHT_LOOP_INTERVAL_MS;
	if (steps < 1) {
		fadeStep = fadeTargetValue - fadeStartValue;
	} else {
		fadeStep = (fadeTargetValue - fadeStartValue) / steps;
	}

	curCmd = cmd;
	return true;
}

/**
 * @brief Execute running commands
 * @return true: if command is (still) running, false: if channel is idle
 *
 * loopFunction() should be called in the EventLoop
 */
bool DmxChannel::loopFunction()
{
	if (curCmd == CMD_NONE) return false;

	fadeValue += fadeStep;
	curValue = fadeValue;

	switch(curCmd) {
	case CMD_SCENE_BLACK:
		if (fadeValue <= 0) {
			curValue = 0;
			curCmd = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADEIN:
		if (fadeValue >= fadeTargetValue) {
			curValue = targetValue;
			curCmd = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADEOUT:
		if (fadeValue <= 0) {
			curValue = 0;
			curCmd = CMD_NONE;
			return false;
		}
		break;
	case CMD_SCENE_FADETO:
		if (fadeStep > 0 && fadeValue >= fadeTargetValue) {
			curValue = fadeTargetValue;
			curCmd = CMD_NONE;
			return false;
		}
		else if (fadeStep < 0 && fadeValue <= fadeTargetValue) {
			curValue = fadeTargetValue;
			curCmd = CMD_NONE;
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}


void DmxChannel::init()
{
	fadeStartValue = 0;
	fadeTargetValue = 0;
	fadeValue = 0;
	fadeStep = 0;
	curCmd = CMD_NONE;

	setClass(PrefVarCore::DMX_CHANNEL,"DmxChannel");
	setDescription("Output mapping from tube number to dmx channel and configuration");
	addExistingVar(tube,"TubeNumber",0,1000,0);
	addExistingVar(dmxType,"DmxType",DMX_GENERIC,DMX_TYPES,DMX_GENERIC);
	addExistingVar(dmxUniverse,"DmxUniverse",0,3,0);
	addExistingVar(dmxChannel,"DmxChannel",0,511,0);
	addExistingVar(dmxValue,"DmxValue",0,255,0);
	addExistingVar(curValue,"Value",0,255,0);
	addExistingVar(targetFullValue,"TargetFullValue",255,100000,10000);
	addExistingVar(targetValue,"TargetValue",0,100000,0);
	addExistingVar(deskVisibleFlag,"DeskVisible",true);
}

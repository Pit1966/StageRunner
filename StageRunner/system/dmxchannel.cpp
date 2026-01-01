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

#include "dmxchannel.h"
#include "config.h"
#include "appcontrol/appcentral.h"

DmxChannel::DmxChannel()
	: VarSet()
{
	init();
}

DmxChannel::DmxChannel(const DmxChannel &o)
	: VarSet()
{
	init();

	tubeId = o.tubeId;
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
	scalerNumerator = o.scalerNumerator;
	scalerDenominator = o.scalerDenominator;
	labelText = o.labelText;

	tempTubeListIdx = -1;
	isPairedMain = o.isPairedMain;
	isPairedSub = o.isPairedSub;
}


void DmxChannel::init()
{
	for (int i=0; i<MIX_LINES; i++) {
		fadeStartValue[i] = 0;
		fadeTargetValue[i] = 0;
		fadeValue[i] = 0;
		fadeStep[i] = 0;
		curCmd[i] = CMD_NONE;
		curValueChanged[i] = false;
	}

	tempTubeListIdx = -1;

	isPairedMain = false;
	isPairedSub = false;

	setClass(PrefVarCore::DMX_CHANNEL,"DmxChannel");
	setDescription("Output mapping from tube number to dmx channel and configuration");
	addExistingVar(tubeId,"TubeNumber",0,1000,0);
	varList.last()->setLoadSecond("DmxChannel");
	addExistingVar(dmxType,"DmxType",DMX_GENERIC,DMX_TYPES,DMX_GENERIC);
	addExistingVar(dmxUniverse,"DmxUniverse",0,3,0);
	addExistingVar(dmxChannel,"DmxChannel",0,511,0);
	addExistingVar(dmxValue,"DmxValue",0,255,0);
	addExistingVar(curValue[MIX_INTERN],"Value",0,255,0);
	addExistingVar(curValue[MIX_EXTERN],"DirectValue",0,255,0);
	addExistingVar(targetFullValue,"TargetFullValue",255,100000,MAX_DMX_FADER_RANGE);
	addExistingVar(targetValue,"TargetValue",0,100000,0);
	addExistingVar(deskVisibleFlag,"DeskVisible",true);
	addExistingVar(deskPositionIndex,"DeskPosIndex",-1,511,-1);
	addExistingVar(scalerNumerator,"ScalerNum",1,0xfffffff,1);
	addExistingVar(scalerDenominator,"ScalerDen",1,0xfffffff,1);
	addExistingVar(deviceID,"DeviceID",-1,0xffffff,-1);
	addExistingVar(labelText,"LabelText");
}

DmxChannel::~DmxChannel()
{
}

/**
 * @brief Execute fade command for channel
 * @param cmd Fade command @see CtrlCmd
 * @param time_ms duration the fade will run in ms
 * @param target_value some commands need a target fade value (e.g. CMD_SCENE_FADETO)
 * @return True if a fade will occur. False if current output value is already the target value
 *
 * The Command will be saved in 'curCmd'
 */
bool DmxChannel::initFadeCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 target_value)
{
	if (isPairedSub) {
		return false;
	}
	else if (!isPairedMain) {
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

	if (!pairedDmxChannel)
		return false;

	// init fade with paired channel
	qint32 curpairvalue = curValue[mixline] * pairedDmxChannel->targetFullValue + pairedDmxChannel->curValue[mixline];
	qint32 targetpairvalue = targetValue * pairedDmxChannel->targetFullValue + pairedDmxChannel->targetValue;

	fadeStartValue[mixline] = curpairvalue;
	fadeValue[mixline] = curpairvalue;

	if (dmxType == DMX_POSITION_TILT) {
		qDebug() << "cur" << curpairvalue << "target" << target_value;
	}

	switch(cmd) {
	case CMD_SCENE_BLACK:
		if (curpairvalue == 0)
			return false;
		fadeTargetValue[mixline] = 0;
		// time_ms = 0;
		break;
	case CMD_SCENE_FADEIN:
		if (curpairvalue >= targetpairvalue)
			return false;
		fadeTargetValue[mixline] = targetpairvalue;
		break;
	case CMD_SCENE_FADEOUT:
		if (curpairvalue <= 0)
			return false;
		fadeTargetValue[mixline] = 0;
		break;
	case CMD_SCENE_FADETO:
		if (curpairvalue == target_value)
			return false;
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

bool DmxChannel::initFadeScannerCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 curScanValue)
{
	fadeStartValue[mixline] = curValue[mixline];
	fadeValue[mixline] = curValue[mixline];

	switch(cmd) {
	case CMD_SCENE_BLACK:
		return false;
	case CMD_SCENE_FADETO:
	case CMD_SCENE_FADEIN:
		if (curValue[mixline] == targetValue) return false;

		qDebug() << dmxChannel << "FadeIN/TO" << curScanValue << targetValue;
		fadeTargetValue[mixline] = targetValue;
		break;
	case CMD_SCENE_FADEOUT:
		return false;
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
	if (isPairedSub)
		return false;

	int i = mixline;

	if (curCmd[i] == CMD_NONE)
		return false;

	if (!isPairedMain) {
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
	}
	else {
		// this is paired 16bit dmx processing for 2 channels
		fadeValue[i] += fadeStep[i];
		// int val = fadeValue[i];

		int max = targetFullValue * pairedDmxChannel->targetFullValue;
		int dmxmax = 65565;
		int dmxval = fadeValue[i] * dmxmax / max;
		if (dmxval > dmxmax)
			dmxval = dmxmax;

		int val_hi = (dmxval >> 8) * targetFullValue / 255;
		int val_lo = (dmxval & 0xff) * pairedDmxChannel->targetFullValue / 255;

		if (val_hi != curValue[i] || val_lo != pairedDmxChannel->curValue[i]) {
			curValue[i] = val_hi;
			curValueChanged[i] = true;
			pairedDmxChannel->curValue[i] = val_lo;
			pairedDmxChannel->curValueChanged[i] = true;
			// qDebug() << "kanal" << dmxChannel + 1 << "val" << val << "mix" << val_hi << val_lo << "dmx" << val_hi * 255 / 10000 << val_lo * 255 / 10000;
		}

		// check fade end conditions
		switch(curCmd[i]) {
		case CMD_SCENE_BLACK:
			if (fadeValue[i] <= 0) {
				curValue[i] = 0;
				pairedDmxChannel->curValue[i] = 0;
				curCmd[i] = CMD_NONE;
				return false;
			}
			break;
		case CMD_SCENE_FADEIN:
			if (fadeValue[i] >= fadeTargetValue[i]) {
				curValue[i] = targetValue;
				pairedDmxChannel->curValue[i] = pairedDmxChannel->targetValue;
				curCmd[i] = CMD_NONE;
				return false;
			}
			break;
		case CMD_SCENE_FADEOUT:
			if (fadeValue[i] <= 0) {
				curValue[i] = 0;
				pairedDmxChannel->curValue[i] = 0;
				curCmd[i] = CMD_NONE;
				return false;
			}
			break;
		case CMD_SCENE_FADETO:
			if (fadeStep[i] > 0 && fadeValue[i] >= fadeTargetValue[i]) {
				curValue[i] = fadeTargetValue[i] / pairedDmxChannel->targetFullValue;
				pairedDmxChannel->curValue[i] = fadeTargetValue[i] - (curValue[i] * pairedDmxChannel->targetFullValue);
				curCmd[i] = CMD_NONE;
				return false;
			}
			else if (fadeStep[i] < 0 && fadeValue[i] <= fadeTargetValue[i]) {
				curValue[i] = fadeTargetValue[i] / pairedDmxChannel->targetFullValue;
				pairedDmxChannel->curValue[i] = fadeTargetValue[i] - (curValue[i] * pairedDmxChannel->targetFullValue);
				curCmd[i] = CMD_NONE;
				return false;
			}
			break;
		default:
			return false;
		}

	}

	return true;
}

DmxChannelType DmxChannel::dmxChannelType() const
{
	if (dmxType > DmxChannelType::DMX_GENERIC)
		return DmxChannelType(dmxType);

	return AppCentral::ref().globalDmxType(dmxUniverse, dmxChannel);
}

DmxChannelType DmxChannel::localDmxType() const
{
	return DmxChannelType(dmxType);
}

DmxChannelType DmxChannel::globalDmxType() const
{
	return AppCentral::ref().globalDmxType(dmxUniverse, dmxChannel);
}

/**
 * @brief DmxChannel::fixtureDeviceID
 * @return The deviceID belonging to the device that contains this channel or -1, if channel has no parent fixture (device)
 *
 * remember: the deviceID for now is the position of the device in the fixture list of given universe
 */
int DmxChannel::globalDeviceID() const
{
	return AppCentral::ref().fixtureDeviceID(dmxUniverse + 1, dmxChannel + 1);
}

/**
 * @brief Get device short ident string for this device containing this channel.
 * @return empty string, if not found
 */
QString DmxChannel::globalDeviceShortId() const
{
	return AppCentral::ref().fixtureDeviceShortId(dmxUniverse + 1, dmxChannel + 1);
}

int DmxChannel::dmxTargetValue() const
{
	int dmx = targetValue * 255 * scalerNumerator / ( targetFullValue * scalerDenominator);

	return qMin(qMax(0, dmx), 255);
}

int DmxChannel::scaledTargetValue() const
{
	int scaledTargetValue = targetValue * scalerNumerator / scalerDenominator;
	return qMin(qMax(0, scaledTargetValue), targetFullValue);
}

bool DmxChannel::setPairedWith(DmxChannel *other)
{
	if (other) {
		pairedDmxChannel = other;
		other->pairedDmxChannel = nullptr;

		isPairedMain = true;
		isPairedSub = false;
		other->isPairedSub = true;
		other->isPairedMain = false;
		return true;
	}

	return false;
}

bool DmxChannel::clrPairedWith()
{
	isPairedMain = false;
	isPairedSub = false;
	if (pairedDmxChannel) {
		pairedDmxChannel->isPairedMain = false;
		pairedDmxChannel->isPairedSub = false;
		pairedDmxChannel = nullptr;
		return true;
	}

	return false;
}



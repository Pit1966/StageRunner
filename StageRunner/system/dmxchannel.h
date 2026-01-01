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

#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include "varset.h"
#include "commandsystem.h"
#include "dmxtypes.h"
#include <QPointer>

#ifndef MIX_LINES
#define MIX_INTERN 0
#define MIX_EXTERN 1
#define MIX_LINES 2
#endif



class DmxChannel : public VarSet
{
public:
	/**
	 * This is the internal channel number.
	 * Channels are called tubes and they do not automatically respond to a dmx channel (but can)
	 */
	qint32 tubeId;
	/**
	 * Type or Function this channel is used for
	 */
	qint32 dmxType;						///< @see DmxChannelType
	qint32 dmxUniverse;					///< DMX universe number [0: MAX] (This is 0 based !!)
	qint32 dmxChannel;					///< DMX channel the function uses [0: 511] (This is 0 based !!)
	qint32 dmxValue;					///< current output value of this dmx channel (0-255)
	qint32 curValue[MIX_LINES];			///< current output value (0 - targetFullRange) HTP with directValue
	qint32 targetFullValue;				///< The maximum value this channel can be set to (default 10000)
	qint32 targetValue;					///< 100% Output value target in a scene (0 - targetFullValue) The dmx Output will be scaled to this
	qint32 deskPositionIndex;			///< The Index the Channel will be positioned in the DeskSceneWidget
	qint32 scalerNumerator;				///< for range scaling e.g. Pan fine
	qint32 scalerDenominator;			///< for range scaling e.g. Pan fine
	qint32 deviceID;					///< the Device index in device list (global template universe). universe is coded in bit [16:23], device index number is codied in bit [0:15] device index number is 1 based
	int tempTubeListIdx;				///< index in tubeList of scene, which is the parent for this DmxChannel
	QString labelText;					///< Spezielle Beschriftung des Kanals

	bool deskVisibleFlag;				///< Show this channel in GUI (scene)
	bool curValueChanged[MIX_LINES];

	// combined channel support. Such as 16bit pan/tilt
	bool isPairedMain;					///< is the main component of 2 paired channels.
	bool isPairedSub;					///< is the sub component of 2 paired channels.
	QPointer<DmxChannel> pairedDmxChannel;	///< this is the DmxChannel instance, that is combined with this channel

protected:
	// These Vars are mainly for animation/fading control in the scenes
	CtrlCmd curCmd[MIX_LINES];						///< This is the command currently running on this channel
	float fadeTargetValue[MIX_LINES];
	float fadeStartValue[MIX_LINES];
	float fadeValue[MIX_LINES];
	float fadeStep[MIX_LINES];

public:
	DmxChannel();
	DmxChannel(const DmxChannel &o);
	~DmxChannel();

	bool initFadeCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 target_value=0 );
	bool initFadeScannerCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 curScanValue=0);
	bool loopFunction(int mixline);
	DmxChannelType dmxChannelType() const;
	DmxChannelType localDmxType() const;

	DmxChannelType globalDmxType() const;
	int globalDeviceID() const;
	QString globalDeviceShortId() const;

	int dmxTargetValue() const;
	int scaledTargetValue() const;

	bool setPairedWith(DmxChannel *other);
	bool clrPairedWith();

private:
	void init();
};

#endif // DMXCHANNEL_H

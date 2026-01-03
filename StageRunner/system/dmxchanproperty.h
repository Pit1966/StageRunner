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

#ifndef DMXCHANPROPERTY_H
#define DMXCHANPROPERTY_H

#include "varset.h"
#include "dmx/dmxtypes.h"

class DmxChanProperty : public VarSet
{
public:
	qint32 dmxUniverse;			///< This is the universe this dmx channel lives in (This will be mapped to the output universe later by the plugin configuration)
	qint32 dmxChannel;			///< This is the native DMX channel
	qint32 dmxType;				///< @see DmxChannelType
	qint32 minValue;			///< this is a value between 0 - 255: default 0
	qint32 maxValue;			///< this is a value between 0 - 255: default 255
	QString labelShort;
	QString labelLong;

public:
	DmxChanProperty();

private:
	void init();
};

#endif // DMXCHANPROPERTY_H

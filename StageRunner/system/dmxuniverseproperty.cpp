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

#include "dmxuniverseproperty.h"

DmxUniverseProperty::DmxUniverseProperty()
	: dmxChanLayout(this)
{
	init();
}

void DmxUniverseProperty::initDmxChannels(int universe, int count)
{
	while (!dmxChanLayout.isEmpty())
		delete dmxChanLayout.takeFirst();

	for (int t=0; t<count; t++) {
		DmxChanProperty *prop = new DmxChanProperty();
		prop->dmxUniverse = universe;
		prop->dmxChannel = t;
		dmxChanLayout.append(prop);
	}

	universeLabel = QString("Universe #%1").arg(universe+1);
	dmxUniverse = universe;
	dmxChannelCount = count;
}

void DmxUniverseProperty::init()
{
	setClass(PrefVarCore::DMX_CHANNEL,"DmxUniverseProperty");
	setDescription("A complete configuration of 512 DMX Channels for a DMX universe");

	addExistingVar(universeLabel,"UniverseLabel");
	addExistingVar(dmxUniverse,"UniverseNumber");
	addExistingVar(dmxChannelCount,"UniverseDmxChannelSize",0,512,512);
	addExistingVarSetList(dmxChanLayout,"UniverseLayout",PrefVarCore::DMX_CHANNEL_PROPERTY);

}

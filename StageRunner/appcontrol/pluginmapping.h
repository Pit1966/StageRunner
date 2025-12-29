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

#ifndef PLUGINMAPPING_H
#define PLUGINMAPPING_H

#include "varset.h"
#include "varsetlist.h"

class QLCIOPlugin;

class PluginConfig : public VarSet
{
public:
	pstring pPluginName;
	pstring pLineName;						///< This is the string returned from plugin->inputs() or outputs()
	pbool pIsUsed;
	pint32 pUniverse;
	pint32 pResponseTime;					///< This is the fade delay time for incoming DMX Data when it is forwarded to a scene
	pstring pParameters;					///< Plugin Parameters for this Line

	QLCIOPlugin *plugin;
	int deviceNumber;
	int deviceIoType;

public:
	PluginConfig();
	~PluginConfig();

private:
	void init();
};


class PluginMapping : public VarSet
{
public:
	PluginMapping();
	~PluginMapping();

	pstring pluginConfigName;
	PluginConfig *defaultLineConfig;
	VarSetList<PluginConfig*>pluginLineConfigs;

	bool saveToDefaultFile();
	bool loadFromDefaultFile();

	PluginConfig * getCreatePluginLineConfig(const QString &plugin_name, const QString &line_name);

private:
	void init();
};

#endif // PLUGINMAPPING_H

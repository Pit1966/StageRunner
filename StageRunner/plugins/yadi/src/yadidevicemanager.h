/***********************************************************************************
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
************************************************************************************/

#ifndef YADIDEVICEMANAGER_H
#define YADIDEVICEMANAGER_H

#include <QList>

class YadiDevice;
class YadiDMXUSBOut;

class YadiDeviceManager
{
private:
	static QList<YadiDevice*>globYadiDeviceList;

public:
	YadiDeviceManager();

	static YadiDevice * deviceAt(int idx);
	inline static int devices() {return globYadiDeviceList.size();}
	static int enumerateYadiDevices(YadiDMXUSBOut *yadiPlugin, bool update = false);
	static void clearYadiDevices();
	static void initYadiDevices();
	static YadiDevice * getDevice(const QString &dev_node, int cap);
	static YadiDevice * getDeviceByDevNodePath(const QString &devpath, int cap);
	static void updateYadiDevicesStatus(YadiDMXUSBOut *yadiPlugin);
	static QList<YadiDevice*> & globalDeviceList() {return globYadiDeviceList;}
	static bool yadiDeviceLessThan(const YadiDevice * s1, const YadiDevice * s2);
	static bool deviceNodeExists(const QString &devNode);
	static bool removeDevice(const QString &devNode);
};


#endif // YADIDEVICEMANAGER_H

/***********************************************************************************
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
************************************************************************************/

#ifndef YADICONFIGDIALOG_H
#define YADICONFIGDIALOG_H

#include "ui_yadiconfigdialog.h"

class YadiDMXUSBOut;
class YadiDevice;
class DmxMonitor;

class YadiConfigDialog : public QDialog, private Ui::YadiConfigDialog
{
	Q_OBJECT
private:
	YadiDMXUSBOut * plugin;				///< Pointer to parent plugin object
	YadiDevice * c_yadi;				///< Pointer to YadiDevice if one is selected in GUI and existing

public:
	explicit YadiConfigDialog(YadiDMXUSBOut *p_plugin = 0);
	~YadiConfigDialog();
	bool configChanged;					///< becomes true, if configuration changed so it has to be reported to QLC

private slots:
	void on_rescanDevButton_clicked();
	void show_current_device_list();
	void on_selectDeviceCombo_currentIndexChanged(int index);
	void on_transMergeModeCombo_activated(int index);
	void on_transLimitOutChannelsSpin_valueChanged(int arg1);
	void on_transLimitInChannelsSpin_valueChanged(int arg1);
	void on_rxLimitInChannelsSpin_valueChanged(int arg1);
	void on_showDmxInButton_clicked();
	void on_showDmxOutButton_clicked();
	void on_debugSpin_valueChanged(int arg1);
};

#endif // YADICONFIGDIALOG_H

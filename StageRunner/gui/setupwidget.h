//=======================================================================
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
//=======================================================================

#ifndef SETUPWIDGET_H
#define SETUPWIDGET_H

#include "ui_setupwidget.h"

class AppCentral;
class QLCIOPlugin;
class PluginMapping;
class UserSettings;

class SetupWidget : public QDialog, public Ui::SetupWidget
{
	Q_OBJECT
private:
	AppCentral * myapp;
	QLCIOPlugin * m_selectedPlugin;
	PluginMapping *m_curPluginMap;

	bool m_updatePluginMapFlag;					///< Flag that decides if PluginMapping should be adjusted on Setup Dialog close
	bool m_restartAudioSlotsOnExit;

public:
	SetupWidget(AppCentral *app_central, QWidget *parent = 0);

private:
	void init();
	void initGui();
	void copy_settings_to_gui();
	void copy_gui_to_settings();
	void update_dmx_mapping_table(QLCIOPlugin *plugin);
	void update_gui_plugin_info();

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_qlcPluginsList_itemActivated(QListWidgetItem *item);
	void on_configurePluginButton_clicked();
	void on_saveDmxConfigButton_clicked();

	void onPluginLineUniverseChanged(int val);
	void onPluginLineResponseTimeChanged(int val);
	void on_appStyleCombo_currentIndexChanged(int idx);
	void on_dialKnobStyleCombo_currentIndexChanged(int idx);

	void on_addDmxUniverseToTemplateButton_clicked();
	void on_restartAudioVideoSlots_clicked();

	void on_updateLinesButton_clicked();
	void on_qlcPluginsList_itemChanged(QListWidgetItem *item);

signals:
	void applicationStyleChanged(QString style);
	void dialKnobStyleChanged(QString style);
	void setupChanged(UserSettings *);
};

#endif // SETUPWIDGET_H

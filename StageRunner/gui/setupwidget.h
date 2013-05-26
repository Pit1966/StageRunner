#ifndef SETUPWIDGET_H
#define SETUPWIDGET_H

#include "ui_setupwidget.h"

class AppCentral;
class QLCIOPlugin;
class PluginMapping;

class SetupWidget : public QDialog, public Ui::SetupWidget
{
	Q_OBJECT
private:
	AppCentral * myapp;
	QLCIOPlugin * cur_selected_qlc_plugin;
	PluginMapping *cur_plugin_map;

public:
	SetupWidget(AppCentral *app_central, QWidget *parent = 0);

private:
	void init();
	void copy_settings_to_gui();
	void copy_gui_to_settings();
	void update_dmx_mapping_table(QLCIOPlugin *plugin);

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_qlcPluginsList_itemActivated(QListWidgetItem *item);
	void on_configurePluginButton_clicked();
	void on_saveDmxConfigButton_clicked();

	void if_pluginline_universe_changed(int val);
};

#endif // SETUPWIDGET_H

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
	void on_appStyleCombo_currentIndexChanged(const QString &arg1);
	void on_dialKnobStyleCombo_currentIndexChanged(const QString &arg1);

	void on_addDmxUniverseToTemplateButton_clicked();
	void on_restartAudioVideoSlots_clicked();

	void on_updateLinesButton_clicked();

signals:
	void applicationStyleChanged(QString style);
	void dialKnobStyleChanged(QString style);
	void setupChanged(UserSettings *);
};

#endif // SETUPWIDGET_H

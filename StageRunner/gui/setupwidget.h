#ifndef SETUPWIDGET_H
#define SETUPWIDGET_H

#include "ui_setupwidget.h"

class AppCentral;
class QLCIOPlugin;

class SetupWidget : public QDialog, public Ui::SetupWidget
{
	Q_OBJECT
private:
	AppCentral * myapp;
	QLCIOPlugin * cur_selected_qlc_plugin;

public:
	SetupWidget(AppCentral *app_central, QWidget *parent = 0);

private:
	void init();
	void copy_settings_to_gui();
	void copy_gui_to_settings();

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_qlcPluginsList_itemActivated(QListWidgetItem *item);
	void on_configurePluginButton_clicked();
};

#endif // SETUPWIDGET_H

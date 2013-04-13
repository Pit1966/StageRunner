#ifndef YADICONFIGDIALOG_H
#define YADICONFIGDIALOG_H

#include "ui_yadiconfigdialog.h"

class YadiDMXUSBOut;
class YadiDevice;

class YadiConfigDialog : public QDialog, private Ui::YadiConfigDialog
{
	Q_OBJECT
private:
	YadiDMXUSBOut * plugin;				///< Pointer to parent plugin object
	YadiDevice * c_yadi;				///< Pointer to YadiDevice if one is selected in GUI and existing

public:
	explicit YadiConfigDialog(YadiDMXUSBOut *p_plugin = 0);
	bool configChanged;					///< becomes true, if configuration changed so it has to be reported to QLC

private slots:
	void on_rescanDevButton_clicked();
	void show_current_device_list();
	void on_selectDeviceCombo_currentIndexChanged(int index);
	void on_transMergeModeCombo_activated(int index);
	void on_transMaxOutChannelsSpin_valueChanged(int arg1);
	void on_transMaxInChannelsSpin_valueChanged(int arg1);
	void on_rxMaxInChannelsSpin_valueChanged(int arg1);
};

#endif // YADICONFIGDIALOG_H

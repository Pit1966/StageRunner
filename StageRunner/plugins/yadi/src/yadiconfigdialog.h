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

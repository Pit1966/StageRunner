#include "yadiconfigdialog.h"

#include "yadidevice.h"
#include "yadidmxusbout.h"
#include "yadidevicemanager.h"
#include "serialwrapper.h"
#include "dmxmonitor.h"

#include <QDebug>
#include <QMessageBox>

YadiConfigDialog::YadiConfigDialog(YadiDMXUSBOut *p_plugin)
	: QDialog()
{
	plugin = p_plugin;
	configChanged = false;
	c_yadi = 0;

	setupUi(this);
	transceiverGroup->hide();
	receiverGroup->hide();
	show_current_device_list();
}

YadiConfigDialog::~YadiConfigDialog()
{
}

void YadiConfigDialog::on_rescanDevButton_clicked()
{
	textEdit->clear();

	configChanged = plugin->findDevices(true);
	show_current_device_list();

	if (configChanged) {
		int ret = QMessageBox::question(this
							  ,tr("Yadi DMX config")
							  ,tr("The device configuration has changed!\n\n"
								  "Do you want to load last settings for found devices?")
							  ,QMessageBox::Yes | QMessageBox::No);

		if (ret == QMessageBox::Yes) {
			// Load Settings
			for (int t=0; t<YadiDeviceManager::devices(); t++) {
				YadiDeviceManager::deviceAt(t)->loadConfig();
			}
			show_current_device_list();
		}
		emit plugin->configurationChanged();
	}

}

void YadiConfigDialog::show_current_device_list()
{
	selectDeviceCombo->clear();
	QStringList combo_elements;

	QString txt;

	txt += QString("<H3>%1</H3>").arg(tr("Detected devices:"));
	txt += "<P>";
	for (int t=0; t<YadiDeviceManager::devices(); t++) {
		YadiDevice *yadi = YadiDeviceManager::deviceAt(t);
		QString dev_str = yadi->deviceProductName;
		dev_str += tr(" (S/N:%1)").arg(yadi->deviceSerial);
		dev_str += tr(": %1").arg(yadi->devNodePath);

		txt += dev_str;
		txt += yadi->deviceNodePresent ? tr(" (<font color=darkgreen>online</font>)") : tr(" (<font color=red>offline</font>)");
		txt += "<br>";

		combo_elements << dev_str;
	}
	txt += "</P>";

	textEdit->setHtml(txt);

	selectDeviceCombo->addItems(combo_elements);
}

void YadiConfigDialog::on_selectDeviceCombo_currentIndexChanged(int index)
{
	transceiverGroup->hide();
	receiverGroup->hide();

	if (index < 0 || index >= YadiDeviceManager::devices()) {
		c_yadi = 0;
		return;
	}

	c_yadi = YadiDeviceManager::deviceAt(index);
	if ( (c_yadi->capabilities & (YadiDevice::FL_INPUT_UNIVERSE | YadiDevice::FL_OUTPUT_UNIVERSE))
			== (YadiDevice::FL_INPUT_UNIVERSE | YadiDevice::FL_OUTPUT_UNIVERSE)) {
		transceiverGroup->show();
		showDmxOutButton->show();
		transMergeModeCombo->setCurrentIndex(c_yadi->universeMergeMode);
		transMaxInChannelsSpin->setValue(c_yadi->usedDmxInChannels);
		transMaxOutChannelsSpin->setValue(c_yadi->usedDmxOutChannels);
	}
	else if ( c_yadi->capabilities & YadiDevice::FL_INPUT_UNIVERSE ) {
		receiverGroup->show();
		showDmxOutButton->hide();
		rxMaxInChannelsSpin->setValue(c_yadi->usedDmxInChannels);
	}



}

void YadiConfigDialog::on_transMergeModeCombo_activated(int index)
{
	if (!c_yadi) return;

	if (index != c_yadi->universeMergeMode) {
		c_yadi->universeMergeMode = index;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("m %1").arg(index);
			qDebug() << "YadiConfigDialog::on_transMergeModeCombo_activated: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_transMaxOutChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxOutChannels) {
		c_yadi->usedDmxOutChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("ho %1").arg(arg1);
			qDebug() << "YadiConfigDialog::on_transMaxOutChannelsSpin_valueChanged: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_transMaxInChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxInChannels) {
		c_yadi->usedDmxInChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("hi %1").arg(arg1);
			qDebug() << "YadiConfigDialog::on_transMaxInChannelsSpin_valueChanged: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_rxMaxInChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxInChannels) {
		c_yadi->usedDmxInChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("hi %1").arg(arg1);
			qDebug() << "YadiConfigDialog::on_rxMaxInChannelsSpin_valueChanged: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_showDmxInButton_clicked()
{
	if (!c_yadi) return;

	DmxMonitor *mon = c_yadi->dmxInMonWidget;
	if (!mon) {
		mon = c_yadi->openDmxInMonitorWidget();
		connect(mon,SIGNAL(monitorClosed(DmxMonitor*)),plugin,SLOT(closeMonitorByInstancePointer(DmxMonitor*)));
	}
	mon->show();
	mon->raise();
}

void YadiConfigDialog::on_showDmxOutButton_clicked()
{
	if (!c_yadi) return;

	DmxMonitor *mon = c_yadi->dmxOutMonWidget;
	if (!mon) {
		mon = c_yadi->openDmxOutMonitorWidget();
		connect(mon,SIGNAL(monitorClosed(DmxMonitor*)),plugin,SLOT(closeMonitorByInstancePointer(DmxMonitor*)));
	}
	mon->show();
	mon->raise();
}

void YadiConfigDialog::on_debugSpin_valueChanged(int arg1)
{
	plugin->debug = arg1;

	if (!c_yadi) return;
	c_yadi->debug = arg1;

}

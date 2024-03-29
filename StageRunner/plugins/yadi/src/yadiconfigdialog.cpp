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
		dev_str += tr(": %1").arg(yadi->devNode());

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
		transLimitInChannelsSpin->setValue(c_yadi->usedDmxInChannels);
		transLimitOutChannelsSpin->setValue(c_yadi->usedDmxOutChannels);
		transMaxInLabel->setText(QString::number(c_yadi->maxDeviceDmxInChannels));
		transMaxOutLabel->setText(QString::number(c_yadi->maxDeviceDmxOutChannels));
	}
	else if ( c_yadi->capabilities & YadiDevice::FL_INPUT_UNIVERSE ) {
		receiverGroup->show();
		showDmxOutButton->hide();
		rxLimitInChannelsSpin->setValue(c_yadi->usedDmxInChannels);
		rxMaxChannelsLabel->setText(QString::number(c_yadi->maxDeviceDmxInChannels));
	}
	debugSpin->setValue(c_yadi->debug);

}

void YadiConfigDialog::on_transMergeModeCombo_activated(int index)
{
	if (!c_yadi) return;

	if (index != c_yadi->universeMergeMode) {
		c_yadi->universeMergeMode = index;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("m %1\n").arg(index);
			qDebug() << "YadiConfigDialog::on_transMergeModeCombo_activated: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_transLimitOutChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxOutChannels) {
		c_yadi->usedDmxOutChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("ho %1\n").arg(arg1);
			qDebug() << "YadiConfigDialog::on_transMaxOutChannelsSpin_valueChanged: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_transLimitInChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxInChannels) {
		c_yadi->usedDmxInChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("hi %1\n").arg(arg1);
			qDebug() << "YadiConfigDialog::on_transMaxInChannelsSpin_valueChanged: send" << cmd;
			c_yadi->write(cmd.toLocal8Bit().data());
		}
	}
}

void YadiConfigDialog::on_rxLimitInChannelsSpin_valueChanged(int arg1)
{
	if (!c_yadi) return;

	if (arg1 != c_yadi->usedDmxInChannels) {
		c_yadi->usedDmxInChannels = arg1;
		c_yadi->settingsChanged = true;

		if (c_yadi->openOutput()) {
			QString cmd = QString("hi %1\n").arg(arg1);
			qDebug() << "YadiConfigDialog::on_rxLimitInChannelsSpin_valueChanged: send" << cmd;
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
	c_yadi->settingsChanged = true;

}

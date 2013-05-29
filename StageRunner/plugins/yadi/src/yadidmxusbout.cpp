#include "yadidmxusbout.h"
#include "serialwrapper.h"
#include "yadidevice.h"
#include "yadidevicemanager.h"
#include "yadiconfigdialog.h"
#include "yadireceiver.h"
#include "dmxmonitor.h"

#include <QDebug>
#include <QTime>

YadiDMXUSBOut::YadiDMXUSBOut()
{
	write_universe_debug_out = false;
}


YadiDMXUSBOut::~YadiDMXUSBOut()
{
	// This removes the Device objects which
	// are hold static in memory
	YadiDeviceManager::clearYadiDevices();

	if (debug) qDebug("~YadiDMXUSBOut");
}

void YadiDMXUSBOut::init()
{
	debug = 0;

	write_universe_debug_out = true;
	findDevices();

	// Load Settings
	for (int t=0; t<YadiDeviceManager::devices(); t++) {
		YadiDeviceManager::deviceAt(t)->loadConfig();
	}
	if (debug) qDebug("YadiDMXUSBOut::init");
}

void YadiDMXUSBOut::findDevices()
{
	// This is a list of all available devices
	YadiDeviceManager udev;
	QList<YadiDevice*>dev_list = udev.enumerateYadiDevices();

	// Update list with output devices
	// First check existing nodes and see if the device has disappeared
	for (int t = output_devices.size()-1; t>=0; t--) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(output_devices.at(t),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (!yadi || !yadi->checkDeviceNode()) {
			output_devices.removeAt(t);
		}
	}

	// Add device to output list if it is not already in
	foreach(YadiDevice *device, dev_list) {
		if (!output_devices.contains(device->devNodePath)) {
			if (device->capabilities&YadiDevice::FL_OUTPUT_UNIVERSE) {
				output_devices.append(device->devNodePath);
				if (debug) qDebug() << "YadiDMXUSBOut::Added Output Device: " << device->deviceProductName << device->devNodePath;
			}
		}
	}


	// Update list with input devices
	// First check existing nodes and see if the device has disappeared
	for (int t = input_devices.size()-1; t>=0; t--) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(input_devices.at(t),YadiDevice::FL_INPUT_UNIVERSE);
		if (!yadi || !yadi->checkDeviceNode()) {
			input_devices.removeAt(t);
		}
	}

	// Add device to input list if it is not already in
	foreach(YadiDevice *device, dev_list) {
		if (!input_devices.contains(device->devNodePath)) {
			if (device->capabilities&YadiDevice::FL_INPUT_UNIVERSE) {
				input_devices.append(device->devNodePath);
				if (debug) qDebug() << "YadiDMXUSBOut::Added input Device: " << device->deviceProductName << device->devNodePath;
			}
		}
	}
}

QString YadiDMXUSBOut::name()
{
	return QString("YADI DMX USB Device");
}

void YadiDMXUSBOut::openOutput(quint32 output)
{
	qDebug("YadiDMXUSBOut::openOutput(%d)",output);

	if ((int)output < output_devices.size()) {
		bool ok;
		YadiDevice *yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (yadi) {
			yadi->outputId = output;
			ok = yadi->openOutput();
		} else {
			ok = false;
		}
		if (ok) {
			if (debug) qDebug("YadiDMXUSBOut::openOutput(%d) successful",output);
		} else {
			qDebug("YadiDMXUSBOut::openOutput(%d) failed!",output);
		}

	} else {
		qDebug("YadiDMXUSBOut::openOutput(%d) failed! Device list own %d outputs",output+1,output_devices.size());
	}
}

void YadiDMXUSBOut::closeOutput(quint32 output)
{
	if (debug) qDebug("YadiDMXUSBOut::closeOutput(%d)",output);

	if ((int)output < output_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (yadi) {
			yadi->closeOutput();
		} else {
			qDebug("YadiDMXUSBOut::closeOutput(%d) Device not found",output);
		}
	}
}

int YadiDMXUSBOut::capabilities() const
{
	return QLCIOPlugin::Output | QLCIOPlugin::Input | QLCIOPlugin::Monitor;
}

QStringList YadiDMXUSBOut::outputs()
{
	QStringList outnames;
	for (int t=0; t<output_devices.size(); t++) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(t),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (!yadi)
			continue;
		QString name = "TX:";
		name += yadi->devNodePath;
		name += ":";
		name += yadi->deviceProductName;
		name += "(" + yadi->deviceSerial + ")";
		if (!yadi->deviceNodePresent) {
			name += " !!!";
		}
		outnames.append(name);
	}
	return outnames;
}

void YadiDMXUSBOut::writeUniverse(quint32 output, const QByteArray &universe)
{
	if ((int)output >= output_devices.size()) {
		if (write_universe_debug_out) {
			write_universe_debug_out = false;
			qWarning("Yadi DMX plugin::writeUniverse: Non existing device adressed: output line: %d",output);
		}
		return;
	}
	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);

	if (!yadi || !yadi->isOutputOpen()) {
		if (write_universe_debug_out) {
			write_universe_debug_out = false;
			qWarning("Yadi DMX plugin:writeUniverse: Device is closed: output line: %d",output);
		}
		return;
	}
	else {
		write_universe_debug_out = true;
	}

	QTime stop;
	stop.restart();
	// qDebug() << QTime::currentTime().msec();

	// Check, what the fastest output variant is.
	int hi_changed_channel = 0;
	int changed_channels = 0;
	if (yadi->outputSendAllData) {
		yadi->outputSendAllData = false;
		changed_channels = universe.size();
		hi_changed_channel = universe.size();
	} else {
		int max_out = universe.size();
		if (max_out > yadi->usedDmxOutChannels) {
			max_out = yadi->usedDmxOutChannels;
		}
		for (int t=0; t<max_out;t++) {
			if (universe.at(t) != yadi->outUniverse.at(t)) {
				changed_channels++;
				hi_changed_channel = t+1;
			}
		}
	}

	if (changed_channels > 8 || hi_changed_channel < 50) {
//		for (int t=0; t<12;t++) {
//			if (universe.at(t) != yadi->outUniverse.at(t)) {
//				qDebug("chan %d: %d->%d",t,quint8(yadi->outUniverse.at(t)),quint8(universe.at(t)));
//			}
//		}
		unsigned char out[] = {'O',0,2};		// 512 Channels
		out[1] = hi_changed_channel;
		out[2] = hi_changed_channel>>8;
		int bytes = yadi->write((char*)out,3);
		bytes += yadi->write(universe.data(),hi_changed_channel);
		if (bytes != hi_changed_channel + 3) {
			handle_output_error(output);
		} else {
			yadi->outUniverse = universe;
		}
		if (debug > 2) qDebug("out burst %dms hi_changed:%d(%d) %d %d",stop.elapsed(),hi_changed_channel,universe.size(),out[1],out[2]);
	} else {
		for (int t=0; t<hi_changed_channel; t++) {
			if (universe.at(t) != yadi->outUniverse.at(t) || yadi->outputSendAllData) {
				yadi->outUniverse[t] = universe.at(t);
				QString dat = "o";
				dat += QString::number(t+1);
				dat += " ";
				dat += QString::number((quint8)universe.at(t));
				dat += "\n";

				if (debug > 3) qDebug() << t << dat;
				int bytes = yadi->write(dat.toLocal8Bit().data());
				if (bytes != dat.size()) {
					handle_output_error(output);
					break;
				}
			}
		}
		if (debug > 2) qDebug("out single %dms",stop.elapsed());
	}
	update_output_monitor(output,universe);
}

QString YadiDMXUSBOut::outputInfo(quint32 output)
{
	QString str;

	str += QString("<HTML>");
	str += QString("<HEAD>");
	str += QString("<TITLE>%1</TITLE>").arg(name());
	str += QString("</HEAD>");
	str += QString("<BODY>");

	if (output == QLCIOPlugin::invalidLine()) {
		str += QString("<H3>%1</H3>").arg(name());
		if (output_devices.size() == 0)
		{
			str += QString("<B>%1</B>").arg(tr("No output devices available."));
			str += QString("<P>");
			str += tr("Make sure that you have your hardware firmly plugged in. ");
			str += QString("</P>");
		}

		str += QString("<P>");
		str += tr("This plugin provides DMX output support for");
		str += QString(" Stonechip Entertainment - YADI DMX ");
		str += QString("</P>");
	}
	else if ((int)output < output_devices.size()) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
		str += QString("<H3>%1</H3>").arg(outputs()[output]);
		str += QString("<P>");
		if (yadi) {
			if (yadi->deviceNodePresent) {
				str += tr("Device is online.<br>");
			} else {
				str += tr("<font color=darkred>Device is offline.</font><br>");
			}
			str += tr("Serial number: %1<br>").arg(yadi->deviceSerial);
			str += tr("Max DMX Output Channels: %1<br>").arg(yadi->maxDeviceDmxOutChannels);
			str += tr("Currently used DMX Channels: %1<br>").arg(yadi->usedDmxOutChannels);
		}
		str += QString("</P>");
	}

	str += QString("</BODY>");
	str += QString("</HTML>");

	return str;

}

void YadiDMXUSBOut::openInput(quint32 input)
{
	if (debug) qDebug("YadiDMXUSBOut::openInput(%d)",input);

	if ((int)input < input_devices.size()) {
		bool ok;
		YadiDevice *yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
		if (yadi) {
			yadi->inputId = input;
			ok = yadi->openInput();
		} else {
			ok = false;
		}
		if (ok) {
			connect(yadi->inputThread(),SIGNAL(dmxInDeviceChannelChanged(quint32,quint32,uchar))
					,this,SLOT(propagateChangedInput(quint32,quint32,uchar)));
			connect(yadi->inputThread(),SIGNAL(exitReceiverWithFailure()),this,SIGNAL(configurationChanged()));
		} else {
			qDebug("YadiDMXUSBOut::openInput(%d) failed!",input);
		}
	}
}

void YadiDMXUSBOut::closeInput(quint32 input)
{
	if (debug) qDebug("YadiDMXUSBOut::closeInput(%d)",input);

	if ((int)input < input_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
		if (yadi) {
			disconnect(yadi->inputThread(),0,this,0);
			yadi->closeInput();
		} else {
			qDebug("YadiDMXUSBOut::closeInput(%d) Device not found",input);
		}
	}
}

QStringList YadiDMXUSBOut::inputs()
{
	QStringList innames;
	for (int t=0; t<input_devices.size(); t++) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(input_devices.at(t),YadiDevice::FL_INPUT_UNIVERSE);
		if (!yadi)
			continue;
		QString name = "RX:";
		name += yadi->devNodePath;
		name += ":";
		name += yadi->deviceProductName;
		name += "(" + yadi->deviceSerial +")";
		if (!yadi->deviceNodePresent) {
			name += " !!!";
		}
		innames.append(name);
	}
	return innames;
}

QString YadiDMXUSBOut::inputInfo(quint32 input)
{
	QString str;

	str += QString("<HTML>");
	str += QString("<HEAD>");
	str += QString("<TITLE>%1</TITLE>").arg(name());
	str += QString("</HEAD>");
	str += QString("<BODY>");

	if (input == QLCIOPlugin::invalidLine()) {
		str += QString("<H3>%1</H3>").arg(name());
		if (input_devices.size() == 0)
		{
			str += QString("<B>%1</B>").arg(tr("No input devices available."));
			str += QString("<P>");
			str += tr("Make sure that you have your hardware firmly plugged in. ");
			str += QString("</P>");
		}

		str += QString("<P>");
		str += tr("This plugin provides DMX input support for");
		str += QString(" Stonechip Entertainment - YADI DMX ");
		str += QString("</P>");
	}
	else if ((int)input < input_devices.size()) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
		str += QString("<H3>%1</H3>").arg(inputs()[input]);
		str += QString("<P>");
		if (yadi) {
			if (yadi->deviceNodePresent) {
				str += tr("Device is online.<br>");
			} else {
				str += tr("<font color=darkred>Device is offline.</font><br>");
			}
			str += tr("Serial number: %1<br>").arg(yadi->deviceSerial);
			str += tr("Max DMX Input Channels: %1<br>").arg(yadi->maxDeviceDmxInChannels);
			str += tr("Currently used DMX Channels: %1<br>").arg(yadi->usedDmxInChannels);
		}
		str += QString("</P>");
	}

	str += QString("</BODY>");
	str += QString("</HTML>");

	return str;

}

bool YadiDMXUSBOut::canConfigure()
{
	return true;
}

void YadiDMXUSBOut::configure()
{
	YadiDeviceManager::updateYadiDevicesStatus();

	YadiConfigDialog dialog(this);
	dialog.exec();

	if (dialog.configChanged)
		emit configurationChanged();

	for (int t=0; t<YadiDeviceManager::devices(); t++) {
		if (YadiDeviceManager::deviceAt(t)->settingsChanged) {
			YadiDeviceManager::deviceAt(t)->saveConfig();
		}
	}
}

DmxMonitor *YadiDMXUSBOut::openOutputMonitor(quint32 output)
{
	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (yadi) {
		return yadi->openDmxOutMonitorWidget();
	} else {
		return 0;
	}
}

DmxMonitor *YadiDMXUSBOut::openInputMonitor(quint32 input)
{
	YadiDevice * yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
	if (yadi) {
		return yadi->openDmxInMonitorWidget();
	} else {
		return 0;
	}
}

void YadiDMXUSBOut::handle_output_error(quint32 output)
{
	qWarning("Yadi DMX plugin: Communication error occured");

	bool close_device = false;

	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (yadi) {
		yadi->deviceNodePresent = false;
		close_device = true;
	}

	if (close_device && yadi) {
		yadi->closeOutput();
		yadi->closeInput();
		qWarning("YadiDMXUSBOut::handle_output_error: Device '%s' closed",output_devices.at(output).toLocal8Bit().data());
		emit configurationChanged();
	}
}

void YadiDMXUSBOut::update_output_monitor(quint32 output, const QByteArray &universe)
{

	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (!yadi) return;
	DmxMonitor *mon = yadi->dmxOutMonWidget;

	if (mon) {
		for (int t=0; t<mon->visibleBars(); t++) {
			mon->setValueInBar(t,uchar(universe[t]));
		}
	}
}

void YadiDMXUSBOut::closeMonitorByInstancePointer(DmxMonitor *instance)
{
	if (!instance) return;


	foreach(QString devname, input_devices) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(devname,YadiDevice::FL_INPUT_UNIVERSE);
		if(yadi->dmxInMonWidget == instance) {
			yadi->closeDmxInMonitorWidget();
			if (debug) qDebug("Delete DMX In Monitor");
		}
	}

	foreach(QString devname, output_devices) {
		YadiDevice * yadi = YadiDeviceManager::getDevice(devname,YadiDevice::FL_OUTPUT_UNIVERSE);
		if(yadi->dmxOutMonWidget == instance) {
			yadi->closeDmxInMonitorWidget();
			if (debug) qDebug("Delete DMX Out Monitor");
		}
	}
}

void YadiDMXUSBOut::propagateChangedInput(quint32 input, quint32 channel, uchar value)
{
	emit valueChanged(input,channel,value);

	if (debug > 1) qDebug("YadiDMXUSBOut::propagateChangedInput %d %d %d", input, channel, value);
}

/****************************************************************************
 * Plugin export
 ****************************************************************************/
#ifndef IS_QT5
Q_EXPORT_PLUGIN2(yadidmxusbout, YadiDMXUSBOut)
#endif

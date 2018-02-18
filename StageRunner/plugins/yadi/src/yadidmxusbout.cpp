#include "yadidmxusbout.h"
#include "serialwrapper.h"
#include "yadidevice.h"
#include "yadidevicemanager.h"
#include "yadiconfigdialog.h"
#include "yadireceiver.h"
#include "dmxmonitor.h"

#include <QDebug>
#include <QTime>
#include <QMutexLocker>
#include <QTimer>


#ifdef unix
#include <errno.h>
#include <string.h>
#endif

YadiDMXUSBOut::YadiDMXUSBOut()
	: accessMutex(new QMutex(QMutex::Recursive))
	, write_universe_debug_out(false)
	, m_reOpenOutput(-1)
	, m_reOpenInput(-1)
	, m_comErrorCounter(0)
	, m_totalComErrorCounter(0)
{
	connect (this,SIGNAL(communicationErrorDetected()),this,SLOT(handleCommunicationError()),Qt::QueuedConnection);
}


YadiDMXUSBOut::~YadiDMXUSBOut()
{
	// This removes the Device objects which
	// are hold static in memory
//#pragma message("Add clearYadiDevices() again !!")
    YadiDeviceManager::clearYadiDevices();

	delete accessMutex;
	if (debug) qDebug("~YadiDMXUSBOut");
}

void YadiDMXUSBOut::init()
{
	qDebug("%s",Q_FUNC_INFO);
	debug = 0;
	write_universe_debug_out = true;

	accessMutex->lock();

	for (int t=0; t<MAX_DEVS; t++) {
		inDevNameTable[t].clear();
		outDevNameTable[t].clear();
	}
	output_devices.clear();
	input_devices.clear();

	YadiDeviceManager::clearYadiDevices();
	findDevices();

	// Load Settings
	for (int t=0; t<YadiDeviceManager::devices(); t++) {
		YadiDeviceManager::deviceAt(t)->loadConfig();
	}

	YadiDeviceManager::initYadiDevices();

	accessMutex->unlock();

	emit configurationChanged();
}

bool YadiDMXUSBOut::findDevices(bool update)
{
	QMutexLocker lock(accessMutex);

	QStringList cur_out(output_devices);
	QStringList cur_in(input_devices);

	// This updates the list of all available devices
	YadiDeviceManager::enumerateYadiDevices(update);

	// Update list with output devices
	// First check existing nodes and see if the device has disappeared
	for (int t = output_devices.size()-1; t>=0; t--) {
		QString devNode = output_devices.at(t);
		YadiDevice *yadi = YadiDeviceManager::getDevice(devNode,YadiDevice::FL_OUTPUT_UNIVERSE);
		if (!yadi) {
			qDebug("Yadi: Output device %s is gone. No Yadi output found"
							  ,devNode.toLocal8Bit().data());
			output_devices.removeAt(t);
		}
		else if (!YadiDeviceManager::deviceNodeExists(devNode)) {
			qDebug("Yadi: Output device %s is gone",devNode.toLocal8Bit().data());
			yadi->closeInOut();
			output_devices.removeAt(t);
			YadiDeviceManager::removeDevice(devNode);
		}
	}

	// Add device to output list if it is not already in
	for (int t=0; t<YadiDeviceManager::globalDeviceList().size(); t++) {
		YadiDevice *device = YadiDeviceManager::globalDeviceList().at(t);
		if (!output_devices.contains(device->devNodePath)) {
			if (device->capabilities&YadiDevice::FL_OUTPUT_UNIVERSE) {
				qDebug() << "YadiDMXUSBOut::Added Output Device: ";
				qDebug() << "         "  << device->deviceProductName << device->devNodePath;
				output_devices.append(device->devNodePath);
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
	foreach(YadiDevice *device, YadiDeviceManager::globalDeviceList()) {
		if (!input_devices.contains(device->devNodePath)) {
			if (device->capabilities&YadiDevice::FL_INPUT_UNIVERSE) {
				input_devices.append(device->devNodePath);
				qDebug() << "YadiDMXUSBOut::Added input Device: " << device->deviceProductName << device->devNodePath;
			}
		}
	}

	if (cur_in != input_devices || cur_out != output_devices) {
		qDebug("Yadi: Find devices: Device list changed");
		return true;
	} else {
		return false;
	}
}

QString YadiDMXUSBOut::name()
{
	return QString("YADI DMX USB Device");
}

bool YadiDMXUSBOut::openOutput(quint32 output, quint32 universe)
{
	qDebug("YadiDMXUSBOut::openOutput(%d), universe %d",output,universe);
	return internOpenOutput(output,universe);
}

void YadiDMXUSBOut::closeOutput(quint32 output, quint32 universe)
{
	if (debug) qDebug("YadiDMXUSBOut::closeOutput(%d), universe %d",output,universe);
	QMutexLocker lock(accessMutex);

	if ((int)output < output_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (yadi) {
			yadi->closeOutput();
		} else {
			qDebug("YadiDMXUSBOut::closeOutput(%d) Device not found",output);
		}
	}

	if (output < MAX_DEVS)
		outDevNameTable[output].clear();
}

int YadiDMXUSBOut::capabilities() const
{
	return QLCIOPlugin::Output | QLCIOPlugin::Input | QLCIOPlugin::Monitor;
}

QString YadiDMXUSBOut::pluginInfo()
{
	QString str;

	str += QString("<HTML>");
	str += QString("<HEAD>");
	str += QString("<TITLE>%1</TITLE>").arg(name());
	str += QString("</HEAD>");
	str += QString("<BODY>");

	str += QString("<P>");
	str += QString("<H3>%1</H3>").arg(name());
	str += tr("This plugin provides DMX output and input for stonechip YADI DMX controllers");
	str += QString("</P>");

	return str;
}
QStringList YadiDMXUSBOut::outputs()
{
	QMutexLocker lock(accessMutex);

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

void YadiDMXUSBOut::writeUniverse(quint32 universe, quint32 output, const QByteArray &data)
{
    Q_UNUSED(universe)

	QMutexLocker lock(accessMutex);

	if ((int)output >= output_devices.size()) {
		if (write_universe_debug_out) {
			write_universe_debug_out = false;
			qDebug("Yadi DMX plugin::writeUniverse: Non existing device adressed: output line: %d",output);
		}
		return;
	}
	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);

	if (!yadi || !yadi->isOutputOpen()) {
		if (write_universe_debug_out) {
			write_universe_debug_out = false;
			qDebug("Yadi DMX plugin:writeUniverse: Device is closed: output line: %d",output);
		}
		return;
	}
	else {
		write_universe_debug_out = true;
	}

	if (data.size() >  yadi->outUniverse.size()) {
		yadi->outUniverse.resize(data.size());
		qDebug() << "Yadi DMX plugin::writeUniverse:  resize output universe to" << data.size();
	}

	QTime stop;
	stop.restart();
	// qDebug() << QTime::currentTime().msec();

	// Check, what the fastest output variant is.
	int hi_changed_channel = 0;
	int changed_channels = 0;
	if (yadi->outputSendAllData) {
		yadi->outputSendAllData = false;
		changed_channels = data.size();
		hi_changed_channel = data.size();
	} else {
		int max_out = data.size();
		if (max_out > yadi->usedDmxOutChannels) {
			max_out = yadi->usedDmxOutChannels;
		}
		for (int t=0; t<max_out;t++) {
			if (data.at(t) != yadi->outUniverse.at(t)) {
				changed_channels++;
				hi_changed_channel = t+1;
			}
		}
	}

	if (changed_channels > 8 || hi_changed_channel > 50) {
//		for (int t=0; t<12;t++) {
//			if (universe.at(t) != yadi->outUniverse.at(t)) {
//				qDebug("chan %d: %d->%d",t,quint8(yadi->outUniverse.at(t)),quint8(universe.at(t)));
//			}
//		}
		unsigned char out[] = {'O',0,2};		// 512 Channels
		out[1] = hi_changed_channel;
		out[2] = hi_changed_channel>>8;
		int bytes = yadi->write((char*)out,3);
		if (bytes < 0) {
			handle_output_error(output);
			return;
		}
		bytes += yadi->write(data.data(),hi_changed_channel);
		if (bytes != hi_changed_channel + 3) {
			handle_output_error(output);
		} else {
			yadi->outUniverse = data;
		}
		if (debug > 2) qDebug("Yadi: out burst %dms hi_changed:%d(%d) %d %d",stop.elapsed(),hi_changed_channel,data.size(),out[1],out[2]);
	} else {
		for (int t=0; t<hi_changed_channel; t++) {
			if (data.at(t) != yadi->outUniverse.at(t) || yadi->outputSendAllData) {
				yadi->outUniverse[t] = data.at(t);
				QString dat = "o";
				dat += QString::number(t+1);
				dat += " ";
				dat += QString::number((quint8)data.at(t));
				dat += "\n";

				if (debug > 3) qDebug() << t << dat;
				int bytes = yadi->write(dat.toLocal8Bit().data());
				if (bytes != dat.size()) {
					handle_output_error(output);
					break;
				}
			}
		}
		if (debug > 2) qDebug("Yadi: out single %dms",stop.elapsed());
	}
	update_output_monitor(output,data);
}

QString YadiDMXUSBOut::outputInfo(quint32 output)
{
	QMutexLocker lock(accessMutex);

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

bool YadiDMXUSBOut::openInput(quint32 input, quint32 universe)
{
	qDebug("YadiDMXUSBOut::openInput(%d), universe %d",input,universe);
	return internOpenInput(input,universe);
}

void YadiDMXUSBOut::closeInput(quint32 input, quint32 universe)
{
	QMutexLocker lock(accessMutex);
	qDebug("YadiDMXUSBOut::closeInput(%d), universe: %d",input,universe);

	if ((int)input < input_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
		if (yadi) {
			// disconnect(yadi->inputThread(),0,this,0);
			yadi->closeInput();
		} else {
			qDebug("YadiDMXUSBOut::closeInput(%d) Device not found",input);
		}
	}

	if (input < MAX_DEVS)
		inDevNameTable[input].clear();
}

QStringList YadiDMXUSBOut::inputs()
{
	QMutexLocker lock(accessMutex);

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
	QMutexLocker lock(accessMutex);

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

void YadiDMXUSBOut::sendFeedBack(quint32 universe, quint32 inputLine, quint32 channel, uchar value, const QString &key)
{
	Q_UNUSED(universe);
	Q_UNUSED(inputLine);
	Q_UNUSED(channel);
	Q_UNUSED(value);
	Q_UNUSED(key);
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

	if (dialog.configChanged) {
		qDebug("Dialog Config changed");
		emit configurationChanged();
	}

	for (int t=0; t<YadiDeviceManager::devices(); t++) {
		if (YadiDeviceManager::deviceAt(t)->settingsChanged) {
			YadiDeviceManager::deviceAt(t)->saveConfig();
		}
	}
}

DmxMonitor *YadiDMXUSBOut::openOutputMonitor(quint32 output)
{
	QMutexLocker lock(accessMutex);

	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (yadi) {
		return yadi->openDmxOutMonitorWidget();
	} else {
		return 0;
	}
}

DmxMonitor *YadiDMXUSBOut::openInputMonitor(quint32 input)
{
	QMutexLocker lock(accessMutex);

	YadiDevice * yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
	if (yadi) {
		return yadi->openDmxInMonitorWidget();
	} else {
		return 0;
	}
}

void YadiDMXUSBOut::handle_output_error(quint32 output)
{
	accessMutex->lock();
	if (int(output) >= output_devices.size()) {
		accessMutex->unlock();
		return;
	}

	int errsv = errno;
	QString msg (QString("Communication error occured. Output %1 (%2)")
		   .arg(output+1).arg(strerror(errsv)));
	qDebug() << Q_FUNC_INFO << msg;
	emit errorMsgEmitted(msg);

	/// @todo: to have an existing device node does not mean necessarely that the device is still connected
	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (yadi) {
		yadi->deviceNodePresent = false;
		if (yadi->isOutputOpen())
			m_reOpenOutput = output;
		else
			m_reOpenOutput = -1;

		if (yadi->isInputOpen())
			m_reOpenInput = output;
		else
			m_reOpenInput = -1;

		yadi->closeInOut();

		qDebug("%s: Device '%s' now closed, due to error"
			   ,Q_FUNC_INFO,output_devices.at(output).toLocal8Bit().data());
	}


	accessMutex->unlock();

	emit communicationErrorDetected();
}

void YadiDMXUSBOut::update_output_monitor(quint32 output, const QByteArray &universe)
{
	QMutexLocker lock(accessMutex);
	if (int(output) >= output_devices.size())
		return;

	YadiDevice * yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
	if (!yadi) return;
	DmxMonitor *mon = yadi->dmxOutMonWidget;

	fprintf(stderr, "update output monitor (cnt:%d) %s\n",universe.size(),universe.toHex().data());

	if (mon) {
		for (int t=0; t<mon->visibleBars(); t++) {
			mon->setValueInBar(t,uchar(universe[t]));
		}
	}
}

bool YadiDMXUSBOut::internOpenOutput(quint32 output, int universe)
{
	QMutexLocker lock(accessMutex);

	bool ok = false;
	if ((int)output < output_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(output_devices.at(output),YadiDevice::FL_OUTPUT_UNIVERSE);
		if (yadi) {
			yadi->outputId = output;
			if (universe < 0) {
				universe = yadi->outUniverseNumber;
			} else {
				yadi->outUniverseNumber = universe;
			}
			ok = yadi->openOutput();
		}

		if (ok) {
			outDevNameTable[output] = output_devices.at(output);
			if (debug) qDebug("YadiDMXUSBOut::openOutput(%d) successful",output);
		} else {
			qDebug("YadiDMXUSBOut::openOutput(%d) failed!",output);
		}

	} else {
		qDebug("YadiDMXUSBOut::openOutput(%d) failed! Device list own %d outputs",output+1,output_devices.size());
	}
	return ok;
}

bool YadiDMXUSBOut::internOpenInput(quint32 input, int universe)
{
	QMutexLocker lock(accessMutex);

	bool ok = false;
	if ((int)input < input_devices.size()) {
		YadiDevice *yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);
		if (yadi) {
			yadi->inputId = input;
			if (universe < 0) {
				universe = yadi->inUniverseNumber;
			} else {
				yadi->inUniverseNumber = universe;
			}
			ok = yadi->openInput();
		}

		if (ok) {
			inDevNameTable[input] = input_devices.at(input);
			connect(yadi->inputThread(),SIGNAL(dmxInDeviceChannelChanged(quint32,quint32,quint32,uchar))
					,this,SLOT(propagateChangedInput(quint32,quint32,quint32,uchar)),Qt::UniqueConnection);
			connect(yadi->inputThread(),SIGNAL(exitReceiverWithFailure(int)),this,SLOT(inputDeviceFailed(int)),Qt::UniqueConnection);
		} else {
			qDebug("YadiDMXUSBOut::openInput(%d) failed!",input);
		}
	}

	return ok;
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

void YadiDMXUSBOut::propagateChangedInput(quint32 universe, quint32 input, quint32 channel, uchar value)
{
	emit valueChanged(universe, input, channel, value);
	// fprintf(stderr, "propagate changed input %d: %d\n",channel,value);

	if (debug > 1) qDebug("YadiDMXUSBOut::propagateChangedInput %d %d %d", input, channel, value);
}

void YadiDMXUSBOut::inputDeviceFailed(int input)
{
	qDebug("YadiDMXUSBOut::inputDeviceFailed(%d) failed!",input);

	accessMutex->lock();
	if (input >= input_devices.size()) {
		accessMutex->unlock();
		return;
	}

	// coming to this point normaly means the device is not connected anymore
	// So we close all devices to give the device nodes in /dev free

	YadiDevice * yadi = YadiDeviceManager::getDevice(input_devices.at(input),YadiDevice::FL_INPUT_UNIVERSE);

	if (yadi) {
		if (yadi->isOutputOpen())
			m_reOpenOutput = input;
		else
			m_reOpenOutput = -1;

		if (yadi->isInputOpen())
			m_reOpenInput = input;
		else
			m_reOpenInput = -1;

		qDebug("YadiDMXUSBOut::inputDeviceFailed: Close inputs and outputs");
		yadi->closeInOut();
	}

	accessMutex->unlock();

	emit communicationErrorDetected();
}

void YadiDMXUSBOut::outputDeviceFailed(int output)
{
	Q_UNUSED(output);

}

void YadiDMXUSBOut::handleCommunicationError()
{
	if (m_comErrorCounter > 2) {
		m_comErrorCounter = 0;
		m_reOpenInput = -1;
		m_reOpenOutput = -1;
		emit configurationChanged();
		return;
	}

	bool tryagain = false;
	if (m_reOpenOutput >= 0) {

		if (internOpenOutput(m_reOpenOutput, -1)) {
			QString msg(QString("Reopened output %1 after communication error (#%2)")
				   .arg(m_reOpenOutput+1).arg(m_totalComErrorCounter+1));
			qDebug() << Q_FUNC_INFO << msg;
			emit errorMsgEmitted(msg);
			m_reOpenOutput = -1;

		} else {
			qDebug("%s Error on reopening output %d after communication error"
				   ,Q_FUNC_INFO,m_reOpenOutput+1);
			tryagain = true;
		}
	}

	if (m_reOpenInput >= 0) {

		if (internOpenInput(m_reOpenInput, -1)) {
			QString msg(QString("Reopened input %1 after communication error (#%2)")
				   .arg(m_reOpenInput+1).arg(m_totalComErrorCounter+1));
			qDebug() << Q_FUNC_INFO << msg;
			emit errorMsgEmitted(msg);
			m_reOpenInput = -1;
		} else {
			qDebug("%s Error on reopening input %d after communication error"
				   ,Q_FUNC_INFO,m_reOpenInput+1);
		}
	}

	m_totalComErrorCounter++;

	if (tryagain) {
		m_comErrorCounter++;
		QTimer::singleShot(100,this,SLOT(handleCommunicationError()));
	}
}

/****************************************************************************
 * Plugin export
 ****************************************************************************/
#ifndef IS_QT5
Q_EXPORT_PLUGIN2(yadidmxusbout, YadiDMXUSBOut)
#endif

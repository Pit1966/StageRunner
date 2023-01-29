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

#include "configrev.h"
#include "yadidevice.h"
#include "yadidmxusbout.h"
#include "yadireceiver.h"
#include "serialwrapper.h"
#include "dmxmonitor.h"

#ifdef USE_QTSERIAL
#  include "qserialportthread.h"
#endif

#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QObject>

YadiDevice::YadiDevice(YadiDMXUSBOut *yadiPlugin, const QString &dev_node)
	: m_yadiPlugin(yadiPlugin)
{
	setDevNodePath(dev_node);
	init();
}

YadiDevice &YadiDevice::operator =(const YadiDevice &other)
{
	bool activate_again = false;
	m_yadiPlugin = other.m_yadiPlugin;

	if (m_devNodePath != other.m_devNodePath) {
		if (m_isDeviceActivated) {
			deActivateDevice();
			activate_again = true;
		}
		m_devNodePath = other.m_devNodePath;
		m_devNodeName = other.m_devNodeName;
	}

	debug = other.debug;
	outUniverseNumber = other.outUniverseNumber;
	inUniverseNumber = other.inUniverseNumber;
	deviceProductName = other.deviceProductName;
	idVendor = other.idVendor;
	idProduct = other.idProduct;
	deviceManufacturer = other.deviceManufacturer;
	deviceSerial = other.deviceSerial;
	capabilities = other.capabilities;
	currentDetectedDmxInPacketSize = other.currentDetectedDmxInPacketSize;
	usedDmxInChannels = other.usedDmxInChannels;
	usedDmxOutChannels = other.usedDmxOutChannels;
	maxDeviceDmxInChannels = other.maxDeviceDmxInChannels;
	maxDeviceDmxOutChannels = other.maxDeviceDmxOutChannels;
	deviceNodePresent = other.deviceNodePresent;
	outputSendAllData = other.outputSendAllData;

	if (activate_again)
		activateDevice();

	return *this;
}

const QString &YadiDevice::devNode() const
{
#if defined(USE_QTSERIAL)
	return devNodeName();
#else
	return devNodePath();
#endif
}

void YadiDevice::setDevNodePath(const QString &path)
{
	m_devNodePath = path;
	if (path.contains(QChar('/'))) {
		int i = path.lastIndexOf(QChar('/'));
		m_devNodeName = path.mid(i+1);
	} else {
		m_devNodeName = path;
	}
}

YadiDevice::~YadiDevice()
{
	deActivateDevice();

	closeDmxInMonitorWidget();
	closeDmxOutMonitorWidget();
}

YadiDevice::YadiDevice(const YadiDevice &other)
{
	init();
	*this = other;
}

bool YadiDevice::activateDevice()
{
	qDebug("Yadi: %s: activate device '%s'",threadNameAsc(),deviceProductName.toLocal8Bit().constData());

	// First let us look if we are already activated. If so and the
	// device path has changed, we deactivate the device first.
#ifdef USE_QTSERIAL
	if (m_serialThread && m_serialThread->deviceNode() != devNodePath)
		deActivateDevice();
#else
	if (file && file->deviceNode() != devNodePath())
		deActivateDevice();
#endif

	bool ok = true;
	// Here we prepare the device
	// We initiate the access object and if we need
	// we create the thread object that should read from the device later
#ifdef USE_QTSERIAL
	if (!m_serialThread) {
		if (deviceNodeExists(devNodePath)) {
			m_serialThread = new QSerialPortThread(this);
			outUniverse.fill(0,512);
			inUniverse.fill(0,512);
		} else {
			ok = false;
		}
	}
	if (m_serialThread) {
		m_isDeviceActivated = true;
		sendConfigToDevice();
	}

#else
	if (!file) {
		if (SerialWrapper::deviceNodeExists(devNode())) {
			file = new SerialWrapper(this, devNodePath());
			file->setProductName(deviceProductName);
			outUniverse.fill(0,512);
		} else {
			ok = false;
		}
	}
	if (file) {
		if ( (capabilities&FL_INPUT_UNIVERSE) && !input_thread ) {
			input_thread = new YadiReceiver(this);
			input_thread->setProductName(deviceProductName);
			inUniverse.fill(0,512);
		}
		else if (!(capabilities&FL_INPUT_UNIVERSE) && input_thread) {
			input_thread->stopRxDmx();
			delete input_thread;
			input_thread = 0;
		}
		m_isDeviceActivated = true;
		sendConfigToDevice();
	}
#endif


	return ok;
}

void YadiDevice::deActivateDevice()
{
#ifdef USE_QTSERIAL
	if (m_serialThread) {
		m_serialThread->sendCommand(QSerialPortThread::CMD_STOP_ALL);
		m_isDeviceActivated = false;
	}
#else
	if (input_thread) {
		input_thread->stopRxDmx();
		delete input_thread;
		input_thread = 0;
	}
	if (file) {
		delete file;
		file = nullptr;
	}

	m_isInputOpen = false;
	m_isOutputOpen = false;
	m_isDeviceActivated = false;
#endif

}

bool YadiDevice::openOutput()
{
	qDebug("Yadi: %s: open output for node '%s'", threadNameAsc(), devNode().toLocal8Bit().constData());
	if (!m_isDeviceActivated) {
		if (!activateDevice())
			return false;
	}

	outputSendAllData = true;

#ifdef USE_QTSERIAL
	m_serialThread->sendCommand(QSerialPortThread::CMD_START_OUTPUT);
	m_isOutputOpen = true;
#else
	if (!file->isOpen()) {
		m_isOutputOpen = file->openSerial();
		if (m_isOutputOpen) {
			QByteArray version;
			file->writeCommand("v", &version);
			QByteArray channels;
			file->writeCommand("c", &channels);
			currentDetectedDmxInPacketSize = channels.split(' ').at(0).toInt();
		}

	} else {
		m_isOutputOpen = true;
	}
#endif

	return m_isOutputOpen;
}

void YadiDevice::closeOutput()
{
	qDebug("Yadi: %s: close output",threadNameAsc());
#ifdef USE_QTSERIAL
	if (m_serialThread) {
		m_serialThread->sendCommand(QSerialPortThread::CMD_STOP_OUTPUT);
	}
#else
	if (file && !m_isInputOpen) {
		file->closeSerial();
	}
#endif
	m_isOutputOpen = false;
}

bool YadiDevice::openInput()
{
	qDebug("Yadi: %s: open input for node '%s'", threadNameAsc(), devNode().toLocal8Bit().constData());
	if (!m_isDeviceActivated) {
		if (!activateDevice())
			return false;
	}

#ifdef USE_QTSERIAL
	m_serialThread->sendCommand(QSerialPortThread::CMD_START_INPUT);
	m_isInputOpen = true;
#else
	if (!file->isOpen()) {
		m_isInputOpen = file->openSerial();
	} else {
		m_isInputOpen = true;
	}
	if (m_isInputOpen && input_thread) {
		input_thread->startRxDmx(inputId);
	}
#endif

	return m_isInputOpen;
}

void YadiDevice::closeInput()
{
#ifdef USE_QTSERIAL
	if (m_serialThread) {
		m_serialThread->sendCommand(QSerialPortThread::CMD_STOP_INPUT);
	}
#else
	if (input_thread) {
		input_thread->stopRxDmx();
	}

	if (file && !m_isOutputOpen) {
		file->closeSerial();
	}
#endif

	m_isInputOpen = false;
}

void YadiDevice::closeInOut()
{
#ifdef USE_QTSERIAL
	if (m_serialThread) {
		m_serialThread->sendCommand(QSerialPortThread::CMD_STOP_ALL);
	}
#else
	if (input_thread) {
		input_thread->stopRxDmx();
	}

	if (file) {
		file->closeSerial();
	}
#endif
	m_isInputOpen = false;
	m_isOutputOpen = false;

}

QByteArray YadiDevice::read(qint64 size)
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->read(size);
#else
	if (file)
		return file->readSerial(size);
#endif
	return QByteArray();
}

qint64 YadiDevice::read(char *buf, qint64 size)
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->read(buf, size);
#else
	if (file)
		return file->readSerial(buf, size);
#endif
	return -1;
}

qint64 YadiDevice::write(const char *buf)
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->write(buf);
#else
	if (file) {
		return file->writeSerial(buf);
	}
#endif
	return -1;
}

qint64 YadiDevice::write(const char *buf, qint64 size)
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->write(buf, size);
#else
	if (file)
		return file->writeSerial(buf,size);
#endif
	return -1;
}

bool YadiDevice::isOutputOpen()
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->isOutputOpen();
#else
	if (file)
		return file->isOpen() && m_isOutputOpen;
#endif
	return false;
}

bool YadiDevice::isInputOpen()
{
#ifdef USE_QTSERIAL
	if (m_serialThread)
		return m_serialThread->isInputOpen();
#else
	if (file)
		return file->isOpen() && m_isInputOpen;
#endif
	return false;
}

bool YadiDevice::checkDeviceNode()
{
	deviceNodePresent = deviceNodeExists(devNode());
	return deviceNodePresent;
}

void YadiDevice::saveConfig()
{
	if (0 == capabilities || deviceProductName.isEmpty()) return;

	QSettings set(QSETFORMAT,"YadiDmxDevice");

	QString group = deviceProductName + "_SN" + deviceSerial;
	group.replace(QChar(' '),QChar('_'));

	set.beginGroup(group);
	set.setValue("Capabilities",capabilities);
	set.setValue("UsedDmxInChannels",usedDmxInChannels);
	set.setValue("UsedDmxOutChannels",usedDmxOutChannels);
	set.setValue("MaxDmxInChannels",maxDeviceDmxInChannels);
	set.setValue("MaxDmxOutChannels",maxDeviceDmxOutChannels);
	set.setValue("InputId",inputId);
	set.setValue("OutputId",outputId);
	set.setValue("UniverseMergeMode",universeMergeMode);
	set.setValue("DebugLevel",debug);
	set.endGroup();
}

void YadiDevice::loadConfig()
{
	QSettings set(QSETFORMAT,"YadiDmxDevice");

	QString group = deviceProductName + "_SN" + deviceSerial;
	group.replace(QChar(' '),QChar('_'));

	set.beginGroup(group);
	if (set.contains("Capabilities")) {
		qDebug() << "Load settings for device" << group;
		capabilities = set.value("Capabilities").toInt();
		usedDmxInChannels = set.value("UsedDmxInChannels").toInt();
		usedDmxOutChannels = set.value("UsedDmxOutChannels").toInt();
		maxDeviceDmxInChannels = set.value("MaxDmxInChannels").toInt();
		maxDeviceDmxOutChannels = set.value("MaxDmxOutChannels").toInt();
		inputId = set.value("InputId").toInt();
		outputId = set.value("OutputId").toInt();
		universeMergeMode = set.value("UniverseMergeMode").toInt();
		debug = set.value("DebugLevel").toInt();
	} else {
		qDebug() << Q_FUNC_INFO << " No settings found for device" << deviceProductName;
	}
	set.endGroup();
}

void YadiDevice::sendConfigToDevice()
{
	qDebug("Yadi: %s: YadiDevice::sendConfigToDevice '%s'"
		   , threadNameAsc()
		   , devNode().toLocal8Bit().constData());
	/// @todo error checking!
	bool old_open_output_state = m_isOutputOpen;

	if (openOutput()) {
		QString cmd;
		if ((capabilities & FL_INOUT_UNIVERSE) == FL_INOUT_UNIVERSE) {
			cmd = QString("m %1\n").arg(universeMergeMode);
			write(cmd.toLocal8Bit().data());
		}
		if (capabilities & FL_INPUT_UNIVERSE) {
			cmd = QString("hi %1\n").arg(usedDmxInChannels);
			write(cmd.toLocal8Bit().data());
		}
		if (capabilities & FL_OUTPUT_UNIVERSE) {
			cmd = QString("ho %1\n").arg(usedDmxOutChannels);
			write(cmd.toLocal8Bit().data());
		}

		/// @todo wait a little bit, since write command returns immediately in USE_QTSERIAL mode

		if (!old_open_output_state)
			closeOutput();
	}
}

void YadiDevice::sendStatusMsg(const QString &msg)
{
	QString txt = QString("%1: %2").arg(m_devNodeName, msg);
	m_yadiPlugin->sendStatusMsg(txt);
}

void YadiDevice::sendErrorMsg(const QString &msg)
{
	QString txt = QString("%1: %2").arg(m_devNodeName, msg);
	m_yadiPlugin->sendErrorMsg(txt);
}

DmxMonitor *YadiDevice::openDmxInMonitorWidget()
{
	if (!dmxInMonWidget) {
		dmxInMonWidget = new DmxMonitor;
		dmxInMonWidget->resize(1400, 800);
		dmxInMonWidget->setWindowTitle(QObject::tr("DMX Input Monitor V0.2 - Universe %1").arg(inUniverseNumber+1));
		dmxInMonWidget->setChannelPeakBars(usedDmxInChannels);
#ifdef USE_QTSERIAL
		if (m_serialThread) {
			QObject::connect(m_serialThread,SIGNAL(dmxInChannelChanged(quint32,uchar)),dmxInMonWidget,SLOT(setValueInBar(quint32,uchar)));
			QObject::connect(m_serialThread,SIGNAL(dmxPacketReceived(YadiDevice*,QString)),dmxInMonWidget,SLOT(setFrameRateInfo(YadiDevice*,QString)));
		}
#else
		QObject::connect(input_thread,SIGNAL(dmxInChannelChanged(quint32,uchar)),dmxInMonWidget,SLOT(setValueInBar(quint32,uchar)));
		QObject::connect(input_thread,SIGNAL(dmxPacketReceived(YadiDevice*,QString)),dmxInMonWidget,SLOT(setFrameRateInfo(YadiDevice*,QString)));
#endif
	} else {
		dmxInMonWidget->setChannelPeakBars(usedDmxInChannels);
	}

	for (int t=0; t<usedDmxInChannels; t++) {
		dmxInMonWidget->setValueInBar(t,uchar(inUniverse[t]));
	}

	return dmxInMonWidget;
}

DmxMonitor *YadiDevice::openDmxOutMonitorWidget()
{
	if (!dmxOutMonWidget) {
		dmxOutMonWidget = new DmxMonitor;
		dmxOutMonWidget->setWindowTitle(QObject::tr("DMX Output Monitor V0.2 - Universe %1").arg(outUniverseNumber+1));
		dmxOutMonWidget->setChannelPeakBars(usedDmxOutChannels);
	} else {
		dmxOutMonWidget->setChannelPeakBars(usedDmxOutChannels);
	}

	for (int t=0; t<usedDmxOutChannels; t++) {
		dmxOutMonWidget->setValueInBar(t,uchar(outUniverse[t]));
	}

	return dmxOutMonWidget;
}

void YadiDevice::closeDmxInMonitorWidget()
{
	if (dmxInMonWidget) {
		dmxInMonWidget->close();
		delete dmxInMonWidget;
		dmxInMonWidget = 0;
	}
}

void YadiDevice::closeDmxOutMonitorWidget()
{
	if (dmxOutMonWidget) {
		dmxOutMonWidget->close();
		delete dmxOutMonWidget;
		dmxOutMonWidget = 0;
	}
}

bool YadiDevice::deviceNodeExists(const QString &dev_node)
{
#if defined(USE_QTSERIAL)
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		if (info.portName() == dev_node)
			return true;
	}

#elif defined(Q_OS_WIN32)
	return true;

#elif defined(Q_OS_UNIX)
	if (dev_node.size() && QFile::exists(dev_node)) {
		return true;
	} else {
		return false;
	}
#endif

	return false;
}

QString YadiDevice::threadName()
{
	return QThread::currentThread()->objectName();
}

const char * YadiDevice::threadNameAsc()
{
	static char outbuf[200];
	sprintf(outbuf, "%s", QThread::currentThread()->objectName().toLocal8Bit().constData());
	return outbuf;
}

void YadiDevice::init()
{
	debug = 0;
	outUniverseNumber = -1;
	inUniverseNumber = -1;
	deviceNodePresent = false;
	maxDeviceDmxInChannels = 512;
	maxDeviceDmxOutChannels = 512;
	usedDmxInChannels = -1;
	usedDmxOutChannels = -1;
	capabilities = FL_CLEAR;
	currentDetectedDmxInPacketSize = 0;
#ifdef USE_QTSERIAL
	m_serialThread = nullptr;
#else
	input_thread = nullptr;
	file = nullptr;
#endif
	m_isInputOpen = 0;
	m_isOutputOpen = 0;
	m_isDeviceActivated = false;
	outputId = 0;
	inputId = 0;
	universeMergeMode = 0;
	settingsChanged = false;
	dmxInMonWidget = 0;
	dmxOutMonWidget = 0;
}

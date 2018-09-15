#include "yadidevice.h"
#include "yadireceiver.h"
#include "serialwrapper.h"
#include "dmxmonitor.h"
#include "configrev.h"

#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QObject>

YadiDevice::YadiDevice(const QString &dev_node)
{
	devNodePath = dev_node;
	init();
}

YadiDevice &YadiDevice::operator =(const YadiDevice &other)
{
	bool was_activated = (0 != file);
	bool activate_again = false;

	if (devNodePath != other.devNodePath) {
		if (was_activated) {
			deActivateDevice();
			activate_again = true;
		}
		devNodePath = other.devNodePath;
	}

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
	if (file && file->deviceNode() != devNodePath) {
		deActivateDevice();
	}

	bool ok = true;
	// Here we prepare the device
	// We initiate the access object and if we need
	// we create the thread object that should read from the device later
	if (!file) {
		if (SerialWrapper::deviceNodeExists(devNodePath)) {
			file = new SerialWrapper(this, devNodePath);
			outUniverse.fill(0,512);
		} else {
			ok = false;
		}
	}

	if (file) {
		if ( (capabilities&FL_INPUT_UNIVERSE) && !input_thread ) {
			input_thread = new YadiReceiver(this);
			inUniverse.fill(0,512);
		}
		else if (!(capabilities&FL_INPUT_UNIVERSE) && input_thread) {
			input_thread->stopRxDmx();
			delete input_thread;
			input_thread = 0;
		}
		sendConfigToDevice();
	}

	return ok;
}

void YadiDevice::deActivateDevice()
{
	if (input_thread) {
		input_thread->stopRxDmx();
		delete input_thread;
		input_thread = 0;
	}
	if (file) {
		delete file;
		file = nullptr;
	}

	input_open_f = false;
	output_open_f = false;
}

bool YadiDevice::openOutput()
{
	qDebug("Yadi: %s: open output for node '%s'", threadNameAsc(), devNodePath.toLocal8Bit().constData());
	if (!file) {
		if (!activateDevice())
			return false;
	}

	outputSendAllData = true;

#ifdef QTSERIAL
	output_open_f = true;
#else
	if (!file->isOpen()) {
		output_open_f = file->openSerial();
	} else {
		output_open_f = true;
	}
#endif

	return output_open_f;
}

void YadiDevice::closeOutput()
{
	qDebug("Yadi: %s: close output",threadNameAsc());
	if (file && !input_open_f) {
		file->closeSerial();
	}
	output_open_f = false;
}

bool YadiDevice::openInput()
{
	qDebug("Yadi: %s: open input for node '%s'", threadNameAsc(), devNodePath.toLocal8Bit().constData());
	if (!file) {
		if (!activateDevice())
			return false;
	}

#ifdef QTSERIAL
	input_open_f = true;
#else
	if (!file->isOpen()) {
		input_open_f = file->openSerial();
	} else {
		input_open_f = true;
	}
#endif

	if (input_open_f && input_thread) {
		input_thread->startRxDmx(inputId);
	}

	return input_open_f;
}

void YadiDevice::closeInput()
{
	if (input_thread) {
		input_thread->stopRxDmx();
	}

	if (file && !output_open_f) {
		file->closeSerial();
	}
	input_open_f = false;
}

void YadiDevice::closeInOut()
{
	if (input_thread) {
		input_thread->stopRxDmx();
	}

	if (file) {
		file->closeSerial();
	}
	input_open_f = false;
	output_open_f = false;

}

QByteArray YadiDevice::read(qint64 size)
{
	if (file) {
		return file->readSerial(size);
	} else {
		return QByteArray();
	}
}

qint64 YadiDevice::read(char *buf, qint64 size)
{
	if (file) {
		return file->readSerial(buf, size);
	} else {
		return -1;
	}
}

qint64 YadiDevice::write(const char *buf)
{
	if (file) {
		return file->writeSerial(buf);
	} else {
		return -1;
	}
}

qint64 YadiDevice::write(const char *buf, qint64 size)
{
	if (file) {
		return file->writeSerial(buf,size);
	} else {
		return -1;
	}
}

bool YadiDevice::isOutputOpen()
{
	if (file) {
#ifdef QTSERIAL
		return output_open_f;
#else
		return file->isOpen() && output_open_f;
#endif
	} else {
		return false;
	}
}

bool YadiDevice::isInputOpen()
{
	if (file) {
		return file->isOpen() && input_open_f;
	} else {
		return false;
	}
}

bool YadiDevice::checkDeviceNode()
{
	deviceNodePresent = SerialWrapper::deviceNodeExists(devNodePath);
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
		   , devNodePath.toLocal8Bit().constData());
	/// @todo error checking!
	bool old_open_output_state = output_open_f;

	if (openOutput()) {
		QString cmd;
		if ((capabilities & FL_INOUT_UNIVERSE) == FL_INOUT_UNIVERSE) {
			cmd = QString("m %1").arg(universeMergeMode);
			write(cmd.toLocal8Bit().data());
		}
		if (capabilities & FL_INPUT_UNIVERSE) {
			cmd = QString("hi %1").arg(usedDmxInChannels);
			write(cmd.toLocal8Bit().data());
		}
		if (capabilities & FL_OUTPUT_UNIVERSE) {
			cmd = QString("ho %1").arg(usedDmxOutChannels);
			write(cmd.toLocal8Bit().data());
		}

		if (!old_open_output_state)
			closeOutput();
	}
}

DmxMonitor *YadiDevice::openDmxInMonitorWidget()
{
	if (!dmxInMonWidget) {
		dmxInMonWidget = new DmxMonitor;
		dmxInMonWidget->setWindowTitle(QObject::tr("DMX Input Monitor V0.2 - Universe %1").arg(inUniverseNumber+1));
		dmxInMonWidget->setChannelPeakBars(usedDmxInChannels);
		QObject::connect(input_thread,SIGNAL(dmxInChannelChanged(quint32,uchar)),dmxInMonWidget,SLOT(setValueInBar(quint32,uchar)));
		QObject::connect(input_thread,SIGNAL(dmxPacketReceived(YadiDevice*,QString)),dmxInMonWidget,SLOT(setFrameRateInfo(YadiDevice*,QString)));
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
	file = 0;
	input_thread = 0;
	input_open_f = 0;
	output_open_f = 0;
	outputId = 0;
	inputId = 0;
	universeMergeMode = 0;
	settingsChanged = false;
	dmxInMonWidget = 0;
	dmxOutMonWidget = 0;
}

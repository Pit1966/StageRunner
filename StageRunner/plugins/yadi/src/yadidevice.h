/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef YADIDEVICE_H
#define YADIDEVICE_H

#include <QString>

class YadiReceiver;
class SerialWrapper;
class DmxMonitor;
class MvgAvgCollector;
class QSerialPortThread;
class YadiDMXUSBOut;


/**
 * @brief The YadiDevice class
 *
 * working parameter (get with stty -F /dev/ttyACM0)
 * speed 115200 baud; line = 0;
 * min = 1; time = 0;
 * -brkint -icrnl -imaxbel
 * -opost
 * -isig -icanon -echo -echoe

 *
 *
 */
class YadiDevice
{
private:
	YadiDMXUSBOut *m_yadiPlugin;			// the parent plugin
	QString m_devNodePath;
	QString m_devNodeName;

public:
	enum Flags {
		FL_CLEAR = 0,
		FL_OUTPUT_UNIVERSE = 1<<0,
		FL_INPUT_UNIVERSE = 1<<1,
		FL_HAS_MONITOR = 1<<2,

		FL_INOUT_UNIVERSE = FL_OUTPUT_UNIVERSE + FL_INPUT_UNIVERSE
	};
	int debug;
	int outUniverseNumber;
	int inUniverseNumber;				// -1: unused

	QByteArray outUniverse;
	QByteArray inUniverse;

	QString deviceProductName;
	QString idVendor;
	QString idProduct;
	QString deviceManufacturer;
	QString deviceSerial;

	int capabilities;
	int currentDetectedDmxInPacketSize;
	int usedDmxInChannels;
	int usedDmxOutChannels;
	int maxDeviceDmxInChannels;
	int maxDeviceDmxOutChannels;
	int universeMergeMode;
	bool deviceNodePresent;
	bool outputSendAllData;

	quint32 inputId;
	quint32 outputId;

	bool settingsChanged;				///< becomes true, if settings for device changed and will be written by QSettings later
	DmxMonitor *dmxInMonWidget;			///< A Pointer to a widget that allows the monitoring of input/output DMX data
	DmxMonitor *dmxOutMonWidget;

private:
#ifdef USE_QTSERIAL
	QSerialPortThread *m_serialThread;
#else
	YadiReceiver *input_thread;
	SerialWrapper *file;
#endif
	bool m_isInputOpen;
	bool m_isOutputOpen;
	bool m_isDeviceActivated;

public:
	YadiDevice(YadiDMXUSBOut *yadiPlugin, const QString & dev_node);
	~YadiDevice();
	YadiDevice(const YadiDevice & other);
	YadiDevice & operator= (const YadiDevice & other);

	inline const QString & devNodeName() const {return m_devNodeName;}
	inline const QString & devNodePath() const {return m_devNodePath;}
	const QString & devNode() const;
	void setDevNodePath(const QString &path);


	bool activateDevice();
	void deActivateDevice();
	bool openOutput();
	void closeOutput();
	bool openInput();
	void closeInput();
	void closeInOut();

#ifdef USE_QTSERIAL
	QSerialPortThread * serialPortThread() {return m_serialThread;}
#else
	inline SerialWrapper *serialDev() const {return file;}
	YadiReceiver *inputThread() {return input_thread;}
#endif

	QByteArray read(qint64 size);
	qint64 read(char *buf, qint64 size);
	qint64 write(const char * buf);
	qint64 write(const char *buf, qint64 size);
	bool isOutputOpen();
	bool isInputOpen();
	bool checkDeviceNode();

	void saveConfig();
	void loadConfig();
	void sendConfigToDevice();

	void sendStatusMsg(const QString &msg);
	void sendErrorMsg(const QString &msg);

	DmxMonitor *openDmxInMonitorWidget();
	DmxMonitor *openDmxOutMonitorWidget();
	void closeDmxInMonitorWidget();
	void closeDmxOutMonitorWidget();

	static bool deviceNodeExists(const QString &dev_node);
	static QString threadName();
	static const char *threadNameAsc();

private:
	void init();


};

#endif // YADIDEVICE_H

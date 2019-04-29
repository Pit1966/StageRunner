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

#ifndef YADIDMXUSBOUT_H
#define YADIDMXUSBOUT_H

#include "qlcioplugin.h"

#include <QStringList>
#include <QMutex>


#define MAX_DEVS 10

class SerialWrapper;
class YadiDevice;
class DmxMonitor;

class YadiDMXUSBOut : public QLCIOPlugin
{
	Q_OBJECT

	Q_INTERFACES(QLCIOPlugin)
#ifdef IS_QT5
	Q_PLUGIN_METADATA(IID "de.stonechip.stagerunner.yadi" FILE "yadi.json")
#endif
public:
	int debug;
	QString inDevNameTable[MAX_DEVS];		///< this list maps the current opened input device numbers to the device name
	QString outDevNameTable[MAX_DEVS];		///< this list maps the current opened output device numbers to the device name

private:
	QStringList output_devices;
	QStringList input_devices;

	QMutex *accessMutex;
	bool write_universe_debug_out;
	int m_reOpenOutput;
	int m_reOpenInput;
	int m_comErrorCounter;
	int m_totalComErrorCounter;

public:
	YadiDMXUSBOut();
	~YadiDMXUSBOut();

	void init();						///< @reimp
	bool findDevices(bool update = false);
	QString name();						///< @reimp
	int capabilities() const;			///< @reimp
	QString pluginInfo();
	bool openOutput(quint32 output, quint32 universe);		///< @reimp
	void closeOutput(quint32 output, quint32 universe);		///< @reimp
	QStringList outputs();				///< @reimp
	void writeUniverse(quint32 universe, quint32 output, const QByteArray& data); ///< @reimp
	QString outputInfo(quint32 output = QLCIOPlugin::invalidLine()); ///< @reimp
	bool openInput(quint32 input, quint32 universe);		///< @reimp
	void closeInput(quint32 input, quint32 universe);		///< @reimp
	QStringList inputs();				///< @reimp
	QString inputInfo(quint32 input);	///< @reimp
	void sendFeedBack(quint32 universe, quint32 inputLine, quint32 channel, uchar value, const QString& key = 0);

	bool canConfigure();				///< @reimp
	void configure();					///< @reimp

	// StageRunner reimplemented functions
	DmxMonitor *openOutputMonitor(quint32 output);
	DmxMonitor *openInputMonitor(quint32 input);

	inline QStringList outputDeviceList() {return output_devices;}
	inline QStringList inputDeviceList() {return input_devices;}

	// Special functions only implemented by YADI
	Q_INVOKABLE bool setInOutMergeMode(quint32 input, quint32 universe, quint32 mode);

private:
	void handle_output_error(quint32 output);
	void update_output_monitor(quint32 output, const QByteArray& universe);
	bool internOpenOutput(quint32 output, int universe);
	bool internOpenInput(quint32 input, int universe);

signals:
	void communicationErrorDetected();
	void errorMsgEmitted(const QString &msg);
	void statusMsgEmitted(const QString &msg);

public slots:
	void closeMonitorByInstancePointer(DmxMonitor *instance);
	void propagateChangedInput(quint32 universe, quint32 input, quint32 channel, uchar value);
	void propagateReceiverFrameRate(YadiDevice *yadiDev, const QString &text);
	void inputDeviceFailed(int input);
	void outputDeviceFailed(int output);
	void handleCommunicationError();

};

#endif // YADIDMXUSBOUT_H

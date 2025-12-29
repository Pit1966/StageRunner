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

#ifndef QSERIALPORTTHREAD_H
#define QSERIALPORTTHREAD_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QElapsedTimer>

#include "../interfaces/qt_versions.h"


class YadiDevice;
class MvgAvgCollector;

static const char * _asc_cmds[] = {
	"NONE",
	"STOP_ALL",
	"STOP_ERROR",
	"START_OUTPUT",
	"START_INPUT",
	"STOP_OUTPUT",
	"STOP_INPUT",
	"DEVELOPER ENTER CMD DESPRICTION HERE"
};

class QSerialPortThread : public QThread
{
	Q_OBJECT

public:
	enum CMD
	{
		CMD_NONE,
		CMD_STOP_ALL,				///< terminate thread
		CMD_STOP_ERROR,				///< terminate thread and exit with error
		CMD_START_OUTPUT,
		CMD_START_INPUT,
		CMD_STOP_OUTPUT,
		CMD_STOP_INPUT,
		CMD_REQ_CHANNELS,			///< Request detected DMX Framesize:  Yadi "c" command
		CMD_REQ_UNIVERSE_SIZE,		///< get max and configured universe size: Yadi "ui" command
		CMD_REQ_VERSION				///< get Firmware version string: Yadi "v" command
	};

private:
	YadiDevice *m_yadiDev;
	QSerialPortInfo m_portInfo;
	QSerialPort *m_serialPort;
	QMutex m_accessMutex;						///< this is for data list
	QRecursiveMutex m_serialMutex;				///< Access protection to serial device
	volatile CMD m_threadCmd;
	volatile CMD m_internalCmd;					///< this is internal command store. You should not set a new command if there ist already one

	// inter thread access
	QList<QByteArray> m_sendDataList;			///< this is a queue of strings to be send via serial device

	// status
	volatile bool m_sendDataListHasData;		///< is true, when sendDataList has at least one entry
	bool m_isOutputOpen;
	bool m_isInputOpen;
	bool m_emitAllInputData;                    ///< if set, the complete input universe will be send, than the flag is resetted
	volatile bool m_isInThreadLoop;

	bool m_waitForAtChar;
	int m_inputDmxFrameCnt;
	int m_inputDmxFrameSize;
	QByteArray m_dmxInLine;

	// Framerate and DMX frame detection
	QElapsedTimer m_time;
	MvgAvgCollector *m_frameRateAvg;


public:
	QSerialPortThread(YadiDevice *dev);
	~QSerialPortThread();
	QString deviceNode() const;
	bool sendCommand(CMD cmd);

	QByteArray read(qint64 size);
	qint64 read(char *buf, qint64 size);
	qint64 write(const char *buf);
	qint64 write(const char *buf, qint64 size);
	bool isOutputOpen() const;
	bool isInputOpen() const;

private:
	void run();
	bool _executeCurrentCommand();
	bool _openSerial();
	void _closeSerial();
	bool _openOutput();
	void _closeOutput();
	bool _openInput();
	void _closeInput();
	bool _processSendQueue();
	void _clearSendQueue();

	static const char * ascCmd(int cmdno);

	void processDmxDataLine();

private slots:
	void onError(const QSerialPort::SerialPortError error);
    void onDataReceived();
	void onCompleteFrameReceived(int rxDataSize);

signals:
	void dmxInDeviceChannelChanged(quint32 universe, quint32 input, quint32 channel, uchar value);
	void dmxInChannelChanged(quint32, uchar);
	void dmxPacketReceived(YadiDevice* dev, const QString &str);
    void exitReceiverWithFailure(int input);
    void statusMsgSent(const QString &msg);
    void errorMsgSent(const QString &msg);

};

#endif // QSERIALPORTTHREAD_H

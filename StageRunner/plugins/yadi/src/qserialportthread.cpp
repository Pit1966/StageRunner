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

#include "qserialportthread.h"
#include "yadidevice.h"
#include "mvgavgcollector.h"

#include <QDebug>
#include <QElapsedTimer>


QSerialPortThread::QSerialPortThread(YadiDevice *dev)
	: QThread()
	, m_yadiDev(dev)
	, m_serialPort(nullptr)
	, m_threadCmd(CMD_NONE)
	, m_sendDataListHasData(false)
	, m_isOutputOpen(false)
	, m_isInputOpen(false)
	, m_emitAllInputData(false)
	, m_isInThreadLoop(false)
	, m_waitForAtChar(true)
	, m_inputDmxFrameCnt(0)
	, m_inputDmxFrameSize(0)
	, m_frameRateAvg(new MvgAvgCollector(40))
{
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		if (info.portName() == dev->devNode()) {
			m_portInfo = info;
			break;
		}
	}
	setObjectName("QSerialPortThread");

	qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
}

QSerialPortThread::~QSerialPortThread()
{
	if (isRunning()) {
		qWarning() << "Yadi listener thread still running on destroy";
		m_threadCmd = CMD_STOP_ALL;
		wait(2000);
	}

	delete m_frameRateAvg;
}

QString QSerialPortThread::deviceNode() const
{
	if (!m_portInfo.isValid())
		return QString();

	return m_portInfo.portName();
}

bool QSerialPortThread::sendCommand(QSerialPortThread::CMD cmd)
{
	//	if (m_yadiDev->debug)
	qDebug("Yadi: %s: send cmd '%s'",YadiDevice::threadNameAsc(),ascCmd(cmd));
	QElapsedTimer time;
	time.start();

	switch (cmd) {
	case CMD_START_INPUT:
		if (!isRunning())
			start();
		break;
	case CMD_START_OUTPUT:
		if (!isRunning())
			start();
		break;

	case CMD_STOP_ALL:
	case CMD_STOP_INPUT:
	case CMD_STOP_OUTPUT:
		if (!isRunning())
			return true;
		break;

	default:
		return true;
	}

	m_threadCmd = cmd;
	while (m_threadCmd > CMD_NONE && isRunning()) {;}

	qDebug("Yadi: Thread command %s executed after %llims",ascCmd(cmd),time.elapsed());

	return true;
}

QByteArray QSerialPortThread::read(qint64 size)
{
	return QByteArray();
}

qint64 QSerialPortThread::read(char *buf, qint64 size)
{
	return 0;
}

qint64 QSerialPortThread::write(const char *buf)
{
	QByteArray data(buf);
	int cnt = data.size();

	m_accessMutex.lock();
	m_sendDataList.append(data);
	m_sendDataListHasData = true;
	//	qDebug("Yadi: %s write serial 1: %s",YadiDevice::threadNameAsc(),data.constData());
	m_accessMutex.unlock();
	return cnt;
}

qint64 QSerialPortThread::write(const char *buf, qint64 size)
{
	QByteArray data(buf, size);
	int cnt = data.size();

	m_accessMutex.lock();
	m_sendDataList.append(data);
	//	qDebug("Yadi: %s write serial 2: %s",YadiDevice::threadNameAsc(),data.constData());
	m_sendDataListHasData = true;
	m_accessMutex.unlock();
	return cnt;
}

bool QSerialPortThread::isOutputOpen() const
{
	return m_isOutputOpen;
}

bool QSerialPortThread::isInputOpen() const
{
	return m_isInputOpen;
}

void QSerialPortThread::run()
{
	emit statusMsgSent(tr("Yadi port thread started"));

	m_serialPort = new QSerialPort(m_portInfo);
	connect(m_serialPort,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(onError(QSerialPort::SerialPortError)));
	connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(onDataReceived()));

	m_inputDmxFrameCnt = 0;
	m_waitForAtChar = true;

	m_isInThreadLoop = true;
	while (m_threadCmd != CMD_STOP_ALL && m_threadCmd != CMD_STOP_ERROR) {
		// check for thread command and execute if there is one
		if (m_threadCmd > CMD_NONE) {
			if (! _executeCurrentCommand()) {
				m_threadCmd = CMD_STOP_ERROR;
				break;
			}
		}

		// check serial send data queue
		if (m_isOutputOpen && m_sendDataListHasData == true)
			_processSendQueue();

		// check if data is available. If this is true,
		// onDataReceived() is called automatically
		bool waitok = false;
		if (m_serialPort->isOpen())
			waitok = m_serialPort->waitForReadyRead(5);

		// Sleep a little bit, commands should not arrive that fast
		if (!waitok)
			msleep(10);
	}
	m_isInThreadLoop = false;

	_closeOutput();
	_closeInput();

	delete m_serialPort;
	m_serialPort = nullptr;

	if (m_threadCmd == CMD_STOP_ERROR)
		qWarning() << "Yadi: Thread exited with error";
	else
		qDebug() << "Yadi: Thread finished";

	m_threadCmd = CMD_NONE;
}

/**
 * @brief Process the current thread command. This function is called only from running thread!
 * @return true on success.
 */
bool QSerialPortThread::_executeCurrentCommand()
{
	Q_ASSERT(isRunning());
	bool ok = true;

	switch (m_threadCmd) {
	case CMD_START_OUTPUT:
		if (!m_isOutputOpen)
			ok = _openOutput();
		break;

	case CMD_START_INPUT:
		if (!m_isInputOpen)
			ok = _openInput();
		break;

	case CMD_STOP_OUTPUT:
		_closeOutput();
		break;

	case CMD_STOP_INPUT:
		_closeInput();
		break;

	case CMD_STOP_ALL:
	case CMD_STOP_ERROR:
		return true;

	default:
		break;
	}

	m_threadCmd = CMD_NONE;
	return ok;
}

bool QSerialPortThread::_openSerial()
{
	if (m_serialPort->isOpen())
		return true;

	bool ok = true;

	m_serialPort->setPortName(m_portInfo.portName());
	ok &= m_serialPort->setBaudRate(115200);

	ok &= m_serialPort->open(QIODevice::ReadWrite);

	m_serialPort->setReadBufferSize(1024);

	if (!ok) {
		qWarning("Yadi: %s cannot open serial driver for port '%s'"
				 ,YadiDevice::threadNameAsc()
				 ,m_portInfo.portName().toLocal8Bit().constData());

		m_threadCmd = CMD_STOP_ALL;
	}
	else {
		m_serialPort->write("v\n");
		m_serialPort->waitForBytesWritten(100);
		m_serialPort->write("c\n");
		m_serialPort->waitForBytesWritten(100);
	}
	return ok;
}

void QSerialPortThread::_closeSerial()
{
	if (m_serialPort->isOpen())
		m_serialPort->close();
}

bool QSerialPortThread::_openOutput()
{
	if (!_openSerial())
		return false;

	m_isOutputOpen = true;
	return true;
}

void QSerialPortThread::_closeOutput()
{
	m_isOutputOpen = false;

	// process send data queue a last time (could be interface configuration)
	_processSendQueue();


	if (!m_isInputOpen)
		_closeSerial();
}

bool QSerialPortThread::_openInput()
{
	if (!_openSerial())
		return false;

	m_isInputOpen = true;
	return true;
}

void QSerialPortThread::_closeInput()
{
	m_isInputOpen = false;

	if (m_isOutputOpen)
		_closeSerial();
}

bool QSerialPortThread::_processSendQueue()
{
	bool ok = true;

	while (m_sendDataListHasData == true) {
		// Get oldest entry from send data queue
		QByteArray data;
		m_accessMutex.lock();
		if (!m_sendDataList.isEmpty()) {
			data = m_sendDataList.takeFirst();
		} else { // this should not happen, but we handle this anyway
			m_sendDataListHasData = false;
		}
		m_accessMutex.unlock();

		if (data.size()) {
			bool isbin = data.contains(char(0));

			if (m_yadiDev->debug > 1) {
				if (!isbin)
					qDebug("Yadi: %s write serial: %s (queued: %d)"
						   ,YadiDevice::threadNameAsc(),data.left(data.size()-1).constData(),m_sendDataList.size());
				else
					qDebug("Yadi: %s write serial: %s",YadiDevice::threadNameAsc(),data.toHex().constData());
			}

			qint64 written = m_serialPort->write(data);
			if (!isbin)
				m_serialPort->flush();
			if (written != data.size())
				qWarning("Yadi: %s serial wrote only %lli of %d",YadiDevice::threadNameAsc(),written,data.size());
		}

		// check if queue is empty and reset the flag if so
		m_accessMutex.lock();
		if (m_sendDataList.isEmpty())
			m_sendDataListHasData = false;
		m_accessMutex.unlock();
	}

	m_serialPort->flush();

	return ok;
}

void QSerialPortThread::_clearSendQueue()
{
	m_accessMutex.lock();
	m_sendDataList.clear();
	m_sendDataListHasData = false;
	m_accessMutex.unlock();
}

const char *QSerialPortThread::ascCmd(int cmdno)
{
	return _asc_cmds[cmdno];
}

void QSerialPortThread::processDmxDataLine()
{
	if (m_dmxInLine.isEmpty())
		return;

	if (m_dmxInLine.startsWith(' ')) {
		m_dmxInLine.remove(0,1);
		qDebug() << "Yadi: command answer:" << m_dmxInLine;
	}
	else {
		if (m_dmxInLine.size() >3) {
			int cnt = m_dmxInLine.left(3).toInt();
			m_dmxInLine.remove(0,3);
			if (cnt > m_dmxInLine.size()) {
				cnt = m_dmxInLine.size();
			}
			for (int t=0; t<cnt; t++) {
				char val = m_dmxInLine.at(t);
				if (m_yadiDev->inUniverse[t] != val || m_emitAllInputData) {
					m_yadiDev->inUniverse[t] = val;
					emit dmxInDeviceChannelChanged(m_yadiDev->inUniverseNumber, m_yadiDev->inputId, t, uchar(val));
					emit dmxInChannelChanged(t, uchar(val));
				}
			}
			m_emitAllInputData = false;
			if (cnt > 0)
				onCompleteFrameReceived(cnt);
		}
	}

	m_dmxInLine.clear();
}

void QSerialPortThread::onError(const QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::NoError) {
		qDebug() << "Yadi: QSerialPort signal received" << error;
		return;
	}

	// qWarning() << "Yadi: error occured:" << error << m_serialPort->errorString();
}

void QSerialPortThread::onDataReceived()
{
	if (!m_isInThreadLoop)
		return;

	while (int cnt = m_serialPort->bytesAvailable()) {
		QByteArray dat = m_serialPort->read(cnt);
		for (int i=0; i<dat.size(); i++) {
			char c = dat.at(i);
			if (m_waitForAtChar) {
				if (c == '@') { // found begin of data sequence
					m_waitForAtChar = false;
					m_dmxInLine.clear();
				}
			}
			else {
				if (c == '\n') { // found line end (most likely a command answer
					processDmxDataLine();
					m_waitForAtChar = true;
				}
				else if (c == '@') { // found a new data begin sequence
					processDmxDataLine();
					m_dmxInLine.clear();
				}
				else {
					m_dmxInLine.append(c);
				}
			}
		}
	}
}

void QSerialPortThread::onCompleteFrameReceived(int rxDataSize)
{
	m_inputDmxFrameCnt++;

	int framenanos = m_time.nsecsElapsed();
	m_time.start();
	double framemillis = double(framenanos)/1000000;
	m_frameRateAvg->append(framemillis);

	QString update = QString("DMX input frame rate: %1 fps (Frame count: %5, frame duration: %2 ms, data size: %3, detected dmx frame size: %4)")
			.arg(1000.0f/ m_frameRateAvg->value(),2,'f',1,QChar('0'))
			.arg(m_frameRateAvg->value(),2,'f',1,QChar('0'))
			.arg(rxDataSize)
			.arg(m_inputDmxFrameSize)
			.arg(m_inputDmxFrameCnt);
	emit dmxPacketReceived(m_yadiDev, update);
}

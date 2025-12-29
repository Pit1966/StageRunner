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

#ifndef SERIALWRAPPER_H
#define SERIALWRAPPER_H

#include <QObject>


#ifdef Q_OS_WIN32
// This is a workaround for compiler BUG with MSVC and Qt5
#define NOMINMAX

#include <windows.h>
#include <stdio.h>
#endif

#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#endif

#ifdef HAS_QTSERIAL
#include <QtSerialPort>
#endif

class YadiDevice;
class QThread;

class SerialWrapper : public QObject
{
	Q_OBJECT
private:
	QString device_node;			// may be port name for QtSerial, or device path for unix
	QString m_productName;			// product name info from USB enumerator
	int error_num;
	YadiDevice *m_yadi;
	QThread *m_openedFromThread;

#if defined (USE_QTSERIAL)
	QSerialPort *m_serialPort;
	QSerialPortInfo m_serialInfo;
#elif defined(Q_OS_WIN32)
	HANDLE serial_handle;
#elif defined(Q_OS_UNIX)	// this includes LINUX and MAC OSX
	int serial_fd;
#endif

public:
	SerialWrapper(YadiDevice *dev, const QString & dev_node = "");
	~SerialWrapper();
	static bool deviceNodeExists(const QString &dev_node);
#ifdef HAS_QTSERIAL
	static QList<QSerialPortInfo> discoverQtSerialPorts(const QString &nameMatch);		// todo move to Manager
#endif
	void setProductName(const QString &productName) {m_productName = productName;}
	bool openSerial();
	void closeSerial();
	QByteArray readSerial(qint64 size);
	qint64 readSerial(char *buf, qint64 size);
	qint64 writeSerial(const char * buf);
	qint64 writeSerial(const char * buf, qint64 size);
	int error();
	QString errorString();
	bool isOpen();
	const QString & deviceNode() {return device_node;}

	bool writeCommand(const QByteArray cmd, QByteArray *serAnswer = nullptr);

signals:

public slots:

};

#endif // SERIALWRAPPER_H

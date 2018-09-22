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
	static QList<QSerialPortInfo> discoverQtSerialPorts(const QString &nameMatch);
#endif

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

	bool writeCommand(const QByteArray cmd);

signals:

public slots:

};

#endif // SERIALWRAPPER_H

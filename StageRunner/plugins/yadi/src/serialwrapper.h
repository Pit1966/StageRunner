#ifndef SERIALWRAPPER_H
#define SERIALWRAPPER_H

#include <QObject>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#endif

class SerialWrapper : public QObject
{
	Q_OBJECT
private:
	QString device_node;
	int error_num;

#if defined(WIN32)
	HANDLE serial_handle;
#elif defined(unix)
	int serial_fd;
#endif

public:
	SerialWrapper(const QString & dev_node = "");
	~SerialWrapper();
	static bool deviceNodeExists(const QString &dev_node);

	bool openSerial(const QString & dev_node = "");
	void closeSerial();
	QByteArray readSerial(qint64 size);
	qint64 readSerial(char *buf, qint64 size);
	qint64 writeSerial(const char * buf);
	qint64 writeSerial(const char * buf, qint64 size);
	int error();
	QString errorString();
	bool isOpen();
	const QString & deviceNode() {return device_node;}

signals:

public slots:

};

#endif // SERIALWRAPPER_H

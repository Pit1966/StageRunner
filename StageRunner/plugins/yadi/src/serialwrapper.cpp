#include "serialwrapper.h"
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QFile>

SerialWrapper::SerialWrapper(const QString &dev_node) :
	QObject()
{
	device_node = dev_node;
	error_num = 0;
#if defined(WIN32)
	serial_handle = 0;
#elif defined(unix)
	serial_fd = 0;
#endif
}

SerialWrapper::~SerialWrapper()
{
#if defined(WIN32)
	if (serial_handle > 0) {
		CloseHandle(serial_handle);
	}
#elif defined(unix)
	if (serial_fd) {
		close(serial_fd);
	}
#endif
}

bool SerialWrapper::deviceNodeExists(const QString &dev_node)
{
#if defined(WIN32)


#elif defined(unix)
	if (dev_node.size() && QFile::exists(dev_node)) {
		return true;
	} else {
		return false;
	}
#endif
}

bool SerialWrapper::openSerial(const QString &dev_node)
{
	if (dev_node.size()) {
		device_node = dev_node;
	}
	if (!device_node.size()) {
		return false;
	}

	bool ok = false;

#if defined(WIN32)
	close();

	serial_handle = CreateFileA( device_name.toLocal8Bit().data(),
							   GENERIC_READ | GENERIC_WRITE,
							   0,
							   0,
							   OPEN_EXISTING,
							   0,
							   0 );
	if (serial_handle != INVALID_HANDLE_VALUE) {
		ok = true;
		error_num = 0;
	} else {
		error_num = GetLastError();
	}

#elif defined(unix)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}

	serial_fd = open(device_node.toLocal8Bit().data(), O_RDWR | O_NONBLOCK);
	if (serial_fd > 0) {
		ok = true;
		qDebug() << device_node << "opened.";
	} else {
		ok = false;
		qDebug() << "Device open failed for" << device_node
					<< strerror(errno);
	}

#endif


	return ok;
}

void SerialWrapper::closeSerial()
{
#if defined(WIN32)
	if (serial_handle) {
		CloseHandle(serial_handle);
		serial_handle = 0;
	}

#elif defined(unix)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}
#endif
}

QByteArray SerialWrapper::readSerial(qint64 size)
{
	QByteArray in;
#if defined(WIN32)
	char readbuf[600];
	DWORD bytes_read;
	if (ReadFile(serial_handle, readbuf, size, &bytes_read, 0)) {
		in.resize(bytes_read);
		char *in_char = in.data();
		for (uint t=0; t<bytes_read; t++) {
			in_char[t] = readbuf[t];
		}
	}
#elif defined(unix)
	char readbuf[600];
	ssize_t bytes_read = read(serial_fd, readbuf, size);
	if (bytes_read > 0) {
		in.resize(bytes_read);
		char *in_char = in.data();
		for (uint t=0; t<bytes_read; t++) {
			in_char[t] = readbuf[t];
		}
		error_num = 0;
	}
	else if (bytes_read < 0) {
		error_num = errno;
	}
	else {
		error_num = 0;
	}
#endif

	return in;
}

qint64 SerialWrapper::readSerial(char *buf, qint64 size)
{
#if defined(WIN32)
	DWORD bytes_read;
	if (ReadFile(serial_handle, buf, size, &bytes_read, 0)) {
		return bytes_read;
	} else {
		return -1;
	}
#elif defined(unix)
	return read(serial_fd, buf, size);
#endif
}

qint64 SerialWrapper::writeSerial(const char *buf)
{
#if defined(WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = (DWORD)strlen(buf);
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
#elif defined(unix)
	qint64 num = write(serial_fd, buf, strlen(buf));
	if (num < 0) {
		error_num = errno;
	} else {
		error_num = 0;
	}
#endif
	return num;
}

qint64 SerialWrapper::writeSerial(const char *buf, qint64 size)
{
#if defined(WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = size;
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
#elif defined(unix)
	qint64 num = write(serial_fd, buf, size);
	if (num < 0) {
		error_num = errno;
	} else {
		error_num = 0;
	}
#endif
	return num;
}

int SerialWrapper::error()
{
	return error_num;
}

QString SerialWrapper::errorString()
{
#if defined(unix)
	return strerror(error_num);
#elif defined(WIN32)
	return QString::number(error_num);
#endif
}

bool SerialWrapper::isOpen()
{
	bool open = false;
#if defined(WIN32)
	if (serial_handle > 0) {
		open = true;
	}
#elif defined(unix)
	if (serial_fd > 0) {
		open = true;
	}
#endif
	return open;
}

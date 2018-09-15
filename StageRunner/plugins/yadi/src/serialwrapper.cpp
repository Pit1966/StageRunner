#include "serialwrapper.h"
#include "yadidevice.h"

#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QFile>

SerialWrapper::SerialWrapper(YadiDevice *dev, const QString &dev_node) :
	QObject()
  , m_yadi(dev)
  , m_openedFromThread(nullptr)
{
	device_node = dev_node;
	error_num = 0;
#if defined(QTSERIAL)
	m_serialPort = nullptr;
#elif defined(WIN32)
	serial_handle = 0;
	if (dev_node.isEmpty()) {
		device_node = "com3";
	}
#elif defined(__unix__)
	serial_fd = 0;
#endif
}

SerialWrapper::~SerialWrapper()
{
#if defined(QTSERIAL)
	if (m_serialPort) {
		m_serialPort->close();
		delete m_serialPort;
	}
#elif defined(WIN32)
	if (serial_handle > 0) {
		CloseHandle(serial_handle);
	}
#elif defined(__unix__)
	if (serial_fd) {
		close(serial_fd);
	}
#endif
}

bool SerialWrapper::deviceNodeExists(const QString &dev_node)
{
#if defined(QTSERIAL)
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		if (info.portName() == dev_node)
			return true;
	}

#elif defined(WIN32)
	return true;

#elif defined(__unix__)
	if (dev_node.size() && QFile::exists(dev_node)) {
		return true;
	} else {
		return false;
	}
#endif

	return false;
}

#ifdef QTSERIAL
/**
 * @brief Return a list of devices, where the product name starts with the given string
 * @param productMatch
 * @return
 */
QList<QSerialPortInfo> SerialWrapper::discoverQtSerialPorts(const QString &nameMatch)
{
	QList<QSerialPortInfo> matchDevs;

	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		QString serial(info.serialNumber());
		QString name(info.description());
		QString vendor(info.manufacturer());
		QString productID(info.productIdentifier());

		qDebug() << "[QtSerialInterface] Serial: " << serial << "name:"
				 << name << "vendor:" << vendor << "productID" << productID;

#if defined(Q_OS_OSX)
		/* Qt 5.6+ reports the same device as "cu" and "tty". Only the first will be considered */
		if (info.portName().startsWith("cu"))
			continue;
#endif

		if (name.startsWith(nameMatch))
			matchDevs.append(info);

	}

	if (matchDevs.isEmpty())
		qDebug() << "[QtSerialInterface] Serial: no serial devices discovered";

	return matchDevs;
}
#endif


bool SerialWrapper::openSerial(const QString &dev_node)
{
	if (dev_node.size()) {
		device_node = dev_node;
	}
	if (!device_node.size()) {
		return false;
	}

	bool ok = false;

	m_openedFromThread = QThread::currentThread();

#if defined(QTSERIAL)
	if (m_serialPort) {
		m_serialPort->close();

		qDebug("Yadi: %s: SerialWrapper::openSerial reopen '%s'"
			   ,YadiDevice::threadNameAsc()
			   ,device_node.toLocal8Bit().constData());

	} else {
		m_serialPort = new QSerialPort(m_serialInfo);

		qDebug("Yadi: %s: SerialWrapper::openSerial init '%s'"
			   ,YadiDevice::threadNameAsc()
			   ,device_node.toLocal8Bit().constData());
	}


	m_serialPort->setPortName(device_node);
	m_serialPort->setBaudRate(115200);

	if (m_serialPort->open(QIODevice::ReadWrite) == false) {
		qWarning("Yadi: %s cannot open serial driver '%s'"
				 ,YadiDevice::threadNameAsc()
				 ,device_node.toLocal8Bit().constData());
		delete m_serialPort;
		m_serialPort = nullptr;
		return false;
	}

	m_serialPort->setReadBufferSize(1024);
	qDebug() << "Yadi: Read buffer size:" << m_serialPort->readBufferSize() << m_serialPort->error() << m_serialPort->errorString();

	return true;

#elif defined(WIN32)
	if (serial_handle) {
		CloseHandle(serial_handle);
	}
	COMMTIMEOUTS ct;

	serial_handle = CreateFileA( device_node.toLocal8Bit().data(),
							   GENERIC_READ | GENERIC_WRITE,
							   0,
							   0,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_DEVICE,
							   0 );
	if (serial_handle != INVALID_HANDLE_VALUE) {
		ok = true;

		SetupComm(serial_handle, 1024, 1024);

		// Set Timeout for nonblocking access to serial device
		memset(&ct,0,sizeof(COMMTIMEOUTS));
		ct.ReadIntervalTimeout = MAXDWORD;
		ct.ReadTotalTimeoutMultiplier = 0;
		ct.ReadTotalTimeoutConstant = 0;
		ct.WriteTotalTimeoutMultiplier = 10;
		ct.WriteTotalTimeoutConstant = 5;
		SetCommTimeouts(serial_handle,&ct);

		error_num = 0;
	} else {
		error_num = GetLastError();
	}

#elif defined(__unix__)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}

	serial_fd = open(device_node.toLocal8Bit().data(), O_RDWR | O_NONBLOCK);
	if (serial_fd > 0) {
		ok = true;
		qDebug("Yadi: %s opened",deviceNode().toLocal8Bit().data());
	} else {
		ok = false;
		qDebug("Yadi: Device open failed for %s (%s)"
			   ,deviceNode().toLocal8Bit().data(),strerror(errno));
	}

#endif


	return ok;
}

void SerialWrapper::closeSerial()
{
	qDebug("Yadi: %s: close serial interface for device '%s'"
		   ,YadiDevice::threadNameAsc()
		   ,device_node.toLocal8Bit().constData());
#if defined(QTSERIAL)
	if (m_serialPort) {
		m_serialPort->close();
	}
#elif defined(WIN32)
	if (serial_handle) {
		CloseHandle(serial_handle);
		serial_handle = 0;
	}

#elif defined(__unix__)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}
#endif
}

QByteArray SerialWrapper::readSerial(qint64 size)
{
	QByteArray in;
#if defined(QTSERIAL)
	in = m_serialPort->read(size);
	if (in.size() > 0) {
		error_num = 0;
	}
	else if (m_serialPort->error()) {
		error_num = m_serialPort->error();
	}
	else {
		error_num = 0;
	}

#elif defined(WIN32)
	char readbuf[600];
	DWORD bytes_read;
	if (ReadFile(serial_handle, readbuf, size, &bytes_read, 0)) {
		in.resize(bytes_read);
		char *in_char = in.data();
		for (uint t=0; t<bytes_read; t++) {
			in_char[t] = readbuf[t];
		}
	}
#elif defined(__unix__)
	char readbuf[600];
	ssize_t bytes_read = read(serial_fd, readbuf, size_t(size));
	if (bytes_read > 0) {
		in.resize(int(bytes_read));
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
#if defined(QTSERIAL)
	return m_serialPort->read(buf, size);
#elif defined(WIN32)
	DWORD bytes_read;
	if (ReadFile(serial_handle, buf, size, &bytes_read, 0)) {
		return bytes_read;
	} else {
		return -1;
	}
#elif defined(__unix__)
	return read(serial_fd, buf, size_t(size));
#endif
}

qint64 SerialWrapper::writeSerial(const char *buf)
{
	if (m_yadi->debug > 2)
		qDebug("Yadi: %s write serial: %s"
			   ,YadiDevice::threadNameAsc()
			   ,buf);

#if defined(QTSERIAL)
	qint64 num = m_serialPort->write(buf);
#elif defined(WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = (DWORD)strlen(buf);
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
#elif defined(__unix__)
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
	if (m_yadi->debug > 2)
		qDebug("Yadi: %s write serial: %s size %lli"
			   ,YadiDevice::threadNameAsc()
			   ,buf
			   ,size);

#if defined(QTSERIAL)
	qint64 num = m_serialPort->write(buf, size);
#elif defined(WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = size;
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
	return num;
#elif defined(__unix__)
	qint64 num = write(serial_fd, buf, size_t(size));
	if (num < 0) {
		error_num = errno;
	} else {
		error_num = 0;
	}
	return num;
#endif

	return num;
}

int SerialWrapper::error()
{
	return error_num;
}

QString SerialWrapper::errorString()
{
#if defined(QTSERIAL)
	if (m_serialPort)
		return m_serialPort->errorString();
	return "Error: Serial port not open";
#elif defined(__unix__)
	return strerror(error_num);
#elif defined(WIN32)
	return QString::number(error_num);
#endif
}

bool SerialWrapper::isOpen()
{
	bool open = false;
#if defined(QTSERIAL)
	if (m_serialPort && m_serialPort->isOpen())
		open = true;
#elif defined(WIN32)
	if (serial_handle) {
		open = true;
	}
#elif defined(__unix__)
	if (serial_fd > 0) {
		open = true;
	}
#endif
	return open;
}

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
#if defined(USE_QTSERIAL)
	m_serialPort = nullptr;
#elif defined(Q_OS_WIN32)
	serial_handle = 0;
	if (dev_node.isEmpty()) {
		device_node = "com3";
	}
#elif defined(Q_OS_UNIX)
	serial_fd = 0;
#endif
}

SerialWrapper::~SerialWrapper()
{
#if defined(USE_QTSERIAL)
	if (m_serialPort) {
		m_serialPort->close();
		delete m_serialPort;
	}
#elif defined(Q_OS_WIN32)
	if (serial_handle > 0) {
		CloseHandle(serial_handle);
	}
#elif defined(Q_OS_UNIX)
	if (serial_fd) {
		close(serial_fd);
	}
#endif
}

bool SerialWrapper::deviceNodeExists(const QString &dev_node)
{
#if defined(USE_QTSERIAL)
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		if (info.portName() == dev_node)
			return true;
	}

#elif defined(Q_OS_WIN32)
	return true;

#elif defined(Q_OS_UNIX)
	if (dev_node.size() && QFile::exists(dev_node)) {
		return true;
	} else {
		return false;
	}
#endif

	return false;
}

#ifdef HAS_QTSERIAL
/**
 * @brief Return a list of devices, where the product name starts with the given string
 * @param productMatch
 * @return
 */
QList<QSerialPortInfo> SerialWrapper::discoverQtSerialPorts(const QString &nameMatch)
{
	QList<QSerialPortInfo> matchDevs;

	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		QString serial(info.serialNumber());
		QString name(info.description());
		QString vendor(info.manufacturer());
		QString productID(info.productIdentifier());


#if defined(Q_OS_OSX)
		/* Qt 5.6+ reports the same device as "cu" and "tty". Only the first will be considered */
		if (info.portName().startsWith("cu"))
			continue;
#endif

		qDebug() << "Yadi: [QtSerialInterface] Serial: " << serial << "name:"
				 << name << "vendor:" << vendor << "productID" << productID;

		if (name.startsWith(nameMatch))
			matchDevs.append(info);

	}

	if (matchDevs.isEmpty())
		qDebug() << "Yadi: [QtSerialInterface] Serial: no serial devices discovered";

	return matchDevs;
}
#endif


bool SerialWrapper::openSerial()
{
	if (!device_node.size()) {
		return false;
	}

	bool ok = false;

#if defined(USE_QTSERIAL)
	if (m_serialPort) {
		qWarning("Yadi: %s: SerialWrapper::openSerial: Serial port already open '%s'"
			   ,YadiDevice::threadNameAsc()
			   ,device_node.toLocal8Bit().constData());
		return false;

	}

	m_serialPort = new QSerialPort(m_serialInfo);
	m_openedFromThread = QThread::currentThread();

	qDebug("Yadi: %s: SerialWrapper::openSerial: Init serial port '%s'"
		   ,YadiDevice::threadNameAsc()
		   ,device_node.toLocal8Bit().constData());


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
	if (m_yadi->debug)
		qDebug() << "Yadi: Read buffer size:" << m_serialPort->readBufferSize() << m_serialPort->error() << m_serialPort->errorString();

	return true;

#elif defined(Q_OS_WIN32)
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

#elif defined(Q_OS_UNIX)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}


//	   fd = open( argv[1], O_RDONLY | O_NOCTTY | O_NONBLOCK );

//	   for(;;) {
//		   len = read( fd, &buf[0], 8192 );
//		   if( len > 0 ) write(1,buf,len);
//		   sleep(1);
//	   }


	serial_fd = open(device_node.toLocal8Bit().data(), O_RDWR | O_NONBLOCK);
	if (serial_fd > 0) {
		ok = true;
		qDebug("Yadi: %s opened",deviceNode().toLocal8Bit().data());

		struct termios tio;
		tcgetattr(serial_fd,&tio);
		cfmakeraw(&tio);
		tio.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXANY | IXOFF);
		tio.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
		tio.c_cflag |= CLOCAL;					// this is necessary for mac when using tty.usbModem instead of cu.usbModem
		cfsetispeed(&tio,B115200);
		cfsetospeed(&tio,B115200);
		tcsetattr(serial_fd,TCSANOW,&tio);   // TCSANOW

#ifdef DEBUGME
		QProcess stty;
//		QString cmd = QString("stty -F %1 115200 raw -echo -echoe").arg(globYadiDeviceList.at(t)->devNodePath);
#ifdef Q_OS_MAC
		QString cmd = QString("stty -f %1").arg(deviceNode());
#else
		QString cmd = QString("stty -F %1").arg(deviceNode());
#endif
		stty.start(cmd);
		stty.waitForFinished(2000);
		// qDebug() <<  "QProcess" << cmd;
		QString out =  QString::fromUtf8(stty.readAllStandardOutput());
		QString err =  QString::fromUtf8(stty.readAllStandardError().data());
		qDebug("stty:\n%s\nerr:\n%s\n",out.toLocal8Bit().constData(),err.toLocal8Bit().constData());
#endif

		// Read Version info
		writeCommand("v");

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

#if defined(USE_QTSERIAL)
	if (m_serialPort) {
		m_serialPort->close();
		delete m_serialPort;
		m_serialPort = nullptr;
	}
#elif defined(Q_OS_WIN32)
	if (serial_handle) {
		CloseHandle(serial_handle);
		serial_handle = 0;
	}

#elif defined(Q_OS_UNIX)
	if (serial_fd > 0) {
		close(serial_fd);
		serial_fd = 0;
	}
#endif
}

QByteArray SerialWrapper::readSerial(qint64 size)
{
	QByteArray in;
#if defined(USE_QTSERIAL)
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

#elif defined(Q_OS_WIN32)
	char readbuf[600];
	DWORD bytes_read;
	if (ReadFile(serial_handle, readbuf, size, &bytes_read, 0)) {
		in.resize(bytes_read);
		char *in_char = in.data();
		for (uint t=0; t<bytes_read; t++) {
			in_char[t] = readbuf[t];
		}
	}
#elif defined(Q_OS_UNIX)
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
	if (!isOpen())
		if (!openSerial())
			return 0;

#if defined(USE_QTSERIAL)
	return m_serialPort->read(buf, size);
#elif defined(Q_OS_WIN32)
	DWORD bytes_read;
	if (ReadFile(serial_handle, buf, size, &bytes_read, 0)) {
		return bytes_read;
	} else {
		return -1;
	}
#elif defined(Q_OS_UNIX)
	return read(serial_fd, buf, size_t(size));
#endif
}

qint64 SerialWrapper::writeSerial(const char *buf)
{
	if (m_yadi->debug > 2)
		qDebug("Yadi: %s write serial: %s"
			   ,YadiDevice::threadNameAsc()
			   ,buf);

	if (!isOpen())
		if (!openSerial())
			return 0;

#if defined(USE_QTSERIAL)
	qint64 num = m_serialPort->write(buf);
#elif defined(Q_OS_WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = (DWORD)strlen(buf);
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
#elif defined(Q_OS_UNIX)
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

	if (!isOpen())
		if (!openSerial())
			return 0;

#if defined(USE_QTSERIAL)
	qint64 num = m_serialPort->write(buf, size);
#elif defined(Q_OS_WIN32)
	DWORD num = 0;
	DWORD bytes_to_write = size;
	if (! WriteFile(serial_handle,buf,bytes_to_write,&num,0)) {
		error_num = GetLastError();
		num = -1;
	} else {
		error_num = 0;
	}
	return num;
#elif defined(Q_OS_UNIX)
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
#if defined(USE_QTSERIAL)
	if (m_serialPort)
		return m_serialPort->errorString();
	return "Error: Serial port not open";
#elif defined(Q_OS_UNIX)
	return strerror(error_num);
#elif defined(WIN32)
	return QString::number(error_num);
#endif
}

bool SerialWrapper::isOpen()
{
	bool open = false;
#if defined(USE_QTSERIAL)
	if (m_serialPort && m_serialPort->isOpen())
		open = true;
#elif defined(Q_OS_WIN32)
	if (serial_handle) {
		open = true;
	}
#elif defined(Q_OS_UNIX)
	if (serial_fd > 0) {
		open = true;
	}
#endif
	return open;
}

bool SerialWrapper::writeCommand(const QByteArray cmd)
{
#if defined(USE_QTSERIAL)

#elif defined(Q_OS_WIN32)

#elif defined(Q_OS_UNIX)
	if (serial_fd == 0)
		return false;

	int cmdsize = cmd.size();
	int byteswritten = writeSerial(cmd.constData(), cmdsize);
	if (byteswritten != cmdsize) {
		qWarning() << "Could not write serial command: " << strerror(errno);
		qDebug("error: %s",strerror(errno));
		return false;
	}

	//tcdrain(serial_fd); // ??
	//tcflush(serial_fd, TCIOFLUSH);

	usleep(20000);

	QByteArray answer;

	// wait for answer:
	char buf[8];
	int bytes = 0;
	do {
		bytes = read(serial_fd, buf, 1);
		if (bytes)
			answer.append(buf[0]);
	} while (bytes > 0);

	qDebug() << "Serialcommand" << cmd << answer;

#endif

	return true;
}

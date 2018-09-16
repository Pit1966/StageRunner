#include "qserialportthread.h"
#include "yadidevice.h"
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
{
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
		if (info.portName() == dev->devNodePath) {
			m_portInfo = info;
			break;
		}
	}
	setObjectName("QSerialPortThread");
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
		return true;
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

	qDebug("Yadi: Thread command %s executed after %dms",ascCmd(cmd),time.elapsed());

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
	qDebug() << "Yadi: Thread started";

	m_serialPort = new QSerialPort(m_portInfo);

	while (m_threadCmd != CMD_STOP_ALL && m_threadCmd != CMD_STOP_ERROR) {
		// check for thread command and execute if there is one
		if (m_threadCmd > CMD_NONE)
			_executeCurrentCommand();

		// check serial send data queue
		if (m_isOutputOpen && m_sendDataListHasData == true)
			_processSendQueue();


		// Sleep a little bit, commands should not arrive that fast
		msleep(10);
	}


	_closeOutput();
	_closeInput();

	delete m_serialPort;
	m_serialPort = nullptr;
	m_threadCmd = CMD_NONE;

	qDebug() << "Yadi: Thread finished";
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
//			if (m_yadiDev->debug > 2)
			if (!data.contains(char(0)))
				qDebug("Yadi: %s write serial: %s",YadiDevice::threadNameAsc(),data.left(data.size()-1).constData());
			else
				qDebug("Yadi: %s write serial: %s",YadiDevice::threadNameAsc(),data.toHex().constData());

			qint64 written = m_serialPort->write(data);
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

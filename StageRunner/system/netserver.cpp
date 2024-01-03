#include "netserver.h"
#include "appcontrol/appcentral.h"
#include "log.h"

#include <QNetworkDatagram>
#include <QJsonDocument>
#include <QNetworkInterface>

class AppCentral;

NetEntity::NetEntity(QTcpSocket *client, const QString &ident)
	: clientSocket(client)
	, ident(ident)
{
}

QString NetEntity::ip4String() const
{
	if (hostAddr.isNull())
		return QString();

	QString str = hostAddr.toString();

	return str;
}

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------


void NetEntityHash::clear()
{
	m_instHash.clear();
}

bool NetEntityHash::add(const NetEntity &entity)
{
	if (m_instHash.contains(entity.ident))
		return false;

	m_instHash.insert(entity.ident, entity);
	return true;
}

bool NetEntityHash::contains(const QString &ident)
{
	return m_instHash.contains(ident);
}

NetEntity &NetEntityHash::get(const QString &ident)
{
	static NetEntity dummy;
	if (!m_instHash.contains(ident))
		return dummy;

	return m_instHash[ident];
}

QStringList NetEntityHash::identList(bool sort) const
{
	QStringList list = m_instHash.keys();
	if (sort)
		std::sort(list.begin(), list.end());

	return list;
}

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

void TcpClientHash::clear()
{
	m_clientHash.clear();
}

bool TcpClientHash::add(const NetEntity &entity)
{
	if (m_clientHash.contains(entity.clientSocket))
		return false;

	m_clientHash.insert(entity.clientSocket, entity);
	return true;
}

bool TcpClientHash::contains(QTcpSocket *client) const
{
	return m_clientHash.contains(client);
}

bool TcpClientHash::remove(QTcpSocket *client)
{
	if (!m_clientHash.contains(client))
		return false;

	m_clientHash.remove(client);
	return true;
}

NetEntity &TcpClientHash::get(QTcpSocket *client)
{
	static NetEntity dummy;
	if (!m_clientHash.contains(client))
		return dummy;

	return m_clientHash[client];
}

QStringList TcpClientHash::identList(bool sort) const
{
	QStringList list;
	for (const NetEntity &e : m_clientHash) {
		list.append(e.ident);
	}

	if (sort)
		std::sort(list.begin(), list.end());

	return list;}

QList<QTcpSocket *> TcpClientHash::clientList() const
{
	return m_clientHash.keys();
}

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

NetServer::NetServer(AppCentral &appcentral, QObject *parent)
	: QObject(parent)
	, 	m_myApp(appcentral)
{
}

NetServer::~NetServer()
{
	delete m_udpSocket;
	delete m_tcpServer;
}

bool NetServer::startUDPServer()
{
	if (!m_udpSocket)
		m_udpSocket = new QUdpSocket();

	if (m_isUDPBound)
		return true;

	m_isUDPBound = m_udpSocket->bind(/*QHostAddress("192.168.10.1"), */m_udpPort);
	m_localAdr = m_udpSocket->localAddress();
//	m_isBound = m_udpSocket->bind(QHostAddress::LocalHost, m_udpPort);
	if (m_isUDPBound) {
		connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingUDPDatagrams()));
		emit logMsgSent(tr("Net Server is running"));
		emit logMsgSent(tr("Listen on %1:%2 ").arg(m_localAdr.toString()).arg(m_udpPort));
		return true;
	}
	return false;
}

bool NetServer::startTCPServer()
{
	if (!m_tcpServer) {
		m_tcpServer = new QTcpServer(this);
		connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewTcpConnection()));
	}

	if (m_isTCPListening)
		return true;

	m_isTCPListening = m_tcpServer->listen(QHostAddress::Any, m_tcpPort);
	if (!m_isTCPListening) {
		LOGERROR(tr("Could not start tcp server on port %1: %2").arg(m_tcpPort).arg(m_tcpServer->errorString()));
		return false;
	}

	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			ipAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	LOGTEXT(tr("The tcp server is running on IP: %1, port: %2")
			.arg(ipAddress).arg(m_tcpServer->serverPort()));


	return true;
}

bool NetServer::isUDPRunning() const
{
	return m_isUDPBound;
}

bool NetServer::sendBroadcastHello(bool isReply)
{
	if (!isUDPRunning())
		return false;


	QJsonObject json;
	if (isReply) {
		json["cmd"] = "here";
	} else {
		json["cmd"] = "hello";
	}
	return sendUDPBroadcast(json);
}

void NetServer::readPendingUDPDatagrams()
{
	while (m_udpSocket->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(m_udpSocket->pendingDatagramSize());

		QHostAddress sender;
		quint16 senderPort;
		m_udpSocket->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);

		QString senderIp = sender.toString();
		if (senderIp.startsWith("::ffff")) {
			senderIp.remove(":");
			senderIp.remove("ffff");
		}

		QString data = QString::fromLocal8Bit(datagram.constData());
	}
}

void NetServer::onNewTcpConnection()
{
	QTcpSocket *client = m_tcpServer->nextPendingConnection();
	if (client) {
		NetEntity ne(client);
		ne.hostAddr = client->peerAddress();
		m_connectedClients.add(ne);

		connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
		connect(client, SIGNAL(readyRead()), this, SLOT(onTcpDataReceived()));

		LOGTEXT(tr("TCP connection from: %1").arg(ne.ip4String()));
	}
}

void NetServer::onClientDisconnected()
{
	QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
	if (!client) return;

	if (m_connectedClients.contains(client)) {
		NetEntity &ne = m_connectedClients.get(client);
		LOGTEXT(tr("TCP client disconnected: %1").arg(ne.ip4String()));

		m_connectedClients.remove(client);
	}
}

void NetServer::onTcpDataReceived()
{
	QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
	if (!client) return;

	QByteArray dat = client->readAll();
	QString cmd = QString::fromLocal8Bit(dat);
	LOGTEXT(tr("Execute received remote command: %1").arg(QString::fromUtf8(dat)));
	emit remoteCmdReceived(cmd);
}

bool NetServer::sendUDPBroadcast(const QJsonObject &json)
{
	QJsonObject j(json);
//	j["ident"] = m_myApp.getMyName();
//	j["ver"] = m_myApp.getVersion();
	j["msgid"] = ++m_msgCount;

	if (!j.contains("cmd")) {
		emit logMsgSent("Missing 'cmd' tag in json message");
		return false;
	}

	QByteArray jsontxt = QJsonDocument(j).toJson(QJsonDocument::Compact);
	int size = m_udpSocket->writeDatagram(jsontxt, QHostAddress::Broadcast, m_udpPort);

	if (size < jsontxt.size())
		return false;

	return true;
}

bool NetServer::sendUDP(const QHostAddress &host, const QJsonObject &json)
{
	QJsonObject j(json);
//	j["ident"] = m_myApp.getMyName();
//	j["ver"] = m_myApp.getVersion();
	j["msgid"] = ++m_msgCount;

	if (!j.contains("cmd")) {
		emit logMsgSent("Missing 'cmd' tag in json message");
		return false;
	}

	QByteArray jsontxt = QJsonDocument(j).toJson(QJsonDocument::Compact);
	int size = m_udpSocket->writeDatagram(jsontxt, host, m_udpPort);

	if (size < jsontxt.size())
		return false;

	return true;
}



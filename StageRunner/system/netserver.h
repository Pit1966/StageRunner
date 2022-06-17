#ifndef NETSERVER_H
#define NETSERVER_H

#include "config.h"


#include <QObject>
#include <QUdpSocket>
#include <QJsonObject>
#include <QHostAddress>
#include <QHash>
#include <QTcpServer>
#include <QTcpSocket>


class AppCentral;

class NetEntity
{
public:
	QTcpSocket *clientSocket;
	QHostAddress hostAddr;
	QString ident;
	bool syncEnabled	= false;

	NetEntity(QTcpSocket *client = nullptr, const QString &ident = QString());
	QString ip4String() const;
};

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

class NetEntityHash
{
private:
	QHash<QString, NetEntity> m_instHash;

public:
	void clear();
	bool add(const NetEntity &entity);
	bool contains(const QString &ident);
	NetEntity &get(const QString &ident);
	QStringList identList(bool sort = false) const;
	QHash<QString, NetEntity>::iterator begin() {return m_instHash.begin();}
	QHash<QString, NetEntity>::const_iterator begin() const {return m_instHash.begin();}
	QHash<QString, NetEntity>::iterator end() {return m_instHash.end();}
	QHash<QString, NetEntity>::const_iterator end() const {return m_instHash.end();}

};

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------

class TcpClientHash
{
private:
	QHash<QTcpSocket*, NetEntity> m_clientHash;

public:
	void clear();
	bool add(const NetEntity &entity);
	bool contains(QTcpSocket *client) const;
	bool remove(QTcpSocket *client);
	NetEntity &get(QTcpSocket *client);
	QStringList identList(bool sort = false) const;
	QList<QTcpSocket*> clientList() const;
	QHash<QTcpSocket*, NetEntity>::iterator begin() {return m_clientHash.begin();}
	QHash<QTcpSocket*, NetEntity>::const_iterator begin() const {return m_clientHash.begin();}
	QHash<QTcpSocket*, NetEntity>::iterator end() {return m_clientHash.end();}
	QHash<QTcpSocket*, NetEntity>::const_iterator end() const {return m_clientHash.end();}

};

//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------


class NetServer : public QObject
{
	Q_OBJECT
private:
	AppCentral &m_myApp;
	quint16 m_udpPort		= NET_UDP_PORT;
	quint16 m_tcpPort		= NET_TCP_PORT;
	int m_msgCount			= 0;
	QUdpSocket *m_udpSocket = nullptr;
	QTcpServer *m_tcpServer = nullptr;
	QHostAddress m_localAdr;
	bool m_isUDPBound		= false;
	bool m_isTCPListening	= false;

	TcpClientHash m_connectedClients;

public:
	explicit NetServer(AppCentral &appcentral, QObject *parent = nullptr);
	~NetServer();
	bool startUDPServer();
	bool startTCPServer();
	bool isUDPRunning() const;
	bool sendBroadcastHello(bool isReply = false);

private slots:
	void readPendingUDPDatagrams();

	void onNewTcpConnection();
	void onClientDisconnected();
	void onTcpDataReceived();

public slots:
	bool sendUDPBroadcast(const QJsonObject &json);
	bool sendUDP(const QHostAddress &host, const QJsonObject &json);

signals:
	void logMsgSent(const QString &msg);
	void netSlotEventReceived(const QString &txt);
	void jsonReceived(const QHostAddress &host, const QByteArray &dat);
	void remoteCmdReceived(const QString &cmd);

};

#endif // NETSERVER_H

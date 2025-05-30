/*
  Q Light Controller Plus
  artnetplugin.h

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef ARTNETPLUGIN_H
#define ARTNETPLUGIN_H

#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QString>
#include <QHash>
#include <QFile>

#include "qlcioplugin.h"
#include "artnetcontroller.h"

typedef struct
{
    QNetworkInterface interface;
    QNetworkAddressEntry address;
    ArtNetController* controller;
} ArtNetIO;

#define ARTNET_INPUTUNI "inputUni"
#define ARTNET_OUTPUTIP "outputIP"
#define ARTNET_OUTPUTUNI "outputUni"
#define ARTNET_TRANSMITMODE "transmitMode"
#define ARTNET_OUTPUT_ADD_IP "outputAddIP"

class ArtNetPlugin : public QLCIOPlugin
{
    Q_OBJECT
    Q_INTERFACES(QLCIOPlugin)
#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
	Q_PLUGIN_METADATA(IID "de.stonechip.stagerunner.artnet")
#endif

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:
    /** @reimp */
    virtual ~ArtNetPlugin();

    /** @reimp */
    void init();

    /** @reimp */
    QString name();

    /** @reimp */
    int capabilities() const;

    /** @reimp */
    QString pluginInfo();

private:
    bool requestLine(quint32 line, int retries);

    /*********************************************************************
     * Outputs
     *********************************************************************/
public:
    /** @reimp */
    bool openOutput(quint32 output, quint32 universe);

    /** @reimp */
    void closeOutput(quint32 output, quint32 universe);

    /** @reimp */
    QStringList outputs();

    /** @reimp */
    QString outputInfo(quint32 output);

    /** @reimp */
    void writeUniverse(quint32 universe, quint32 output, const QByteArray& data);

    /*************************************************************************
     * Inputs
     *************************************************************************/
public:
    /** @reimp */
    bool openInput(quint32 input, quint32 universe);

    /** @reimp */
    void closeInput(quint32 input, quint32 universe);

    /** @reimp */
    QStringList inputs();

    /** @reimp */
    QString inputInfo(quint32 input);

    /*********************************************************************
     * Configuration
     *********************************************************************/
public:
    /** @reimp */
    void configure();

    /** @reimp */
    bool canConfigure();

    /** @reimp */
    void setParameter(quint32 universe, quint32 line, Capability type, QString name, QVariant value);

    /** Get a list of the available Input/Output lines */
    QList<ArtNetIO> getIOMapping();

private:
    /** Map of the ArtNet plugin Input/Output lines */
    QList<ArtNetIO> m_IOmapping;

    /*********************************************************************
     * ArtNet socket
     *********************************************************************/
private:
    QSharedPointer<QUdpSocket> getUdpSocket();
private slots:
    void slotReadyRead();
private:
    void handlePacket(QByteArray const& datagram, QHostAddress const& senderAddress);
private:
    QWeakPointer<QUdpSocket> m_udpSocket;
};

#endif

/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef DMXRECEIVER_H
#define DMXRECEIVER_H

#include <QThread>
#include <QPair>
#include <QElapsedTimer>



class QFile;
class QTime;
class SerialWrapper;
class YadiDevice;
class MvgAvgCollector;

class DmxAnswer
{
public:
	enum DmxAnswerType {
		NONE,
		UNKNOWN,
		DMX_DATA,
		STATUS,
		REQ_CHANNELS,
		REQ_DMX_FRAMESIZE,
		REQ_VERSION
	};
	DmxAnswerType type;
	QByteArray answerBytes;
	QString answerString;
	bool rxDmxActive;
	quint8 dmxStatus;
	int dmxDataSize;
	int dmxFrameSize;
	int deviceMaxChannels;
	int deviceUsedChannels;

public:
	DmxAnswer() {
		type = NONE;
		init();
	}
	DmxAnswer(DmxAnswerType p_type) {
		type = p_type;
		init();
	}
private:
	void init() {
		rxDmxActive = false;
		dmxDataSize = 0;
		dmxFrameSize = 0;
		deviceMaxChannels = 0;
		deviceUsedChannels = 0;
		dmxStatus = 0;
	}

};


class YadiReceiver : public QThread
{
	Q_OBJECT
public:
	enum RunCmd {
		IDLE,
		RUNNING,
		STOPPED
	};

public:
	quint8 dmxStatus;

private:
	YadiDevice *device;
	QString m_deviceProductName;
	int inputNumber;

	volatile RunCmd cmd;
	QElapsedTimer m_time;
	MvgAvgCollector *m_frameRateAvg;

public:
	YadiReceiver(YadiDevice *p_device);
	~YadiReceiver();

	void setProductName(const QString &productName) {m_deviceProductName = productName;}
	bool startRxDmx(int input);
	bool stopRxDmx();
	bool detectRxDmxPacketSize(int *packet_size);
	bool detectRxDmxUniverseSize(int *max_universe_size, int *used_universe_size);

	bool waitForAtHeader(DmxAnswer & dmxstat);

private:
	void init();
	void run();
	bool receiver_loop();

signals:
	void exitReceiverWithFailure(int input);
	void dmxPacketReceived(YadiDevice *yadiDev, const QString &msg);
	void dmxInDeviceChannelChanged(quint32 universe, quint32 input, quint32 channel, uchar value);
	void dmxInChannelChanged(quint32 channel, uchar value);
	void rxDmxPacketSizeReceived(int channels);
	void dmxStatusReceived(quint8);
	void statusMsgSent(const QString &msg);
	void errorMsgSent(const QString &msg);

};

#endif // DMXRECEIVER_H

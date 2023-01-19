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

#include "yadireceiver.h"
#include "yadidevice.h"
#include "serialwrapper.h"
#include "mvgavgcollector.h"

#include <QDateTime>
#include <QDebug>
#include <QTime>
#include <QElapsedTimer>
#include <QMessageBox>

#define DMX_HEADER_SIZE 4

YadiReceiver::YadiReceiver(YadiDevice *p_device)
	: device(p_device)
{
	init();
}

YadiReceiver::~YadiReceiver()
{
	delete m_frameRateAvg;
}

void YadiReceiver::init()
{
	dmxStatus = 0;
	cmd = IDLE;
	inputNumber = 0;
	m_frameRateAvg = new MvgAvgCollector(40);
}

bool YadiReceiver::startRxDmx(int input)
{
	if (isRunning()) {
		qDebug() << "YadiReceiver::startRxDmx failed -> Thread already running";
		return false;
	}

	inputNumber = input;

	// Thread starten
	this->start();
	return true;
}


void YadiReceiver::run()
{
	cmd = RUNNING;

	bool ok = receiver_loop();

	if (!ok) {
		if (device->debug)
			qDebug("YadiReceiver::run: Final exit DMXreceiver thread with failure");
		emit exitReceiverWithFailure(inputNumber);
	} else {
		if (device->debug)
			qDebug("YadiReceiver::run: exit DMXreceiver thread normaly");
	}

	cmd = IDLE;
}

bool YadiReceiver::receiver_loop()
{
	bool ok = true;
	bool firstloop = true;

#ifdef USE_QTSERIAL
	while (cmd != STOPPED && !device->serialDev()->isOpen()) {
		msleep(10);
	}
#endif

	qDebug("Yadi: receiver loop enter");

	while (cmd != STOPPED && ok && device->serialDev() && device->serialDev()->isOpen()) {
		bool emit_all = true;

		int max_channels = -1;
		int used_channels = -1;
		ok = detectRxDmxUniverseSize(&max_channels, &used_channels);

		if (device->debug > 3)
			printf("max channels: %d, used channels: %d\n",max_channels, used_channels);

		int rx_dmx_packet_size = -1;
		ok = detectRxDmxPacketSize(&rx_dmx_packet_size);
		if (device->debug > 3)
			printf("  -> rx dmx packet size: %d",rx_dmx_packet_size);

		if (rx_dmx_packet_size > 0 && used_channels > 0) {
			dmxStatus |= 1;
		} else {
			msleep(100);
		}

		if (device->debug)
			printf("[ YadiReceiver ]: Enter inner loop\n");

		if (firstloop) {
			QString msg = tr("<font color=green>YADI started listener</font>: DMX packet size: %1, channels: used %2 / max %3")
					.arg(rx_dmx_packet_size - 1)
					.arg(used_channels)
					.arg(max_channels);
			emit statusMsgSent(msg);
		}

		bool dmxSizeReported = false;

		// Begin inner loop
		m_time.start();
		int transfer = 5;
		while (cmd != STOPPED && (dmxStatus&1) && ok) {

			DmxAnswer dmx;
			ok = waitForAtHeader(dmx);
			if (!ok) {
				qDebug() << "YadiReceiver::run: Wait for @-Header failed -> Leave inner receiver loop";
				// We could not receive rx DMX input data from interface.
				// This means that cable was disconnected or something similar
				// Not a fault so far.
				dmxStatus &= ~1;
				ok = true;
				// So we leave the receiver loop and check if the interface is still present.
				break;
			}

			if (dmx.type == DmxAnswer::DMX_DATA) {
				QByteArray in;
				QElapsedTimer wait;
				wait.start();

				qint64 channels_to_read = dmx.dmxDataSize;
				while (channels_to_read > 0 && wait.elapsed() < 1400 && cmd != STOPPED) {
					if (!device->serialDev())
						return false;
					in += device->serialDev()->readSerial(channels_to_read);
					channels_to_read = dmx.dmxDataSize - in.size();

				}
				// qDebug() << in.toHex();

				if (device->debug > 1 || in.size() != dmx.dmxDataSize)
					qDebug("YadiReceiver::run: read dmx data size: %d",in.size());
				if (!dmxSizeReported) {
					dmxSizeReported = true;
					QString msg = tr("<font color=green>YADI listener</font>: Detected DMX Frame size: %1")
							.arg(in.size());
					emit statusMsgSent(msg);
				}

				if (!device->serialDev())
					return false;

				if (device->serialDev()->error()) {
					qDebug() << "YadiReceiver: serial error:" << device->serialDev()->errorString();
					ok = false;
				} else {
					int framenanos = m_time.nsecsElapsed();
					double framemillis = double(framenanos)/1000000;
					m_time.start();
					m_frameRateAvg->append(framemillis);
					if (in.isEmpty()) {	// no data received
						// fprintf(stderr, "[ YadiReceiver ]: NULL frame: dmx data size: %d\n",dmx.dmxDataSize);
						msleep(2);
						continue;
					}

					QString update = QString("DMX frame rate: %1 fps (Frame duration: %2 ms, data size: %3, frame size: %4)")
							.arg(1000.0f/ m_frameRateAvg->value(),2,'f',1,QChar('0'))
							.arg(m_frameRateAvg->value(),2,'f',1,QChar('0'))
							.arg(dmx.dmxDataSize)
							.arg(rx_dmx_packet_size);
					emit dmxPacketReceived(device, update);
				}

				if (in.size() && !(transfer--)) {
					transfer = 0;
					int max_read = device->maxDeviceDmxInChannels;
					if (max_read > in.size())
						max_read = in.size();

					for (int t=0; t<max_read; t++) {
						char val = in.at(t);
						if (device->inUniverse[t] != val || emit_all) {
							device->inUniverse[t] = val;
							emit dmxInDeviceChannelChanged(device->inUniverseNumber,device->inputId,t,(uchar)val);
							emit dmxInChannelChanged(t,(uchar)val);
							// fprintf(stderr, "value changed: channel %d = %d\n",t,(uchar)val);
							// qDebug() << "Input channel" << t+1 << "=" << QString::number(uchar(val));
						}
					}
					if (device->debug > 3) qDebug() << "Line HEX in:" << in.toHex();
					emit_all = false;
				}
			}

		}
		// end inner loop

		firstloop = false;
	}
	return ok;
}



bool YadiReceiver::stopRxDmx()
{
	bool ok = true;
	if (isRunning()) {
		cmd = STOPPED;
		QElapsedTimer wait;
		wait.start();
		while (wait.elapsed() < 800) {;}

		if (isRunning()) {
			qDebug("YadiReceiver: stopRxDmx: Still running after 800ms");
			this->terminate();
			ok = false;
		}
	}
	return ok;
}



bool YadiReceiver::detectRxDmxPacketSize(int *packet_size)
{
	if (!device->serialDev())
		return false;

	if (!device->serialDev()->isOpen())
		return false;

	if ( device->serialDev()->writeSerial("c\n") <= 0 ) {
		qDebug("YadiReceiver: Device 'c' command failed");
		return false;
	}

	DmxAnswer stat(DmxAnswer::REQ_DMX_FRAMESIZE);
	bool ok = waitForAtHeader(stat);

	if (ok) {
		if (packet_size)
			*packet_size = stat.dmxFrameSize;
		device->currentDetectedDmxInPacketSize = stat.dmxFrameSize;
		emit rxDmxPacketSizeReceived(stat.dmxFrameSize);
	}
	if (device->debug)
		qDebug() << "YadiReceiver::getDetectedDmxFrameSize  detected DMX framesize" << stat.dmxFrameSize;

	return ok;
}

bool YadiReceiver::detectRxDmxUniverseSize(int *max_universe_size, int *used_universe_size)
{
	if (!device->serialDev())
		return false;
	if (!device->serialDev()->isOpen())
		return false;

	if ( device->serialDev()->writeSerial("ui\n") <= 0 ) {
		qDebug("YadiReceiver: Device 'ui' command failed");
		return false;
	}

	DmxAnswer stat(DmxAnswer::REQ_CHANNELS);
	bool ok = waitForAtHeader(stat);
	if (ok) {
		// qDebug() << "ui:" << stat.answerString;
		if (max_universe_size)
			*max_universe_size = stat.deviceMaxChannels;
		if (used_universe_size)
			*used_universe_size = stat.deviceUsedChannels;
		device->maxDeviceDmxInChannels = stat.deviceMaxChannels;
		device->usedDmxInChannels = stat.deviceUsedChannels;
	}

	if (device->debug) qDebug() << "YadiReceiver::detectRxDmxUniverseSize: detected channels:"
			 << *max_universe_size << "; used channels" << *used_universe_size;
	return ok;
}

bool YadiReceiver::waitForAtHeader(DmxAnswer &dmxstat)
{
	DmxAnswer::DmxAnswerType wait_for_type = dmxstat.type;

	if (!device->serialDev())
		return false;
	if (!device->serialDev()->isOpen())
		return false;


	// We do not wait forever here. So we need a timer for a breakcondition
	// We are waiting max 800ms or 1400ms depending on the type

	QElapsedTimer wait;
	wait.start();

	int time_to_wait;
	if (dmxstat.type == DmxAnswer::NONE) {
		time_to_wait = 800;
	} else {
		time_to_wait = 1400;
	}

	QByteArray &in = dmxstat.answerBytes;
	in.clear();

	// We loop as long as we get a timeout or the demanded type matches the received type
	bool ok = true;
	do {

		// Now we are waiting for the special character '@' the device
		// gives us hopefully back
		bool at = false;
		bool data = false;
		bool msg = false;
		bool end = false;
		in.clear();
		while (wait.elapsed()<time_to_wait && !end && cmd != STOPPED) {
			if (!device->serialDev()) return false;
			char inchar;
			qint64 bytes_read = device->serialDev()->readSerial(&inchar,1);
			if (bytes_read < 1) {
				msleep(5);
				continue;
			}

			if (at) {
				// We have received a '@'. Now we look for what it is
				if (inchar != ' ') {
					// It is a beginning dmx data block. But first there is a 3 char long ascii number, which
					// indicates the frame size
					data = true;
				} else {
					// a Message comes in and will end with a '\n'
					msg = true;
				}
				at = false;
				in.append(inchar);
			}
			else if (msg) {
				if (inchar == 0x0a) {
					// Message end
					end = true;
				} else {
					// Append until we receive the end character
					in.append(inchar);
				}
			}
			else if (data) {
				// Read 3 character asc number and end the loop
				in.append(inchar);
				if (in.size() == 3) end = true;
			}
			else if (inchar == '@') {
				// '@' found. That indicates the beginning of a status message or a dmx data block
				// But we have to wait for the next input character to see what it is
				at = true;
			}

		}

		// now we look at what we have got
		if (cmd == STOPPED) {
			ok = true;
			dmxstat.type = DmxAnswer::NONE;
			dmxstat.answerString = "Thread stopped";
		}
		else if (!end) {
			// A timeout occured -> that is bad
			qDebug("YadiReceiver::waitForAtHeader answer:timeout");
			ok = false;
		}
		else if (data) {
			// Ok, there is an upcoming datablock
			// Let's look at the size
			dmxstat.type = DmxAnswer::DMX_DATA;
			dmxstat.dmxDataSize = QString(in).toInt();
			if (device->debug > 4) qDebug("answer: dmx data block -> size:%d",dmxstat.dmxDataSize);
			ok = true;
		}
		else if (msg) {
			dmxstat.answerString = in;
			if (dmxstat.answerString.contains("Stonechip")) {
				dmxstat.type = DmxAnswer::REQ_VERSION;
			}
			else if (dmxstat.answerString.contains("Channels")) {
				dmxstat.type = DmxAnswer::REQ_DMX_FRAMESIZE;
				dmxstat.dmxFrameSize = dmxstat.answerString.mid(1,3).toInt();

			}
			else if (dmxstat.answerString.contains("RX DMX packetsize")) {
				dmxstat.type = DmxAnswer::REQ_DMX_FRAMESIZE;
				dmxstat.dmxFrameSize = dmxstat.answerString.mid(1,3).toInt();

			}
			else if (dmxstat.answerString.contains("(max,used)")) {
				dmxstat.type = DmxAnswer::REQ_CHANNELS;
				dmxstat.deviceMaxChannels = dmxstat.answerString.mid(1,3).toInt();
				dmxstat.deviceUsedChannels = dmxstat.answerString.mid(5,3).toInt();
			}
			else if (dmxstat.answerString.contains("Status")) {
				dmxstat.type = DmxAnswer::STATUS;
				dmxstat.dmxStatus = dmxstat.answerString.mid(8,3).toInt();
				if (dmxStatus != dmxstat.dmxStatus) {
					dmxStatus = dmxstat.dmxStatus;
					emit dmxStatusReceived(dmxStatus);
				}
				if (device->debug > 4) qDebug("answer: Status: %d",dmxstat.dmxStatus);
			}
			else {
				dmxstat.type = DmxAnswer::UNKNOWN;

			}
			ok = true;
		}
		else {
			// No data and no msg ??
			qDebug("%s: Neither message header nor data header received",Q_FUNC_INFO);
			ok = false;
			dmxstat.type = DmxAnswer::NONE;
		}

		if (!ok) {
			dmxstat.type = DmxAnswer::NONE;
			dmxstat.dmxDataSize = 0;
		}
	} while (ok && wait_for_type != DmxAnswer::NONE && wait_for_type != dmxstat.type && cmd != STOPPED);

	return ok;
}

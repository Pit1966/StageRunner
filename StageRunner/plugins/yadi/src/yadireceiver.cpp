#include "yadireceiver.h"
#include "yadidevice.h"
#include "serialwrapper.h"

#include <QDebug>
#include <QTime>
#include <QMessageBox>

#define DMX_HEADER_SIZE 4
#define DEBUG 0

YadiReceiver::YadiReceiver(YadiDevice *p_device, SerialWrapper * p_file)
{
	device = p_device;
	file = p_file;
	init();
}

YadiReceiver::~YadiReceiver()
{

	delete time;
}

void YadiReceiver::init()
{
	dmxStatus = 0;
	cmd = IDLE;
	time = new QTime;
}

bool YadiReceiver::startRxDmx()
{
	if (isRunning()) {
		qDebug() << "YadiReceiver::startRxDmx failed -> Thread already running";
		return false;
	}


	// Thread starten
	this->start();
	return true;
}


void YadiReceiver::run()
{
	bool ok = true;

	while (cmd != STOPPED && ok) {
		bool emit_all = true;

		int max_channels = -1;
		int used_channels = -1;
		ok = detectRxDmxUniverseSize(&max_channels, &used_channels);

		int rx_dmx_packet_size = -1;
		ok = detectRxDmxPacketSize(&rx_dmx_packet_size);

		if (rx_dmx_packet_size > 0 && used_channels > 0) {
			dmxStatus |= 1;
		} else {
			msleep(100);
		}

		// Begin inner loop
		time->restart();
		int transfer = 5;
		while (cmd != STOPPED && (dmxStatus&1) && ok) {
			cmd = RUNNING;

			DmxAnswer dmx;
			ok = waitForAtHeader(dmx);
			if (!ok) {
				qDebug() << "YadiReceiver::run: Wait for @-Header failed -> Leave receiver loop";
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
				QTime wait;
				wait.restart();

				qint64 channels_to_read = dmx.dmxDataSize;
				while (channels_to_read > 0 && wait.elapsed() < 1000) {
					in += file->readSerial(channels_to_read);
					channels_to_read = dmx.dmxDataSize - in.size();
				}
				if (DEBUG) qDebug("YadiReceiver::run: read size: %d",in.size());
				if (in.size() != dmx.dmxDataSize) {
					qDebug("YadiReceiver::run: read size: %d",in.size());
				}

				if (file->error()) {
					qDebug() << file->errorString();
					cmd = STOPPED;
				} else {
					int millis = time->elapsed();
					if (!millis) {
						msleep(50);
						continue;
					}
					QString update = QString("Frame interval: %1ms (%2frames/s)").arg(millis).arg(1000/millis);
					emit dmxPacketReceived(update);
					time->restart();
				}

				if (in.size() && !(transfer--)) {
					transfer = 0;
					int max_read = device->maxDeviceDmxInChannels;
					if (max_read > in.size()) max_read = in.size();

					for (int t=0; t<max_read; t++) {
						char val = in.at(t);
						if (device->inUniverse[t] != val || emit_all) {
							device->inUniverse[t] = val;
							emit dmxInChannelChanged(device->inputId,t,(uchar)val);
						}
					}
					if (DEBUG) qDebug()<< in.toHex();
					emit_all = false;
				}
			}

		}
		// end inner loop

	}
	if (!ok) {
		qDebug("YadiReceiver::run: exit DMXreceiver thread with failure");
		emit exitReceiverWithFailure();
	} else {
		qDebug("YadiReceiver::run: exit DMXreceiver thread normaly");
	}
	cmd = IDLE;
}

bool YadiReceiver::stopRxDmx()
{
	bool ok = true;
	if (isRunning()) {
		cmd = STOPPED;
		QTime wait;
		wait.restart();
		while (wait.elapsed() < 800) ;;

		if (isRunning()) {
			this->terminate();
			ok = false;
		}
	}
	return ok;
}



bool YadiReceiver::detectRxDmxPacketSize(int *packet_size)
{
	if (!file->isOpen())
		return false;

	file->writeSerial("c");

	DmxAnswer stat(DmxAnswer::REQ_DMX_FRAMESIZE);
	bool ok = waitForAtHeader(stat);

	if (ok) {
		if (packet_size)
			*packet_size = stat.dmxFrameSize;
		device->currentDetectedDmxInPacketSize = stat.dmxFrameSize;
		emit rxDmxPacketSizeReceived(stat.dmxFrameSize);
	}
	if (DEBUG) qDebug() << "YadiReceiver::getDetectedDmxFrameSize  detected DMX framesize" << stat.dmxFrameSize;

	return ok;
}

bool YadiReceiver::detectRxDmxUniverseSize(int *max_universe_size, int *used_universe_size)
{
	if (!file->isOpen())
		return false;

	file->writeSerial("ui");

	DmxAnswer stat(DmxAnswer::REQ_CHANNELS);
	bool ok = waitForAtHeader(stat);
	if (ok) {
		if (max_universe_size)
			*max_universe_size = stat.deviceMaxChannels;
		if (used_universe_size)
			*used_universe_size = stat.deviceUsedChannels;
		device->maxDeviceDmxInChannels = stat.deviceMaxChannels;
		device->usedDmxInChannels = stat.deviceUsedChannels;
	}

	qDebug() << "YadiReceiver::detectRxDmxUniverseSize: detected channels:"
			 << *max_universe_size << "; used channels" << *used_universe_size;
	return ok;
}

bool YadiReceiver::waitForAtHeader(DmxAnswer &dmxstat)
{
	DmxAnswer::DmxAnswerType wait_for_type = dmxstat.type;

	// We do not wait forever here. So we need a timer for a breakcondition
	// We are waiting max 100ms

	if (!file->isOpen())
		return false;

	QTime wait;
	wait.start();

	int time_to_wait;
	if (dmxstat.type == DmxAnswer::NONE) {
		time_to_wait = 600;
	} else {
		time_to_wait = 1000;
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
			char inchar;
			qint64 bytes_read = file->readSerial(&inchar,1);
			if (bytes_read < 1) {
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
			if (DEBUG) qDebug("answer: dmx data block -> size:%d",dmxstat.dmxDataSize);
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
				if (DEBUG) qDebug("Status: %d",dmxstat.dmxStatus);
			}
			else {
				dmxstat.type = DmxAnswer::UNKNOWN;

			}
			ok = true;
		}
		else {
			// No data and no msg ??
			qDebug("Neither message header nor data header received");
			ok = false;
			dmxstat.type = DmxAnswer::NONE;
		}

		if (!ok) {
			dmxstat.type = DmxAnswer::NONE;
			dmxstat.dmxDataSize = 0;
		}
	} while (ok && wait_for_type != DmxAnswer::NONE && wait_for_type != dmxstat.type);

	return ok;
}

#ifndef DMXRECEIVER_H
#define DMXRECEIVER_H

#include <QThread>
#include <QPair>


class QFile;
class QTime;
class SerialWrapper;
class YadiDevice;

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
	SerialWrapper *file;

	volatile RunCmd cmd;
	QTime *time;

public:
	YadiReceiver(YadiDevice *p_device, SerialWrapper * p_file);
	~YadiReceiver();

	bool startRxDmx();
	bool stopRxDmx();
	bool detectRxDmxPacketSize(int *packet_size);
	bool detectRxDmxUniverseSize(int *max_universe_size, int *used_universe_size);

	bool waitForAtHeader(DmxAnswer & dmxstat);

private:
	void init();
	void run();

signals:
	void exitReceiverWithFailure();
	void dmxPacketReceived(QString msg);
	void dmxInChannelChanged(quint32 input, quint32 channel, uchar value);
	void rxDmxPacketSizeReceived(int channels);
	void dmxStatusReceived(quint8);

};

#endif // DMXRECEIVER_H
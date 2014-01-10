#ifndef YADIDEVICE_H
#define YADIDEVICE_H

#include <QString>

class YadiReceiver;
class SerialWrapper;
class DmxMonitor;


class YadiDevice
{
public:
	enum Flags {
		FL_CLEAR = 0,
		FL_OUTPUT_UNIVERSE = 1<<0,
		FL_INPUT_UNIVERSE = 1<<1,
		FL_HAS_MONITOR = 1<<2,

		FL_INOUT_UNIVERSE = FL_OUTPUT_UNIVERSE + FL_INPUT_UNIVERSE
	};
	int debug;

	QByteArray outUniverse;
	QByteArray inUniverse;

	QString devNodePath;
	QString deviceProductName;
	QString idVendor;
	QString idProduct;
	QString deviceManufacturer;
	QString deviceSerial;

	int capabilities;
	int currentDetectedDmxInPacketSize;
	int usedDmxInChannels;
	int usedDmxOutChannels;
	int maxDeviceDmxInChannels;
	int maxDeviceDmxOutChannels;
	int universeMergeMode;
	bool deviceNodePresent;
	bool outputSendAllData;

	quint32 inputId;
	quint32 outputId;

	bool settingsChanged;				///< becomes true, if settings for device changed and will be written by QSettings later
	DmxMonitor *dmxInMonWidget;			///< A Pointer to a widget that allows the monitoring of input/output DMX data
	DmxMonitor *dmxOutMonWidget;

private:
	SerialWrapper *file;
	YadiReceiver *input_thread;

	bool input_open_f;
	bool output_open_f;

public:
	YadiDevice(const QString & dev_node);
	~YadiDevice();
	YadiDevice(const YadiDevice & other);
	YadiDevice & operator= (const YadiDevice & other);

	bool activateDevice();
	void deActivateDevice();
	bool openOutput();
	void closeOutput();
	bool openInput();
	void closeInput();
	void closeInOut();
	inline SerialWrapper *serialDev() const {return file;}

	QByteArray read(qint64 size);
	qint64 read(char *buf, qint64 size);
	qint64 write(const char * buf);
	qint64 write(const char *buf, qint64 size);
	bool isOutputOpen();
	bool isInputOpen();
	bool checkDeviceNode();
	YadiReceiver *inputThread() {return input_thread;}

	void saveConfig();
	void loadConfig();
	void sendConfigToDevice();

	DmxMonitor *openDmxInMonitorWidget();
	DmxMonitor *openDmxOutMonitorWidget();
	void closeDmxInMonitorWidget();
	void closeDmxOutMonitorWidget();

private:
	void init();


};

#endif // YADIDEVICE_H

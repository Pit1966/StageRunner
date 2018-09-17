#ifndef YADIDEVICE_H
#define YADIDEVICE_H

#include <QString>

class YadiReceiver;
class SerialWrapper;
class DmxMonitor;
class MvgAvgCollector;
class QSerialPortThread;


/**
 * @brief The YadiDevice class
 *
 * working parameter (get with stty -F /dev/ttyACM0)
 * speed 115200 baud; line = 0;
 * min = 1; time = 0;
 * -brkint -icrnl -imaxbel
 * -opost
 * -isig -icanon -echo -echoe

 *
 *
 */
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
	int outUniverseNumber;
	int inUniverseNumber;				// -1: unused

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
#ifdef QTSERIAL
	QSerialPortThread *m_serialThread;
#else
	YadiReceiver *input_thread;
	SerialWrapper *file;
#endif
	bool m_isInputOpen;
	bool m_isOutputOpen;
	bool m_isDeviceActivated;

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

#ifdef QTSERIAL
	QSerialPortThread * serialPortThread() {return m_serialThread;}
#else
	inline SerialWrapper *serialDev() const {return file;}
	YadiReceiver *inputThread() {return input_thread;}
#endif

	QByteArray read(qint64 size);
	qint64 read(char *buf, qint64 size);
	qint64 write(const char * buf);
	qint64 write(const char *buf, qint64 size);
	bool isOutputOpen();
	bool isInputOpen();
	bool checkDeviceNode();

	void saveConfig();
	void loadConfig();
	void sendConfigToDevice();

	DmxMonitor *openDmxInMonitorWidget();
	DmxMonitor *openDmxOutMonitorWidget();
	void closeDmxInMonitorWidget();
	void closeDmxOutMonitorWidget();

	static bool deviceNodeExists(const QString &dev_node);

	static QString threadName();
	static const char *threadNameAsc();

private:
	void init();


};

#endif // YADIDEVICE_H

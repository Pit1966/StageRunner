#ifndef YADIDMXUSBOUT_H
#define YADIDMXUSBOUT_H

#include "qlcioplugin.h"

#include <QStringList>

class SerialWrapper;
class YadiDevice;
class DmxMonitor;

class YadiDMXUSBOut : public QLCIOPlugin
{
	Q_OBJECT

	Q_INTERFACES(QLCIOPlugin)
#ifdef IS_QT5
	Q_PLUGIN_METADATA(IID "de.stonechip.stagerunner.yadi" FILE "yadi.json")
#endif
public:
	int debug;

private:
	QStringList output_devices;
	QStringList input_devices;

	bool write_universe_debug_out;

public:
	YadiDMXUSBOut();
	~YadiDMXUSBOut();

	void init();						///< @reimp
	void findDevices();
	QString name();						///< @reimp
	int capabilities() const;			///< @reimp
	void openOutput(quint32 output = 0);		///< @reimp
	void closeOutput(quint32 output = 0);		///< @reimp
	QStringList outputs();				///< @reimp
	void writeUniverse(quint32 output, const QByteArray& universe); ///< @reimp
	QString outputInfo(quint32 output = QLCIOPlugin::invalidLine()); ///< @reimp
	void openInput(quint32 input);		///< @reimp
	void closeInput(quint32 input);		///< @reimp
	QStringList inputs();				///< @reimp
	QString inputInfo(quint32 input);	///< @reimp
	void sendFeedBack(quint32 inputLine, quint32 channel, uchar value, const QString& key = 0);

	bool canConfigure();				///< @reimp
	void configure();					///< @reimp


	// StageRunner reimplemented functions
	DmxMonitor *openOutputMonitor(quint32 output);
	DmxMonitor *openInputMonitor(quint32 input);

	inline QStringList outputDeviceList() {return output_devices;}
	inline QStringList inputDeviceList() {return input_devices;}

private:
	void handle_output_error(quint32 output);
	void update_output_monitor(quint32 output, const QByteArray& universe);

signals:


public slots:
	void closeMonitorByInstancePointer(DmxMonitor *instance);
	void propagateChangedInput(quint32 input, quint32 channel, uchar value);

};

#endif // YADIDMXUSBOUT_H

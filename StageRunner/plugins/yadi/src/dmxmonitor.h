#ifndef DMXMONITOR_H
#define DMXMONITOR_H

#include <QWidget>
#include <QElapsedTimer>

class YadiDevice;

class DmxMonitor : public QWidget
{
	Q_OBJECT
private:
	int m_myUniverse;			///< the universe number which is observed here
	int bars;					///< Anzahl der angezeigten Kanäle
	int bar_value[512];
	int used_bars;				///< Das sind die Balken, die tatsächlich nur aktiv sind
	bool m_autoBarsEnabled;		///< If true, the used bar count will automatically set to the hightest bar number with a value > 0
	QString m_displayedFrameRateMsg;
	QString m_lastReceivedFrameRateMsg;
	QElapsedTimer m_frameRateUpdateTimer;

public:
	DmxMonitor(QWidget *parent = 0);
	void setChannelPeakBars(int num);
	inline int visibleBars() {return bars;}
	inline void setAutoBarsEnabled(bool enable) {m_autoBarsEnabled = enable;}

	void paintEvent(QPaintEvent *);
private:
	void init();
	void closeEvent(QCloseEvent *);

public slots:
	void setValueInBar(quint32 bar, uchar value);
	void setDetectedChannelsNumber(int channels);
	void setDmxValues(int universe, const QByteArray &dmxValues);
	void setFrameRateInfo(YadiDevice *yadiDev, const QString &frameRateMsg);

signals:
	void monitorClosed(DmxMonitor *instance);

};

#endif // DMXMONITOR_H

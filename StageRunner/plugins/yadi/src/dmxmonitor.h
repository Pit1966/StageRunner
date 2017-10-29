#ifndef DMXMONITOR_H
#define DMXMONITOR_H

#include <QWidget>


class DmxMonitor : public QWidget
{
	Q_OBJECT
private:
	int m_myUniverse;			///< the universe number which is shown here
	int bars;					///< Anzahl der angezeigten Kanäle
	int bar_value[512];
	int used_bars;				///< Das sind die Balken, die tatsächlich nur aktiv sind
	bool m_autoBarsEnabled;		///< If true, the used bar count will automatically set to the hightest bar number with a value > 0

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

signals:
	void monitorClosed(DmxMonitor *instance);

};

#endif // DMXMONITOR_H

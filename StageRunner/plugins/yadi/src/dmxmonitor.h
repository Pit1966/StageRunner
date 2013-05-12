#ifndef DMXMONITOR_H
#define DMXMONITOR_H

#include <QWidget>


class DmxMonitor : public QWidget
{
	Q_OBJECT
private:
	int bars;			///< Anzahl der angezeigten Kanäle
	int bar_value[512];
	int used_bars;		///< Das sind die Balken, die tatsächlich nur aktiv sind

public:
	DmxMonitor(QWidget *parent = 0);
	void setChannelPeakBars(int num);
	inline int visibleBars() {return bars;}

	void paintEvent(QPaintEvent *);
private:
	void init();
	void closeEvent(QCloseEvent *);

public slots:
	void setValueInBar(quint32 bar, uchar value);
	void setDetectedChannelsNumber(int channels);

signals:
	void monitorClosed(DmxMonitor *instance);

};

#endif // DMXMONITOR_H

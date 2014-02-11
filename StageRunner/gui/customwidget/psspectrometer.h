#ifndef PSSPECTROMETER_H
#define PSSPECTROMETER_H

#include "frqspectrum.h"

#include <QWidget>
#include <QTime>
#include <QVector>

#define BANDS_NORMAL 40

class PsSpectrometer : public QWidget
{
	Q_OBJECT
private:
	struct Bar {
		Bar()
			: value(0)
			, lastPeak(0)
			, peakAtMs(0)
			, holdSub(1)
			, hold(false)
		{}
		int value;
		int lastPeak;
		int peakAtMs;
		int holdSub;
		bool hold;
	};

	FrqSpectrum m_spectrum;
	QVector<Bar> m_bars;
	int m_showLowBand;
	int m_showHiBand;
	int m_idleTimerId;
	int m_idleCount;
	int m_idleBar;
	QTime m_timer;
	bool m_isZoomed;

public:
	PsSpectrometer(QWidget *parent = 0, int specSize = BANDS_NORMAL);
	inline int frqBands() const {return m_spectrum.size();}

private:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *);
	void timerEvent(QTimerEvent *ev);
	void animateIdle();

signals:

public slots:
	void setFrqSpectrum(FrqSpectrum *newSpectrum);

};

#endif // PSSPECTROMETER_H

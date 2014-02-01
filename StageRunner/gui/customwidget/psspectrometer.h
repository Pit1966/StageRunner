#ifndef PSSPECTROMETER_H
#define PSSPECTROMETER_H

#include "frqspectrum.h"

#include <QWidget>
#include <QTime>
#include <QVector>

#define BANDS_NORMAL 20
#define BANDS_ZOOM 40

class PsSpectrometer : public QWidget
{
	Q_OBJECT
private:
	struct Bar {
		Bar()
			: lastPeak(0)
			, peakAtMs(0)
			, holdSub(1)
			, hold(false)
		{}
		int lastPeak;
		int peakAtMs;
		int holdSub;
		bool hold;
	};

	FrqSpectrum m_spectrum;
	QVector<Bar> m_bars;
	int m_showLowBand;
	int m_showHiBand;
	QTime m_timer;

public:
	PsSpectrometer(QWidget *parent = 0, int specSize = BANDS_NORMAL);
	inline int frqBands() const {return m_spectrum.size();}

private:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *);

signals:

public slots:
	void setFrqSpectrum(FrqSpectrum *newSpectrum);

};

#endif // PSSPECTROMETER_H

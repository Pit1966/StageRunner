#include "psspectrometer.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

PsSpectrometer::PsSpectrometer(QWidget *parent, int specSize)
	: QWidget(parent)
	, m_spectrum(specSize)
	, m_bars(specSize)
	, m_showLowBand(0)
	, m_showHiBand(specSize-1)
{
	m_timer.start();
}

void PsSpectrometer::paintEvent(QPaintEvent *event)
{

	QRect drawRect = event->rect();
	int w = drawRect.width();
	int h = drawRect.height();
	int bars = m_showHiBand - m_showLowBand + 1;

	QPainter p(this);
	// this mirrors the paintarea at x-axes. 0,0 coordinate is now left/bottom
	p.setViewport(0,h,w,-h);
	p.fillRect(drawRect,QColor(Qt::black));

	if (bars <= 0)
		return;

	int w_bar = w / bars;


	for (int i=m_showLowBand; i<=m_showHiBand; i++) {
		if (i >= m_spectrum.size())
			break;
		Bar &bar = m_bars[i];

		int y_bar = float(h) * m_spectrum.levelAt(i);
		if (y_bar >= bar.lastPeak) {
			bar.lastPeak = y_bar;
			bar.peakAtMs = m_timer.elapsed();
			bar.hold = true;
		} else {
			if (bar.hold && m_timer.elapsed() - bar.peakAtMs > 400) {
				bar.hold = false;
				bar.holdSub = 1;
				bar.peakAtMs = m_timer.elapsed();
			}
			else if (bar.hold == false) {
				bar.lastPeak -= bar.holdSub;
				if (m_timer.elapsed() - bar.peakAtMs > 3) {
					bar.holdSub++;
					bar.peakAtMs = m_timer.elapsed();
				}
			}
		}

		p.setBrush(QBrush(Qt::blue));
		p.drawRect(i*w_bar,0,w_bar,bar.lastPeak);

		p.setBrush(QBrush(Qt::red));
		p.drawRect(i*w_bar,0,w_bar,y_bar);
	}
}

void PsSpectrometer::mousePressEvent(QMouseEvent *)
{
	if (m_spectrum.size() == BANDS_NORMAL) {
		m_spectrum.setFrqBands(BANDS_ZOOM);
		m_bars.resize(BANDS_ZOOM);
		m_showHiBand = BANDS_ZOOM/2;
	} else {
		m_spectrum.setFrqBands(BANDS_NORMAL);
		m_bars.resize(BANDS_NORMAL);
		m_showHiBand = BANDS_NORMAL-1;
	}
	update();
}

void PsSpectrometer::setFrqSpectrum(FrqSpectrum *newSpectrum)
{
	if (!newSpectrum) return;

	// Transform the spectral data
	m_spectrum.transformPeakFrom(*newSpectrum);
	// m_spectrum.transformAvgFrom(*newSpectrum);
	// and initiate a gui update
	update();
}

#include "psspectrometer.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

PsSpectrometer::PsSpectrometer(QWidget *parent, int specSize)
	: QWidget(parent)
	, m_spectrum(specSize)
{
}

void PsSpectrometer::paintEvent(QPaintEvent *event)
{
	QRect drawRect = event->rect();
	int w = drawRect.width();
	int h = drawRect.height();
	int bars = m_spectrum.size();

	QPainter p(this);
	// this mirrors the paintarea at x-axes. 0,0 coordinate is now left/bottom
	p.setViewport(0,h,w,-h);
	p.fillRect(drawRect,QColor(Qt::black));

	if (bars <= 0)
		return;

	int w_bar = w / bars;

	p.setBrush(QBrush(Qt::red));

	for (int bar=0; bar<bars; bar++) {
		int y_bar = float(h) * m_spectrum.levelAt(bar);
		p.drawRect(bar*w_bar,0,w_bar,y_bar);
	}
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

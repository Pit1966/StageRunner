//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "psspectrometer.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

PsSpectrometer::PsSpectrometer(QWidget *parent, int specSize)
	: QWidget(parent)
	, m_spectrum(specSize)
	, m_bars(specSize)
	, m_showLowBand(0)
	, m_showHiBand(specSize*2/3)
	, m_idleTimerId(0)
	, m_idleCount(0)
	, m_idleBar(0)
	, m_isZoomed(false)
{
	m_timer.start();
	m_idleTimerId = startTimer(40);
}

void PsSpectrometer::paintEvent(QPaintEvent *event)
{

	QRect drawRect = event->rect();
	int w = drawRect.width();
	int h = drawRect.height();
	int bars = m_showHiBand - m_showLowBand + 1;

	QPainter p(this);
	p.fillRect(drawRect,QColor(Qt::black));
	if (frqBands() <= 0) return;

	float freq = m_spectrum.maxFrequency() * (m_showHiBand+1) / frqBands();
	QFont font;
	font.setPointSize(6);
	p.setFont(font);
	p.setPen(QPen(Qt::white));
	p.drawText(QRect(0,0,w,10),tr("max %1Hz").arg(freq),QTextOption(Qt::AlignRight));

	// this mirrors the paintarea at x-axes. 0,0 coordinate is now left/bottom
	p.setViewport(0,h,w,-h);

	if (bars <= 0)
		return;

	float w_bar = float(w) / bars;


	for (int i=m_showLowBand; i<=m_showHiBand; i++) {
		if (i >= m_spectrum.size())
			break;
		Bar &bar = m_bars[i];

		int y_bar = float(h) * m_spectrum.levelAt(i);
		bar.value = y_bar;
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

		p.setPen(QPen());
		p.setBrush(QBrush(Qt::red));
		p.drawRect(w_bar * i,bar.lastPeak-1,w_bar,3);

		p.setBrush(QBrush(QColor(0x4050ff)));
		p.drawRect(w_bar * i,0,w_bar,y_bar);
	}

}

void PsSpectrometer::mousePressEvent(QMouseEvent *)
{
	if (!m_isZoomed) {
		m_showHiBand = m_spectrum.size()/4;
	} else {
		m_showHiBand = m_spectrum.size()*2/3;
	}
	m_isZoomed = !m_isZoomed;
	update();
}

void PsSpectrometer::timerEvent(QTimerEvent *ev)
{
	if (ev->timerId() == m_idleTimerId) {
		if (m_idleCount == 1) {
			m_spectrum.clear();
			m_idleCount--;
		}
		else if (m_idleCount == 0) {
			animateIdle();
			update();
		}
		else {
			m_idleCount--;
		}
	}
}

void PsSpectrometer::animateIdle()
{
	if (m_idleBar <= frqBands()) {
		if (m_idleBar < frqBands()) {
			m_bars[m_idleBar].lastPeak = height();
			m_bars[m_idleBar].peakAtMs = m_timer.elapsed();
			m_bars[m_idleBar].hold = true;
		}
	} else {
		if (m_idleBar > 100) {
			m_idleBar = 0;
		}
	}
	m_idleBar++;
}

void PsSpectrometer::setFrqSpectrum(FrqSpectrum *newSpectrum)
{
	if (!newSpectrum) return;
	m_idleCount = 40;

	// Transform the spectral data
	m_spectrum.transformPeakFrom(*newSpectrum);
	// m_spectrum.transformAvgFrom(*newSpectrum);
	// and initiate a gui update
	update();
	// qDebug("samplerate: %f",newSpectrum->maxFrequency());
}

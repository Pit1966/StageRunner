//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

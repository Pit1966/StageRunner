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

#ifndef PSVUMETER_H
#define PSVUMETER_H

#include "thirdparty/widget/qsynthmeter.h"


class PsVuMeter : public qsynthMeter
{
	Q_OBJECT

private:
	float m_volume;
	float m_dragBeginVolume;
	int m_mouseButton;
	QPoint m_dragBeginMousePos;

public:
	PsVuMeter(QWidget *parent = 0);
	inline float volume() const {return m_volume;}
	void setVolume(float vol);

private:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *);

signals:
	void valueChanged(float level);
	void valueChanged(int level);
	void sliderPressed();
	void sliderReleased();
};

#endif // PSVUMETER_H

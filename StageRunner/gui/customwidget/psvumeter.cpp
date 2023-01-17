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

#include "psvumeter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRect>

PsVuMeter::PsVuMeter(QWidget *parent)
	: qsynthMeter(parent)
	, m_volume(0.0f)
	, m_dragBeginVolume(0.0f)
	, m_mouseButton(0)
{
}

void PsVuMeter::setVolume(float vol)
{
	if (vol < 0.0f)
		vol = 0.0f;
	if (vol > 1.0f)
		vol = 1.0f;

	if (vol != m_volume) {
		m_volume = vol;
		update();
		emit valueChanged(m_volume);
		emit valueChanged(int(m_volume * 1000));
	}
}

void PsVuMeter::_setVolume(float vol)
{
	if (vol < 0.0f)
		vol = 0.0f;
	if (vol > 1.0f)
		vol = 1.0f;

	if (vol != m_volume) {
		m_volume = vol;
		update();
		emit valueChanged(m_volume);
		emit valueChanged(int(m_volume * 1000));
		emit sliderMoved(m_volume);
		emit sliderMoved(int(m_volume * 1000));
	}
}

void PsVuMeter::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	int xs = 14;
	int x = width() / 2 - 7;
	int ys = m_volume * height();
	int y = height() - ys;

	QColor col(0x006000);

	p.fillRect(x,y,xs,ys ,col);

}

void PsVuMeter::mousePressEvent(QMouseEvent *ev)
{

	if (ev->button() == Qt::LeftButton) {
		m_mouseButton = Qt::LeftButton;
		m_dragBeginMousePos = ev->pos();
		m_dragBeginVolume = m_volume;
		emit sliderPressed();
	}
}

void PsVuMeter::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_mouseButton == Qt::LeftButton) {
		QPoint move_dif = ev->pos() - m_dragBeginMousePos;
		float moved_norm = float(move_dif.y()) / float(height());
		_setVolume(m_dragBeginVolume - moved_norm);
		update();
	}
}

void PsVuMeter::mouseReleaseEvent(QMouseEvent *)
{
	if (m_mouseButton == Qt::LeftButton)
		emit sliderReleased();
	m_mouseButton = 0;
}

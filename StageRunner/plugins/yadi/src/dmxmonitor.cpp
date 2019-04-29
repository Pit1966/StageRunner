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

#include "dmxmonitor.h"

#include <QPainter>
#include <QDebug>


// #include <QDBusConnection>

DmxMonitor::DmxMonitor(QWidget *parent) :
	QWidget(parent)
{
	init();
}

void DmxMonitor::init()
{
	bars = 0;
	used_bars = 0;
	m_myUniverse = 0;
	memset(bar_value,0,sizeof(int) * 512);
	memset(bar_valueSec,0,sizeof(int) * 512);
	m_frameRateUpdateTimer.start();
	m_flags = F_ENABLE_BAR_BORDERS | F_ENABLE_BAR_TEXTS;

	m_barsBorderColor = Qt::blue;
	m_barsColor = Qt::white;
	m_barsSecColor = Qt::darkGreen;
}

void DmxMonitor::paintEvent(QPaintEvent *)
{
	int xsize = width();
	int ysize = height();
	int legendYSize = ysize < 40 ? 0 : 20;

	QPainter paint(this);
	// Hintergrund löschen
	paint.eraseRect(0,0,xsize,ysize);

	paint.fillRect(0,legendYSize,xsize,ysize-legendYSize,palette().base().color());

	paint.translate(0,ysize-1);
	paint.scale(1,-1);

	if (bars) {
		int bar_width = xsize / bars;
		int bar_height = ysize - legendYSize;

		// Second bar group
		if (m_flags & F_ENABLE_SECOND_BAR_GROUP) {
			paint.setBrush(m_barsSecColor);
			paint.setPen(Qt::NoPen);

			for (int t=0; t< bars; t++) {
				if (t >= used_bars) {
					paint.setPen(Qt::red);
				}
				int h = bar_height * bar_valueSec[t] / 255;
				paint.drawRect(t * bar_width + 1, 0, bar_width/2 - 1, h);
			}
		}

		// Main bar group
		paint.setBrush(m_barsColor);
		if (m_flags & F_ENABLE_BAR_BORDERS) {
			paint.setPen(m_barsBorderColor);
		} else {
			paint.setPen(Qt::NoPen);
		}
		for (int t=0; t< bars; t++) {
			if (t >= used_bars) {
				paint.setPen(Qt::red);
			}
			int h = bar_height * bar_value[t] / 255;
			if (m_flags & F_ENABLE_SECOND_BAR_GROUP) {
				paint.drawRect(t * bar_width + bar_width/2, 0, bar_width/2 - 2, h);
			} else {
				paint.drawRect(t * bar_width + 1, 0, bar_width - 2, h);
			}
		}

		// Main texts
		if (legendYSize > 0 && (m_flags&F_ENABLE_BAR_TEXTS)) {
			int yp = height();
			paint.resetTransform();
			for (int t=0; t< bars; t++) {
				if (m_flags & F_ENABLE_AUTO_BARS) {
					paint.setPen(Qt::white);
					QString val = QString("%1").arg(t+1,3,10);
					QRect rect = paint.boundingRect(0,18,bar_width,12,Qt::AlignCenter,val);
					paint.drawText(t * bar_width + rect.x(), rect.y(), val);
				}

				if (t >= used_bars) {
					paint.setPen(Qt::red);
				} else {
					paint.setPen(Qt::black);
				}
				QRect rect = paint.boundingRect(0,18,bar_width,18,Qt::AlignCenter,QString::number(bar_value[t],16));
				paint.drawText(t * bar_width + rect.x(), yp - 4/*-rect.y()*/ , QString::number(bar_value[t],16) );
			}
		}
	}

	paint.resetTransform();
	if (legendYSize > 0) {
		paint.setPen(Qt::white);
		paint.drawText(5,20, m_displayedFrameRateMsg);
	}
}

void DmxMonitor::closeEvent(QCloseEvent *)
{
	emit monitorClosed(this);
}

void DmxMonitor::mouseDoubleClickEvent(QMouseEvent *)
{
	setSmallHeightEnabled(!(m_flags&F_SMALL_HEIGHT));
}

void DmxMonitor::setChannelPeakBars(int num)
{
	if (num < 0 || num > 512) num = 512;
	bars = num;
	used_bars = num;
	memset(bar_value,0,sizeof(int)*512);

	if (bars < 48) {
		setMinimumWidth(bars * 20);
	}
	setMaximumWidth(bars * 20);

	update();
}

void DmxMonitor::setAutoBarsEnabled(bool enable)
{
	enable ? m_flags |= F_ENABLE_AUTO_BARS : m_flags &= ~F_ENABLE_AUTO_BARS;
}

void DmxMonitor::setBarsBordersEnabled(bool enable)
{
	enable ? m_flags |= F_ENABLE_BAR_BORDERS : m_flags &= ~F_ENABLE_BAR_BORDERS;
}

void DmxMonitor::setSecondBarGroupEnabled(bool enable)
{
	enable ? m_flags |= F_ENABLE_SECOND_BAR_GROUP : m_flags &= ~F_ENABLE_SECOND_BAR_GROUP;
}

void DmxMonitor::setSmallHeightEnabled(bool enable)
{
	if ( (m_flags & F_SMALL_HEIGHT) != enable) {
		if (enable) {
			m_flags |= F_SMALL_HEIGHT;
			setMaximumHeight(25);
		} else {
			m_flags &= ~F_SMALL_HEIGHT;
			setMaximumHeight(20000);
		}
	}
}

void DmxMonitor::setValueInBar(quint32 bar, uchar value)
{
	if (bar >= quint32(bars)) return;
	if (bar_value[bar] != value) {
		bar_value[bar] = value;
		update();
	}
}

void DmxMonitor::setDetectedChannelsNumber(int channels)
{
	used_bars = channels;
}

void DmxMonitor::setDmxValues(int universe, const QByteArray &dmxValues)
{
	if (universe != m_myUniverse)
		return;

	bool modified = false;
	for (int t=0; t<dmxValues.size(); t++) {
		if (t>=512)
			break;

		uchar val = dmxValues.at(t);
		if ( (m_flags&F_ENABLE_AUTO_BARS) && val > 0 && t >= bars) {
			bars = ((t/12)+1) * 12;
			used_bars = bars;
		}

		if (bar_value[t] != val) {
			bar_value[t] = val;
			modified = true;
		}
	}

	if (modified)
		update();
}

void DmxMonitor::setDmxValuesSec(int universe, const QByteArray &dmxValues)
{
	if (universe != m_myUniverse)
		return;

	bool modified = false;
	for (int t=0; t<dmxValues.size(); t++) {
		if (t>=512)
			break;

		uchar val = dmxValues.at(t);
		if ( (m_flags&F_ENABLE_AUTO_BARS) && val > 0 && t >= bars) {
			bars = ((t/12)+1) * 12;
			used_bars = bars;
		}

		if (bar_valueSec[t] != val) {
			bar_valueSec[t] = val;
			modified = true;
		}
	}

	if (modified)
		update();
}

void DmxMonitor::setFrameRateInfo(YadiDevice *yadiDev, const QString &frameRateMsg)
{
	Q_UNUSED(yadiDev);
	if (m_lastReceivedFrameRateMsg != frameRateMsg) {
		m_lastReceivedFrameRateMsg = frameRateMsg;

		if (m_frameRateUpdateTimer.elapsed() > 200) {
			m_frameRateUpdateTimer.start();
			m_displayedFrameRateMsg = m_lastReceivedFrameRateMsg;
			update();
		}
	}
}

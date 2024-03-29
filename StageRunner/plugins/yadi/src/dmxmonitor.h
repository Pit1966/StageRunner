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

#ifndef DMXMONITOR_H
#define DMXMONITOR_H

#include <QWidget>
#include <QElapsedTimer>

class YadiDevice;

class DmxMonitor : public QWidget
{
	Q_OBJECT
public:
	enum FLAGS {
		F_NONE = 0,
		F_ENABLE_AUTO_BARS = 1<<0,				///< If set, the used bar count will automatically set to the hightest bar number with a value > 0
		F_ENABLE_BAR_BORDERS = 1<<1,			///< If set (default), the bars will appear with a thin margin
		F_ENABLE_SECOND_BAR_GROUP = 1<<2,		///< enable a second bar group
		F_ENABLE_BAR_TEXTS = 1<<3,
		F_SMALL_HEIGHT = 1<<4
	};

private:
	int m_myUniverse;			///< the universe number which is observed here
	int bars;					///< Anzahl der angezeigten Kanäle
	int bar_value[512];
	int bar_valueSec[512];
	int used_bars;				///< Das sind die Balken, die tatsächlich nur aktiv sind
	FLAGS m_flags;				///< Design and option flags
	QString m_displayedFrameRateMsg;
	QString m_lastReceivedFrameRateMsg;
	QElapsedTimer m_frameRateUpdateTimer;
	QColor m_barsBorderColor;
	QColor m_barsColor;
	QColor m_barsSecColor;
	QColor m_valueColor;

public:
	DmxMonitor(QWidget *parent = 0);
	void setChannelPeakBars(int num);
	inline int visibleBars() {return bars;}
	void setAutoBarsEnabled(bool enable);
	void setBarsBordersEnabled(bool enable);
	void setSecondBarGroupEnabled(bool enable);
	void setSmallHeightEnabled(bool enable);

private:
	void init();
	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
	void setValueInBar(quint32 bar, uchar value);
	void setDetectedChannelsNumber(int channels);
	void setDmxValues(int universe, const QByteArray &dmxValues);
	void setDmxValuesSec(int universe, const QByteArray &dmxValues);
	void setFrameRateInfo(YadiDevice *yadiDev, const QString &frameRateMsg);

signals:
	void monitorClosed(DmxMonitor *instance);

};

inline DmxMonitor::FLAGS operator|(DmxMonitor::FLAGS a, DmxMonitor::FLAGS b)
{return static_cast<DmxMonitor::FLAGS>(static_cast<int>(a) | static_cast<int>(b));}
inline DmxMonitor::FLAGS & operator|=(DmxMonitor::FLAGS &a, const int b)
{a = a | DmxMonitor::FLAGS(b); return a;}
inline DmxMonitor::FLAGS operator&(DmxMonitor::FLAGS a, DmxMonitor::FLAGS b)
{return static_cast<DmxMonitor::FLAGS>(static_cast<int>(a) & static_cast<int>(b));}
inline DmxMonitor::FLAGS & operator&=(DmxMonitor::FLAGS &a, const DmxMonitor::FLAGS &b)
{a = a & b; return a;}
inline DmxMonitor::FLAGS & operator&=(DmxMonitor::FLAGS &a, const int b)
{a = a & DmxMonitor::FLAGS(b); return a;}

#endif // DMXMONITOR_H

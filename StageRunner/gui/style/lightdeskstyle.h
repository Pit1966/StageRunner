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

#ifndef LIGHTDESKSTYLE_H
#define LIGHTDESKSTYLE_H

#ifdef IS_QT5
#include <QtWidgets>
#else
#include <QProxyStyle>
#include <QStyleFactory>
#include <QPen>
#include <QPixmap>
#include <QDebug>
#include <QTabWidget>
#include <QPushButton>
#include <QPainter>
#include <QStyleOption>
#include <QGroupBox>
#endif

class LightDeskStyle : public QProxyStyle
{
	Q_OBJECT
protected:
	static int roundRadius;
	static QPen penBevelDarkTrans;
	static QPen penBevelLightTrans;

	QPixmap pixButtonL;
	QPixmap pixButtonM;
	QPixmap pixButtonR;

public:
	LightDeskStyle(const QString &basekey = "windows");
	~LightDeskStyle();

	void polish(QPalette &pal);
	void polish(QWidget *widget);

	int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const;
	int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;
	void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
	void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const;
	void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const;



protected:
	static void setTexture(QPalette &pal, QPalette::ColorRole role, const QPixmap &pixmap);
	static QPainterPath drawRoundBox(const QRect &rect, bool tabFrame = false, int textWidth = 0);
	static QPainterPath drawTabRoundBox(const QRect &rect, QStyle::State state);

signals:

public slots:

};

#endif // LIGHTDESKSTYLE_H

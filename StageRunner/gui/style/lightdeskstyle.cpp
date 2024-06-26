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

#include "lightdeskstyle.h"
#include "customwidget/psdockwidget.h"

#include <QPainterPath>

/// Statics
QPen LightDeskStyle::penBevelDarkTrans = QPen(QColor(10,10,10,120),3);
QPen LightDeskStyle::penBevelLightTrans = QPen(QColor(200,200,200,120),3);
int LightDeskStyle::roundRadius = 8;

LightDeskStyle::LightDeskStyle(const QString &basekey)
	: QProxyStyle(QStyleFactory::create(basekey))
	, m_isPaletteInitialized(false)
{
	pixButtonL = QPixmap(":/gfx/customwidget/desk_knob_left.png");
	pixButtonM = QPixmap(":/gfx/customwidget/desk_knob_mid.png");
	pixButtonR = QPixmap(":/gfx/customwidget/desk_knob_right.png");

	setObjectName("lightdesk");
}

LightDeskStyle::~LightDeskStyle()
{
	// qDebug("LightDeskStyle destroyed");
}

QPalette LightDeskStyle::standardPalette() const
{
	if (!m_isPaletteInitialized) {
		QColor darkGrey(30,30,30);
		QColor grey(180,180,200);
		QColor orange(220,180,0);
		QColor greyBlue(60,60,160);
		// QColor darkRedTrans(160,60,60,80);
		QColor highGreenTrans(0,250,0,60);
		QPixmap background_img(":/gfx/customwidget/desk_structure_2.png");

		QPalette pal(darkGrey);
		pal.setBrush(QPalette::Base, darkGrey);
		pal.setBrush(QPalette::Highlight, highGreenTrans);
		pal.setBrush(QPalette::HighlightedText, orange);
		pal.setBrush(QPalette::Window, darkGrey);
		pal.setBrush(QPalette::WindowText, grey);
		pal.setBrush(QPalette::Mid, greyBlue);
		setTexture(pal, QPalette::Window, background_img);

		m_standardPalette = pal;
		m_isPaletteInitialized = true;
	}

	return m_standardPalette;
}

void LightDeskStyle::polish(QPalette &pal)
{
	QColor darkGrey(30,30,30);
	QColor grey(180,180,200);
	QColor orange(220,180,0);
	QColor greyBlue(60,60,160);
	// QColor darkRedTrans(160,60,60,80);
	QColor highGreenTrans(0,250,0,60);
	QPixmap background_img(":/gfx/customwidget/desk_structure_2.png");

	pal = QPalette(darkGrey);
	pal.setBrush(QPalette::Base, darkGrey);
	pal.setBrush(QPalette::Highlight, highGreenTrans);
	pal.setBrush(QPalette::HighlightedText, orange);
	pal.setBrush(QPalette::Window, darkGrey);
	pal.setBrush(QPalette::WindowText, grey);
	// pal.setBrush(QPalette::Base, QColor("blue"));
	pal.setBrush(QPalette::Mid, greyBlue);
	setTexture(pal, QPalette::Window, background_img);
}

void LightDeskStyle::polish(QWidget *widget)
{
	Q_UNUSED(widget)
	// widget->setAutoFillBackground(true);
}

int LightDeskStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
	switch (hint) {
	case SH_DitherDisabledText:
		return int(false);
	case SH_EtchDisabledText:
		return int(true);
	case SH_DockWidget_ButtonsHaveFrame:
		return int(false);
	default:
		return QProxyStyle::styleHint(hint, option, widget, returnData);
	}
}

int LightDeskStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
	switch (metric) {
	case PM_ComboBoxFrameWidth:
		return 8;
	case PM_ScrollBarExtent:
		return QProxyStyle::pixelMetric(metric, option, widget) + 4;
	case QStyle::PM_SmallIconSize:
		return 30;
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}

}

void LightDeskStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
//	const QWidget *w = widget;
//	while (w) {
//		if (qobject_cast<const PsDockWidget*>(w)) {
//			qDebug() << "draw Complex PsDock" << w ;
//			QRect r = option->rect;
//			painter->fillRect(r, Qt::blue);
//		}
//		w = w->parentWidget();
//	}

//	QRect r = option->rect;
//	painter->fillRect(r, Qt::blue);


//	QString name = widget->objectName();
//	if (!name.contains(QRegExp("(Audio|itemEdit)")))
//		qDebug() << "drawprim" << element << option << widget->objectName();

//	if (element == PE_FrameTabBarBase) {
//		if (widget->parent())
//			qDebug() << "draw" << option << widget << " p" << widget->parent();
//		else
//			qDebug() << "draw " << option << widget;


//		qDebug() << " " << option->rect;
//	}


//  Test
//	QRect r = option->rect;
//	painter->fillRect(r, Qt::green);
//	qDebug() << "drawPrimitive element type" << element << widget->objectName();

	switch (element) {
//	case PE_CustomBase:
//	case PE_FrameFocusRect:
//	case PE_FrameWindow:
//	case PE_Widget:
//	case PE_FrameTabBarBase:
//	case PE_FrameDefaultButton:
//	case PE_PanelButtonTool:
//	case PE_FrameMenu:
//	case PE_FrameStatusBar:
//	case PE_FrameDockWidget:
//	case PE_FrameButtonTool:

//	case PE_IndicatorTabTear:
//	case PE_IndicatorTabClose:

//	case PE_FrameButtonBevel:
//	case PE_PanelButtonBevel:

	case PE_FrameDefaultButton:
	case PE_FrameFocusRect:
		break;

	case PE_PanelButtonCommand:
		{
			const QPushButton *but = qobject_cast<const QPushButton*>(widget);
			if (but && but->text().isEmpty()) return;

			painter->save();
			QRect area = option->rect;

			if (option->state & QStyle::State_HasFocus) {
				painter->fillRect(area,QBrush(Qt::red));
			} else {
				painter->fillRect(area,QBrush(Qt::black));
			}
			if (!(option->state & QStyle::State_Raised)) {
				area.setTopLeft(area.topLeft() + QPoint(1,1));
				area.setBottomRight(area.bottomRight() + QPoint(0,1));
			}

			QRect area_m = QRect(area.topLeft() + QPoint(1,1),area.size()-QSize(10,2));
			QRect area_l = QRect(area.topLeft() + QPoint(1,1),QSize(30,area.height()-2));
			QRect area_r = QRect(area.topRight()- QPoint(30,-1),QSize(30,area.height()-2));

			painter->drawPixmap(area_m,pixButtonM);
			painter->drawPixmap(area_l,pixButtonL);
			painter->drawPixmap(area_r,pixButtonR);

			painter->restore();
		}
		break;

	case PE_FrameTabBarBase: {
		QRect r = option->rect;
		//r.adjust(0,0,0,-1);
		painter->fillRect(r, QColor(200,200,200,120));
		break;
	}

	case PE_FrameTabWidget:
		{
			painter->save();

			bool top_left_edge_not_rounded = false;
			const QTabWidget *tab_widget = qobject_cast<const QTabWidget*>(widget);
			if (tab_widget && tab_widget->currentIndex() == 0) {
				top_left_edge_not_rounded = true;
			}

			QPainterPath roundRect = drawRoundBox(option->rect,top_left_edge_not_rounded);

			int x, y, w, h;
			option->rect.getRect(&x, &y, &w, &h);
			painter->setRenderHint(QPainter::Antialiasing, true);

			int radius = 8;
			int x1 = x;
			int x2 = x + radius;
			int x3 = x + w - radius;
			int x4 = x + w;


			QPolygon topHalf;
			topHalf << QPoint(x1, y)
					<< QPoint(x4, y)
					<< QPoint(x3, y + radius)
					<< QPoint(x2, y + h - radius)
					<< QPoint(x1, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(topHalf, Qt::IntersectClip);
			painter->setPen(penBevelLightTrans);
			painter->drawPath(roundRect);

			QPolygon bottomHalf = topHalf;
			bottomHalf[0] = QPoint(x4, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(bottomHalf, Qt::IntersectClip);
			painter->setPen(penBevelDarkTrans);
			painter->drawPath(roundRect);

			painter->restore();
		}
		break;

	case PE_FrameGroupBox:
		{
			painter->save();

			int textLength = 0;

			const QGroupBox *group = qobject_cast<const QGroupBox*>(widget);
			if (group) {
				QFontMetrics fm(group->font());
				QRect frect = fm.boundingRect(group->title());
				textLength = frect.width() + 10;
			}

			QPainterPath roundRect = drawRoundBox(option->rect,false,textLength);

			int x, y, w, h;
			option->rect.getRect(&x, &y, &w, &h);
			painter->setRenderHint(QPainter::Antialiasing, true);

			if (widget->autoFillBackground()) {
				painter->fillPath(roundRect,widget->palette().base());
			}

			int radius = 8;
			int x1 = x;
			int x2 = x + radius;
			int x3 = x + w - radius;
			int x4 = x + w;


			QPolygon topHalf;
			topHalf << QPoint(x1, y)
					<< QPoint(x4, y)
					<< QPoint(x3, y + radius)
					<< QPoint(x2, y + h - radius)
					<< QPoint(x1, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(topHalf, Qt::IntersectClip);
			painter->setPen(penBevelDarkTrans);
			painter->drawPath(roundRect);

			QPolygon bottomHalf = topHalf;
			bottomHalf[0] = QPoint(x4, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(bottomHalf, Qt::IntersectClip);
			painter->setPen(penBevelLightTrans);
			painter->drawPath(roundRect);

			// painter->setPen(option->palette.foreground().color());
			// painter->setClipping(false);
			// painter->drawPath(roundRect);


			painter->restore();
		}


		break;
	default:
		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
}

void LightDeskStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
//	if (element == CE_ShapedFrame) {
//		QRect r = option->rect;
//		//r.adjust(0,0,0,-1);
//		painter->fillRect(r, Qt::green);

//		return;
//	}

//	const QWidget *w = widget;
//	while (w) {
//		if (qobject_cast<const PsDockWidget*>(w)) {
//			qDebug() << "PsDock" << w << "element" << element;
//			QRect r = option->rect;
//			painter->fillRect(r, Qt::green);
//		}
//		w = w->parentWidget();
//	}

//	if (element != CE_ShapedFrame)
//		qDebug() << "drawControl" << element;

	switch (element) {
	case CE_TabBarTabShape:
		{
			painter->save();

			int x, y, w, h;
			option->rect.getRect(&x, &y, &w, &h);
			painter->setRenderHint(QPainter::Antialiasing, true);

			if (option->state & State_Selected) {
				QBrush over(QPixmap(":/gfx/customwidget/desk_structure_2.png"));
				painter->setBrush(over);
				painter->setPen(Qt::NoPen);
				painter->drawRect(x,y,w,h);
			}

			// qDebug("paint CE_TabBarTabShape x%d, y%d, w%d, h%d",x,y,w,h);


			QPainterPath clipRect = drawRoundBox(QRect(x,y,w,h));
			QPainterPath roundRect = drawTabRoundBox(QRect(x,y,w,h),option->state);

			int radius = roundRadius;
			int x1 = x;
			int x2 = x + radius;
			int x3 = x + w - radius;
			int x4 = x + w;


			QPolygon topHalf;
			topHalf << QPoint(x1, y)
					<< QPoint(x4, y)
					<< QPoint(x3, y + radius)
					<< QPoint(x2, y + h - radius)
					<< QPoint(x1, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(topHalf, Qt::IntersectClip);
			if (option->state & State_Selected) {
				painter->setPen(penBevelLightTrans);
			} else {
				painter->setPen(penBevelDarkTrans);
			}
			painter->drawPath(roundRect);

			QPolygon bottomHalf = topHalf;
			bottomHalf[0] = QPoint(x4, y + h);

			painter->setClipPath(roundRect);
			painter->setClipRegion(bottomHalf, Qt::IntersectClip);
			if (option->state & State_Selected) {
				painter->setPen(penBevelDarkTrans);
			} else {
				painter->setPen(penBevelLightTrans);
			}
			painter->drawPath(roundRect);

			painter->restore();
		}
		break;

#if QT_VERSION >= 0x050e00
	case CE_MenuItem:
		painter->fillRect(option->rect, QColor(30,30,30));
		QProxyStyle::drawControl(element, option, painter, widget);
		break;
#endif

	default:
//			QRect r = option->rect;
//			//r.adjust(0,0,0,-1);
//			painter->fillRect(r, Qt::red);
		QProxyStyle::drawControl(element, option, painter, widget);
	}
}

void LightDeskStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
//				QRect r = option->rect;
//				//r.adjust(0,0,0,-1);
//				painter->fillRect(r, Qt::green);
//	const QWidget *w = widget;
//	while (w) {
//		if (qobject_cast<const PsDockWidget*>(w)) {
//			qDebug() << "draw Complex PsDock" << w ;
//			QRect r = option->rect;
//			painter->fillRect(r, Qt::red);
//		}
//		w = w->parentWidget();
//	}

	QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void LightDeskStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
	// qDebug() << text << textRole;
	if (textRole == QPalette::NoRole) {
		textRole = QPalette::HighlightedText;
		painter->save();
		QFont font = painter->font();
		QRect newrect(rect);
		newrect.setX(rect.x());
		newrect.setWidth(rect.width()+ 11);
		font.setWeight(QFont::Bold);
		painter->setFont(font);
		painter->setClipping(false);
		QProxyStyle::drawItemText(painter, newrect, flags, pal, enabled, text, textRole);
		painter->restore();
		return;
	}

	QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}

void LightDeskStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
	QProxyStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

void LightDeskStyle::setTexture(QPalette &pal, QPalette::ColorRole role, const QPixmap &pixmap)
{
	for (int i = 0; i < QPalette::NColorGroups; ++i) {
		QColor color = pal.brush(QPalette::ColorGroup(i), role).color();
		pal.setBrush(QPalette::ColorGroup(i), role, QBrush(color, pixmap));
	}
}

QPainterPath LightDeskStyle::drawRoundBox(const QRect &rect, bool tabFrame, int textWidth)
{
	int textHeight = 10;
	if (!textWidth) {
		textHeight = 0;
	} else {
		textWidth += 20;
	}
	int radius = roundRadius;
	int diam = 2 * radius;

	int x1, y1, x2, y2;
	rect.getCoords(&x1, &y1, &x2, &y2);

	QPainterPath path;
	path.moveTo(x1 + radius, y2);
	path.arcTo(QRect(x1, y2 - diam, diam, diam), -90.0, -90.0);
	if (tabFrame) {
		path.lineTo(x1, y1);
	}
	else {
		path.lineTo(x1, y1 + radius + textHeight);
		path.arcTo(QRect(x1, y1 + textHeight, diam, diam), -180.0, -90.0);
	}

	if (textWidth) {
		path.lineTo(x1 + textWidth, y1 + textHeight);
		path.lineTo(x1 + textWidth, y1 + radius);
		path.arcTo(QRect(x1 + textWidth, y1, diam, diam), -180, -90);

	}

	path.lineTo(x2 - radius, y1);
	path.arcTo(QRect(x2 - diam, y1, diam, diam), -290.0, -90.0);
	path.lineTo(x2, y2 - radius);
	path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 0.0, -90.0);
	path.lineTo(x1 + radius, y2);

	return path;
}


QPainterPath LightDeskStyle::drawTabRoundBox(const QRect &rect, QStyle::State state)
{
	int radius = roundRadius;
	int diam = 2 * radius;

	int x1, y1, x2, y2;
	rect.getCoords(&x1, &y1, &x2, &y2);

	QPainterPath path;
	if (state & State_Selected) {
		path.moveTo(x1, y2 + 15);
		path.lineTo(x1, y1 + radius);
		path.arcTo(QRect(x1,y1, diam, diam), -180.0, -90.0);
		path.lineTo(x2 - radius, y1);
		path.arcTo(QRect(x2 - diam, y1, diam, diam), -290.0, -90.0);
		path.lineTo(x2, y2 + 5);
	} else {
		// This draws full edge rounded tab entry
//		path.moveTo(x1 + radius, y2);
//		path.arcTo(QRect(x1, y2 - diam, diam, diam), -90.0, -90.0);
//		path.lineTo(x1, y1 + radius);
//		path.arcTo(QRect(x1,y1, diam, diam), -180.0, -90.0);
//		path.lineTo(x2 - radius, y1);
//		path.arcTo(QRect(x2 - diam, y1, diam, diam), -290.0, -90.0);
//		path.lineTo(x2, y2 - radius);
//		path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 0.0, -90.0);
//		path.lineTo(x1 + radius, y2);

		// This is for unselected tabs with located at top of the tab widget
		path.moveTo(x1, y2);
		path.lineTo(x1, y1 + radius);
		path.arcTo(QRect(x1,y1, diam, diam), -180.0, -90.0);
		path.lineTo(x2 - radius, y1);
		path.arcTo(QRect(x2 - diam, y1, diam, diam), -290.0, -90.0);
		path.lineTo(x2, y2);
	}

	return path;
}


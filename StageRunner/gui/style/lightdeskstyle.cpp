#include "lightdeskstyle.h"

#include <QDebug>
#include <QTabWidget>

/// Statics
QPen LightDeskStyle::penBevelDarkTrans = QPen(QColor(10,10,10,120),3);
QPen LightDeskStyle::penBevelLightTrans = QPen(QColor(200,200,200,120),3);
int LightDeskStyle::roundRadius = 8;


LightDeskStyle::LightDeskStyle(const QString &basekey) :
	QProxyStyle(QStyleFactory::create(basekey))
{
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
	pal.setBrush(QPalette::Mid, greyBlue);
	setTexture(pal, QPalette::Window, background_img);

}

int LightDeskStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
	switch (hint) {
	case SH_DitherDisabledText:
		return int(false);
	case SH_EtchDisabledText:
		return int(true);
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
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}

}

void LightDeskStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{

	switch (element) {
//	case PE_FrameButtonBevel:
//	case PE_CustomBase:
//	case PE_FrameFocusRect:
//	case PE_FrameWindow:
//	case PE_Widget:
//	case PE_PanelButtonBevel:
//	case PE_FrameTabBarBase:
//	case PE_PanelButtonCommand:
//	case PE_FrameDefaultButton:
//	case PE_PanelButtonTool:
//	case PE_FrameMenu:
//	case PE_FrameStatusBar:
//	case PE_FrameDockWidget:
//	case PE_FrameButtonTool:

//	case PE_IndicatorTabTear:
//	case PE_IndicatorTabClose:

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
				textLength = frect.width();
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
	default:
		QProxyStyle::drawControl(element, option, painter, widget);
	}
}

void LightDeskStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
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
		newrect.setX(rect.x()-6);
		newrect.setWidth(rect.width()+5);
		font.setWeight(QFont::Bold);
		painter->setFont(font);
		painter->setClipping(false);
		QProxyStyle::drawItemText(painter, newrect, flags, pal, enabled, text, textRole);
		painter->restore();
		return;
	}

	QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
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
		path.moveTo(x1 + radius, y2);
		path.arcTo(QRect(x1, y2 - diam, diam, diam), -90.0, -90.0);
		path.lineTo(x1, y1 + radius);
		path.arcTo(QRect(x1,y1, diam, diam), -180.0, -90.0);
		path.lineTo(x2 - radius, y1);
		path.arcTo(QRect(x2 - diam, y1, diam, diam), -290.0, -90.0);
		path.lineTo(x2, y2 - radius);
		path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 0.0, -90.0);
		path.lineTo(x1 + radius, y2);
	}

	return path;
}


//void LightDeskStyle::polish(QWidget *widget)
//{

//}
#include "timelineitem.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace PS_TL {

TimeLineItem::TimeLineItem(TimeLineWidget *timeline, int trackId)
	: m_timeline(timeline)
	, m_trackId(trackId)
{
}

void TimeLineItem::setPosition(int ms)
{
	m_positionMs = ms;
	m_isTimePosValid = true;
	m_isPixelPosValid = false;
}

void TimeLineItem::setDuration(int ms)
{
	m_isTimePosValid = true;
	m_isPixelPosValid = false;
	m_durationMs = ms;
}

qreal TimeLineItem::yPos() const
{
	return y();
}

void TimeLineItem::setYPos(qreal yPixelPos)
{
	setY(yPixelPos);
}

void TimeLineItem::setLabel(const QString &label)
{
	m_label = label;
}

void TimeLineItem::setBackgroundColor(const QColor &col)
{
	m_colorBG = col;
}

void TimeLineItem::recalcPixelPos()
{
	if (m_isTimePosValid) {
		setX(qreal(m_positionMs) / m_timeline->msPerPixel());
		m_xSize = qreal(m_durationMs) / m_timeline->msPerPixel();
	}
}

QRectF TimeLineItem::boundingRect() const
{
	return QRectF(0, 0, m_xSize + m_penWidthBorder, m_ySize + m_penWidthBorder);
}

void TimeLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_timeline->msPerPixel() <= 0)
		return;

	if (m_isTimePosValid) {
		if (!m_isPixelPosValid) {
			setX(qreal(m_positionMs) / m_timeline->msPerPixel());
			m_xSize = qreal(m_durationMs) / m_timeline->msPerPixel();
		}
	}
	else {
		m_isTimePosValid = true;
		m_positionMs = m_timeline->msPerPixel() * x();
		m_durationMs = m_timeline->msPerPixel() * m_xSize;

	}


	QPen pen;
	pen.setColor(m_colorBorder);
	pen.setWidthF(m_penWidthBorder);
	painter->setPen(pen);
	painter->setBrush(m_colorBG);
	painter->drawRect(m_penWidthBorder/2, m_penWidthBorder/2, m_xSize, m_ySize);

	painter->drawText(5, m_ySize / 2, m_label);
}

void TimeLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_clickPos = event->scenePos();
		m_itemPos = scenePos();				// store current position for move
		m_isClicked = true;
		// qDebug() << "clicked item" << m_label << "at pos" << m_itemPos;
	} else {
		m_isClicked = false;
	}
}

void TimeLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isClicked) {
		QPointF curPos = event->scenePos();
		qreal xdif = curPos.x() - m_clickPos.x();
		// qreal ydif = curPos.y() - m_clickPos.y();

		qreal newX = m_itemPos.x() + xdif;
		qreal newY = m_itemPos.y() /*+ ydif*/;
		if (newX < 0)
			newX = 0;
		if (newY < 0)
			newY = 0;

		// set new item pos
		setPos(newX, newY);

		// calc new time position
		m_positionMs = m_timeline->msPerPixel() * newX;
		qDebug() << m_label << "current timepos" << m_positionMs << "size" << m_durationMs;
	}
}

void TimeLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
{
	m_isClicked = false;
	m_timeline->checkForRightMostItem(this);
}

} // namespace PS_TL

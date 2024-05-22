#include "timelinecursor.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace PS_TL {

TimeLineCursor::TimeLineCursor(TimeLineWidget *timeline, int trackId)
	: TimeLineBase(timeline, trackId)
{
	int yo = m_gripOffset;
	int s = m_gripSize;


	m_handle << QPointF(-s, yo)
			 << QPointF(-s, s + yo)
			 << QPointF(0, 2*s + yo)
			 << QPointF(s, s + yo)
			 << QPointF(s, yo);

	setHeight(m_height);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
}

void TimeLineCursor::setHeight(qreal height)
{
	m_height = height;
	m_line.setP2(QPointF(0, m_height));
}

void TimeLineCursor::recalcPixelPos()
{
	if (m_isTimePosValid) {
		setX(qreal(position()) / m_timeline->msPerPixel());
		m_xSize = 1;
	}
}

// QSizeF TimeLineCursor::size() const
// {
// 	float minX = m_handle[0].x();
//     float minY = m_handle[0].y();
//     float maxX = m_handle[0].x();
//     float maxY = m_handle[0].y();
//     for(QPointF point : m_handle){
//         if (point.x() < minX){
//             minX = point.x();
//         }
//         if (point.y() < minY){
//             minY = point.y();
//         }
//         if (point.x() > maxX){
//             maxX = point.x();
//         }
//         if (point.y() > maxY){
//             maxY = point.y();
//         }
//     }
//     return QSizeF(maxX-minX,line.p2().y());
// }

QRectF TimeLineCursor::boundingRect() const
{
	//QRectF rf = m_handle.boundingRect();
	// rf.setHeight(m_height);

	QRectF rf(-m_gripSize, 0, 2 * m_gripSize + 1, m_height);

	return rf;
}

void TimeLineCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_timeline->msPerPixel() <= 0)
		return;

	if (m_isTimePosValid) {
		if (!m_isPixelPosValid) {
			setX(qreal(position()) / m_timeline->msPerPixel());
			m_xSize = 1;
		}
	}
	else {
		m_isTimePosValid = true;
		setPosition(m_timeline->msPerPixel() * x());
		setDuration(0);
	}

	QPen pen;
	pen.setColor(m_lineCol);
	painter->setPen(pen);
	painter->drawLine(m_line);

	pen.setColor(m_handleBorderCol);
	painter->setPen(pen);
	painter->setBrush(m_handleCol);
	painter->drawPolygon(m_handle);

	m_wasPainted = true;
}

void TimeLineCursor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_clicked = true;
	}
}

void TimeLineCursor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_clicked) {
		QPointF evPos = event->scenePos();
		// calc new time position 1
		// int new_ms = m_timeline->msPerPixel() * evPos.x();
		// if (new_ms < 0) {
		// 	new_ms = 0;
		// }
		// else if (new_ms > m_timeline->timeLineDuration()) {
		// 	new_ms = m_timeline->timeLineDuration();
		// }

		if (evPos.x() < m_timeline->leftViewPixel()) {
			setPos(m_timeline->leftViewPixel(), y());
		}
		else if (evPos.x() > m_timeline->rightViewPixel()) {
			setPos(m_timeline->rightViewPixel(), y());
		}
		else {
			setPos(evPos.x(), y());
		}
		// QGraphicsItem::mouseMoveEvent(event);
		// update();

		int new_ms = m_timeline->msPerPixel() * x();
		if (new_ms != position()) {
			setPosition(new_ms);
			emit cursorMoved(new_ms);
		}
	}
}

void TimeLineCursor::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
{
	m_clicked = false;
}

} // namespace PS_TL

#include "timelinecursor.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace PS_TL {

TimeLineCursor::TimeLineCursor(TimeLineWidget *timeline)
	: TimeLineBase(timeline)
{
	m_handle << QPointF(-10,0)
			 << QPointF(-10,10)
			 << QPointF(0, 20)
			 << QPointF(10,10)
			 << QPointF(10,0);

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
		setX(qreal(m_positionMs) / m_timeline->msPerPixel());
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
	QRectF rf = m_handle.boundingRect();
	rf.setHeight(m_height);
	return rf;
}

void TimeLineCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_timeline->msPerPixel() <= 0)
		return;

	if (m_isTimePosValid) {
		if (!m_isPixelPosValid) {
			setX(qreal(m_positionMs) / m_timeline->msPerPixel());
			m_xSize = 1;
		}
	}
	else {
		m_isTimePosValid = true;
		m_positionMs = m_timeline->msPerPixel() * x();
		m_durationMs = 0;
	}


	QPen pen;
	pen.setColor(m_lineCol);
	painter->setPen(pen);
	painter->drawLine(m_line);

	pen.setColor(m_handleBorderCol);
	painter->setPen(pen);
	painter->setBrush(m_handleCol);
	painter->drawPolygon(m_handle);
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
		QPointF pos = event->scenePos();

		// calc new time position
		m_positionMs = m_timeline->msPerPixel() * pos.x();

		setPos(pos.x(),	y());
		// QGraphicsItem::mouseMoveEvent(event);
		// update();
	}
}

void TimeLineCursor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	m_clicked = false;
}

} // namespace PS_TL

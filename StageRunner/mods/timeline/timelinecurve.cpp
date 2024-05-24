#include "timelinecurve.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QDebug>

namespace PS_TL {


int TimeLineCurve::Node::scaledY() const
{
	int b = 3;
	int h = m_myTrack->ySize() - 2 * b;		// track height

	int y = h * yPM / 1000;

	return h - y + b;
}

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

TimeLineCurve::TimeLineCurve(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
{
	m_xSize = 500;
	m_xSize = 30;
	m_colorBG = 0x333355;

	m_myTrack = m_timeline->findTrackWithId(trackId);

	m_nodes.append(Node(m_myTrack, 0, 10));
	m_nodes.append(Node(m_myTrack, 10000, 0));
	m_nodes.append(Node(m_myTrack, 40000, 1000));
}

void TimeLineCurve::setTimeLineDuration(int ms)
{
	setDuration(ms);

	// delete all nodes that lies beyond the duration
	Node last = m_nodes.takeLast();
	last.xMs = ms;

	while (m_nodes.size() > 1 && m_nodes.last().xMs >= ms)
		m_nodes.removeLast();

	// append last node again
	m_nodes.append(last);

	recalcPixelPos();
}

void TimeLineCurve::recalcPixelPos()
{
	setX(0);
	if (m_timeline->msPerPixel() > 0) {
		m_xSize = m_timeline->msToPixel(duration());
		m_isPixelPosValid = true;
	} else {
		m_isPixelPosValid = false;
	}
}

void TimeLineCurve::setTrackDuration(int ms)
{
	setTimeLineDuration(ms);
}

QRectF TimeLineCurve::boundingRect() const
{
	QRectF br(0, 0, m_xSize, m_ySize);
	return br;
}

void TimeLineCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_isTimePosValid) {
		m_xSize = m_timeline->msToPixel(duration());
	}

	// painter->setBrush(m_colorBG);
	// painter->setPen(Qt::NoPen);
	// painter->drawRect(0, 0, m_xSize, m_ySize);

	QPen pen;
	pen.setColor(m_colorBorder);
	painter->setPen(pen);

	QRectF textRect = boundingRect();
	textRect.adjust(5,-2,-5,-10);
	painter->drawText(textRect, m_label);

	if (!m_myTrack)
		m_myTrack = m_timeline->findTrackWithId(m_trackId);
	if (!m_myTrack)
		return;



	// draw nodes
	qreal trackH = m_myTrack->ySize();

	painter->setBrush(QColor(0xffff00));
	for (int t=0; t<m_nodes.size(); t++) {
		const Node &n = m_nodes.at(t);
		painter->drawEllipse(QPointF(m_timeline->msToPixel(n.xMs), n.scaledY()), 3, 3);
	}
	painter->setPen(QColor(0xffff00));
	for (int t=1; t<m_nodes.size(); t++) {
		const Node &n1 = m_nodes.at(t-1);
		const Node &n2 = m_nodes.at(t);
		painter->drawLine(QPointF(m_timeline->msToPixel(n1.xMs), n1.scaledY()), QPointF(m_timeline->msToPixel(n2.xMs), n2.scaledY()));
	}
}



} // namespace PS_TL

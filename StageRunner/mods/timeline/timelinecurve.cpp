#include "timelinecurve.h"
#include "timelinewidget.h"

#include <QPainter>

namespace PS_TL {

TimeLineCurve::TimeLineCurve(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
{
	m_xSize = 500;
	m_xSize = 30;
	m_colorBG = 0x333355;

	m_nodes.append(Node(0, 1000));
	m_nodes.append(Node(60000, 1000));
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

QRectF TimeLineCurve::boundingRect() const
{
	QRectF br(0, 0, m_xSize, m_ySize);
	return br;
}

void TimeLineCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setBrush(m_colorBG);
	painter->drawRect(0, 0, m_xSize, m_ySize);

}

} // namespace PS_TL

#include "timelinecurve.h"
#include "timelinewidget.h"

#include <QPainter>

namespace PS_TL {

TimeLineCurve::TimeLineCurve(TimeLineWidget *timeline)
	: TimeLineBase(timeline)
{
	m_xSize = 500;
	m_xSize = 30;
	m_colorBG = 0x333355;
}

void TimeLineCurve::setTimeLineDuration(int ms)
{
	setDuration(ms);
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

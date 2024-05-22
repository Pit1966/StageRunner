#include "timelineitem.h"

namespace PS_TL {

TimeLineItem::TimeLineItem(TimeLineWidget *timeline, int trackId)
	: QGraphicsObject()
	, m_timeline(timeline)
	, m_trackId(trackId)
{
}

void TimeLineItem::setLabel(const QString &label)
{
	if (m_label != label) {
		m_label = label;
		update();
		emit labelChanged(label);
	}
}

void TimeLineItem::setBackgroundColor(const QColor &col)
{
	m_colorBG = col;
}

void TimeLineItem::setPosition(int ms)
{
	if (ms != m_positionMs || !m_wasPainted) {
		m_positionMs = ms;
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		update();
		emit timePositionChanged(ms);
	}
}

void TimeLineItem::setDuration(int ms)
{
	if (m_durationMs != ms) {
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		m_durationMs = ms;
		update();
		emit durationChanged(ms);
	}
}

qreal TimeLineItem::yPos() const
{
	return y();
}

void TimeLineItem::setYPos(qreal yPixelPos)
{
	setY(yPixelPos);
}


} // namespace PS_TL

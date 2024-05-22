#include "timelinebase.h"

namespace PS_TL {

TimeLineBase::TimeLineBase(TimeLineWidget *timeline, int trackId)
	: QGraphicsObject()
	, m_timeline(timeline)
	, m_trackId(trackId)
{
}

void TimeLineBase::setLabel(const QString &label)
{
	if (m_label != label) {
		m_label = label;
		update();
		emit labelChanged(label);
	}
}

void TimeLineBase::setBackgroundColor(const QColor &col)
{
	m_colorBG = col;
}

void TimeLineBase::setPosition(int ms)
{
	if (ms != m_positionMs || !m_wasPainted) {
		m_positionMs = ms;
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		update();
		emit timePositionChanged(ms);
	}
}

void TimeLineBase::setDuration(int ms)
{
	if (m_durationMs != ms) {
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		m_durationMs = ms;
		update();
		emit durationChanged(ms);
	}
}

qreal TimeLineBase::yPos() const
{
	return y();
}

void TimeLineBase::setYPos(qreal yPixelPos)
{
	setY(yPixelPos);
}


} // namespace PS_TL

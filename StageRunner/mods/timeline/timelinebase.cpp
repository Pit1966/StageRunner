#include "timelinebase.h"

namespace PS_TL {

TimeLineBase::TimeLineBase(TimeLineWidget *timeline)
	: QGraphicsObject()
	, m_timeline(timeline)
{
}

void TimeLineBase::setPosition(int ms)
{
	if (ms != m_positionMs) {
		m_positionMs = ms;
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		update();
	}
}

void TimeLineBase::setDuration(int ms)
{
	if (m_durationMs != ms) {
		m_isTimePosValid = true;
		m_isPixelPosValid = false;
		m_durationMs = ms;
		update();
	}
}

} // namespace PS_TL

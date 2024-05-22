#include "timelineclasses.h"

#include "timelineitem.h"

namespace PS_TL {

TimeLineTrack::TimeLineTrack(TRACK_TYPE type, int id, int y, int ySize)
	: m_type(type)
	, m_trackID(id)
	, m_yPos(y)
	, m_ySize(ySize)
{
}

TimeLineTrack::~TimeLineTrack()
{
	while (!m_itemList.isEmpty())
		delete m_itemList.takeFirst();
}

void TimeLineTrack::appendTimeLineItem(TimeLineBase *item)
{
	m_itemList.append(item);
}

TimeLineBase *TimeLineTrack::itemAt(int idx) const
{
	if (idx >= 0 && idx < m_itemList.size())
		return m_itemList.at(idx);
	return nullptr;
}

}

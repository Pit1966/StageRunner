#include "timelineclasses.h"

#include "timelinebox.h"

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

void TimeLineTrack::appendTimeLineItem(TimeLineItem *item)
{
	m_itemList.append(item);
}

TimeLineItem *TimeLineTrack::itemAt(int idx) const
{
	if (idx >= 0 && idx < m_itemList.size())
		return m_itemList.at(idx);
	return nullptr;
}

void TimeLineTrack::deleteAllItems()
{
	while (!m_itemList.isEmpty())
		delete m_itemList.takeFirst();
}

void TimeLineTrack::setTrackIdOfEachItem(int trackId, bool adjustYPosAlso)
{
	int id = trackId < 0 ? m_trackID : trackId;
	for (TimeLineItem *it : qAsConst(m_itemList)) {
		it->setTrackID(id);
		if (adjustYPosAlso)
			it->setYPos(m_yPos);
	}
}

}

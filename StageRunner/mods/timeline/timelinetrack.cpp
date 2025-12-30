#include "timelinetrack.h"

#include "timelinebox.h"
#include "timelinewidget.h"

namespace PS_TL {

TimeLineTrack::TimeLineTrack(TimeLineWidget *timeline, TRACK_TYPE type, int id, int y, int ySize)
	: m_timeline(timeline)
	, m_type(type)
	, m_trackID(id)
	, m_yPos(y)
	, m_ySize(ySize)
	, m_isOverlay(false)
{
}

TimeLineTrack::~TimeLineTrack()
{
	while (!m_itemList.isEmpty())
		delete m_itemList.takeFirst();
}

void TimeLineTrack::setOverlay(bool state)
{
	if (m_isOverlay != state) {
		m_isOverlay = state;
	}
}

void TimeLineTrack::appendTimeLineItem(TimeLineItem *item)
{
	m_itemList.append(item);
}

bool TimeLineTrack::removeTimeLineItem(TimeLineItem *item)
{
	return m_itemList.removeOne(item);
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
	for (TimeLineItem *it : std::as_const(m_itemList)) {
		it->setTrackID(id);
		if (adjustYPosAlso)
			it->setYPos(m_yPos);
	}
}

void TimeLineTrack::setTrackDuration(int ms)
{
	for (int t=0; t<m_itemList.size(); t++) {
		itemAt(t)->setTrackDuration(ms);
	}
}

void TimeLineTrack::alignItemPositionsToTrack()
{
	for (TimeLineItem* item : std::as_const(m_itemList)) {
		item->setYSize(m_ySize - yAlignSize);
		item->setYPos(m_yPos + yAlignOffset);
	}
}

bool TimeLineTrack::hasOverlayContextMenu() const
{
	// joined track is the track below
	if (m_next) {
		if (m_next->isOverlay() && m_next->trackType() == TRACK_AUDIO_CURVE) {
			return true;
		}
	}
	return false;
}

QList<TimeLineContextMenuEntry> TimeLineTrack::getOverlayContextMenu(const QPointF &scenePos)
{
	// linked track or overlay is the track below
	if (m_next) {
		if (m_next->isOverlay() && m_next->trackType() == TRACK_AUDIO_CURVE) {
			return m_next->m_itemList.first()->getOverlayContextMenu(scenePos);
		}
	}
	return {};
}

TimeLineTrack *TimeLineTrack::trackAbove()
{
	if (m_trackID > 2)
		return m_timeline->findTrackAboveId(m_trackID);

	return nullptr;
}

QString TimeLineTrack::trackTypeToString(TRACK_TYPE type)
{
	switch (type) {
	case TRACK_ITEMS:
		return "ITEMS";
	case TRACK_AUDIO_CURVE:
		return "AUDIO_CURVE";
	case TRACK_RULER:
		return "RULER";
	default:
		return "UNKNOWN";
	}
}

}

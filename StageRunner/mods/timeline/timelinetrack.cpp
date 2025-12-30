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
		if (m_next->isOverlay() && m_next->trackType() >= TRACK_CURVES) {
			return true;
		}
	}
	return false;
}

QList<TimeLineContextMenuEntry> TimeLineTrack::getOverlayContextMenu(const QPointF &scenePos)
{
	QList<TimeLineContextMenuEntry> list;

	// linked track or overlay is the track below
	if (m_next) {
		if (m_next->isOverlay() && m_next->trackType() >= TRACK_CURVES) {
			list.append(m_next->m_itemList.first()->getOverlayContextMenu(scenePos));
			// Maybe, we have another overlay
			TimeLineTrack *nnt = m_next->m_next;
			if (nnt && nnt->isOverlay() && nnt->trackType() >= TRACK_CURVES)
				list.append(nnt->m_itemList.first()->getOverlayContextMenu(scenePos));
		}
	}
	return list;
}

/**
 * @brief Search track above the current one (using the trackId)
 * @return
 *
 * This function does not return the ruler track.
 * @todo could be done with m_next/m_prev
 */
TimeLineTrack *TimeLineTrack::trackAbove()
{
	if (m_trackID > 2)
		return m_timeline->findTrackAboveId(m_trackID);

	return nullptr;
}

TimeLineTrack *TimeLineTrack::nextTrack()
{
	return m_next;
}

TimeLineTrack *TimeLineTrack::prevTrack()
{
	if (m_prev && m_prev->trackType() != TRACK_RULER)
		return m_prev;

	return nullptr;
}

QString TimeLineTrack::trackTypeToString(TRACK_TYPE type)
{
	switch (type) {
	case TRACK_ITEMS:
		return "ITEMS";
	case TRACK_RULER:
		return "RULER";
	case TRACK_AUDIO_VOL_CURVE:
		return "AUDIO VOL CURVE";
	case TRACK_AUDIO_PAN_CURVE:
		return "AUDIO PAN CURVE";
	default:
		return "UNKNOWN";
	}
}

}

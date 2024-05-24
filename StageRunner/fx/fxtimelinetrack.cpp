#include "fxtimelinetrack.h"

using namespace PS_TL;

FxTimeLineTrack::FxTimeLineTrack(TimeLineWidget *timeline, TRACK_TYPE type, int id, int y, int size)
	: TimeLineTrack(timeline, type, id)
{
	init();

	// We have to do the initialisation again, because init() has set the default values
	m_type = type;
	m_trackID = id;
	m_yPos = y;
	m_ySize = size;
}

bool FxTimeLineTrack::isEqual(TimeLineTrack *o) const
{
	return trackBgColor == o->trackBgColor
			&& m_type == o->trackType()
			&& m_trackID == o->trackId()
			&& m_yPos == o->yPos()
			&& m_ySize == o->ySize();
}

void FxTimeLineTrack::copyDataFrom(TimeLineTrack *o)
{
	trackBgColor = o->trackBgColor;
	m_type = o->trackType();
	m_trackID = o->trackId();
	m_yPos = o->yPos();
	m_ySize = o->ySize();
}

void FxTimeLineTrack::copyDataTo(TimeLineTrack *o)
{
	o->trackBgColor = trackBgColor;
	o->setTrackType(m_type);
	o->setTrackId(m_trackID);
	o->setYPos(m_yPos);
	o->setYSize(m_ySize);
}

void FxTimeLineTrack::init()
{
	setClass(PrefVarCore::FX_TIMELINE_TRACK,"TLTrack");

	addExistingVar(trackBgColor, "BgColor");
	addExistingVar((qint32&)m_type, "TrackType");
	addExistingVar(m_trackID, "TrackID");
	addExistingVar(m_yPos, "yPos");
	addExistingVar(m_ySize, "ySize");
}

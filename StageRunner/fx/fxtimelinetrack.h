#ifndef FXTIMELINETRACK_H
#define FXTIMELINETRACK_H

#include "tool/varset.h"
// TimeLineTrack is in timelineclasses.h
#include "mods/timeline/timelinetrack.h"
#include <QtCore>

class FxTimeLineTrack : public VarSet, public PS_TL::TimeLineTrack
{
public:
	FxTimeLineTrack(PS_TL::TimeLineWidget *timeline, PS_TL::TRACK_TYPE type = PS_TL::TRACK_UNDEF, int id = 0, int y = 0, int size = 24);
	bool isEqual(PS_TL::TimeLineTrack *o) const;
	void copyDataFrom(PS_TL::TimeLineTrack *o);
	void copyDataTo(PS_TL::TimeLineTrack *o);

private:
	void init();

};

#endif // FXTIMELINETRACK_H

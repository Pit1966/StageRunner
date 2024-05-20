#ifndef FXTIMELINETRACK_H
#define FXTIMELINETRACK_H

// TimeLineTrack is ins timelineclasses.h
#include "mods/timeline/timelineclasses.h"
#include "tool/varset.h"

class FxTimeLineTrack : public VarSet, public PS_TL::TimeLineTrack
{
public:
	FxTimeLineTrack(PS_TL::TRACK_TYPE type, int id, int y = 0, int size = 24);

private:
	void init();

};

#endif // FXTIMELINETRACK_H

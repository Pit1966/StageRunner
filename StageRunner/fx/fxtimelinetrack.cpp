#include "fxtimelinetrack.h"

using namespace PS_TL;

FxTimeLineTrack::FxTimeLineTrack(TRACK_TYPE type, int id, int y, int size)
	: TimeLineTrack(type, id, y, size)
{

}

void FxTimeLineTrack::init()
{
	setClass(PrefVarCore::FX_TIMELINE_TRACK,"TLTrack");
	// addExistingVar();
}

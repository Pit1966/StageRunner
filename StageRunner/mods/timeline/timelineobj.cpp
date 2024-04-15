#include "fxtimelineobj.h"

FxTimeLineObj::FxTimeLineObj(int posMs, int durationMs, const QString &label, int trackID)
{
	init();

	this->trackId = trackID;
	this->posMs = posMs;
	this->lenMs = durationMs;
	this->label = label;
}

FxTimeLineObj::FxTimeLineObj(const FxTimeLineObj &o)
	: VarSet()
{
	init();
	trackId = o.trackId;
	posMs = o.posMs;
	lenMs = o.lenMs;
	label = o.label;
}

void FxTimeLineObj::clear()
{
	// implement me

}

void FxTimeLineObj::init()
{
	setClass(PrefVarCore::TIMELINE_OBJ,"TLObj");

	addExistingVar(trackId, "TrackID");
	addExistingVar(posMs,"PosMs");
	addExistingVar(lenMs,"LenMs");
	addExistingVar(label, "Label");
}

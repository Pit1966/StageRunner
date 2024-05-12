#include "fxtimelineobj.h"

FxTimeLineObj::FxTimeLineObj(int posMs, int durationMs, const QString &label, int trackID)
	: VarSet()
	, TimeLineItemData()
{
	init();

	this->trackId = trackID;
	this->posMs = posMs;
	this->lenMs = durationMs;
	this->label = label;
}

void FxTimeLineObj::clear()
{
	// implement me
}

bool FxTimeLineObj::operator ==(const FxTimeLineObj &o)
{
	return this->isEqual(&o);
}

bool FxTimeLineObj::isEqual(const FxTimeLineObj *o)
{
	return TimeLineItemData::isEqual(o)
			&& trackId == o->trackId
			&& posMs == o->posMs
			&& lenMs == o->lenMs
			&& label == o->label;
}

void FxTimeLineObj::init()
{
	setClass(PrefVarCore::TIMELINE_OBJ,"TLObj");

	addExistingVar(trackId, "TrackID");
	addExistingVar(posMs,"PosMs");
	addExistingVar(lenMs,"LenMs");
	addExistingVar(label, "Label");
	addExistingVar(reinterpret_cast<int&>(m_linkedObjType), "Type");
	addExistingVar(m_fxID, "FxId");
	addExistingVar(m_maxDurationMs, "MaxDurMs");
}

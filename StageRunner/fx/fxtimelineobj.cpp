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
	// cloneFrom(o);
	trackId = o.trackId;
	posMs = o.posMs;
	lenMs = o.lenMs;
	label = o.label;
	m_linkedObjType = o.m_linkedObjType;
	m_fxID = o.m_fxID;
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
	return m_linkedObjType == o->m_linkedObjType
			&& trackId == o->trackId
			&& posMs == o->posMs
			&& lenMs == o->lenMs
			&& label == o->label
			&& m_fxID == o->m_fxID;
}

void FxTimeLineObj::init()
{
	setClass(PrefVarCore::TIMELINE_OBJ,"TLObj");

	addExistingVar(trackId, "TrackID");
	addExistingVar(posMs,"PosMs");
	addExistingVar(lenMs,"LenMs");
	addExistingVar(label, "Label");
	addExistingVar(m_linkedObjType, "Type");
	addExistingVar(m_fxID, "FxId");
}

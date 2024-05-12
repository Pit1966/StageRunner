#include "fxtimelineobj.h"
#include "system/log.h"
#include "fx/fxitem_includes.h"

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

/**
 * @brief startTime of timeline object
 * @return absolute time [ms] in relation to timeline origin
 */
int FxTimeLineObj::startAtMs() const
{
	return posMs;
}

/**
 * @brief stopPosition of timeline object
 * @return absolute time in [ms] when the last command is executed to finish the timeline object. returns 0 on error
 *
 *  @note the finish time is different to the end point of the timeline object, which is posMs + durationMs = lenMs
 *  stopAtMs() must always return a time < endMs();
 */
int FxTimeLineObj::stopAtMs() const
{
	if (m_linkedObjType == LINKED_FX_SCENE) {
		// get linked FxItem
		FxItem *fx = linkedFxItem();
		if (!fx) {
			LOGERROR(tr("Scene FX for timeline object '%1' not found!").arg(label));
			return 0;
		}
		return endMs() - fx->fadeOutTime();
	}
	else if (m_linkedObjType == LINKED_FX_AUDIO) {
		// get linked FxItem
		FxItem *fx = linkedFxItem();
		if (!fx) {
			LOGERROR(tr("Audi FX for timeline object '%1' not found!").arg(label));
			return 0;
		}
		return endMs();
	}
	else if (m_linkedObjType == LINKED_FX_SCRIPT) {
		// get linked FxItem
		FxItem *fx = linkedFxItem();
		if (!fx) {
			LOGERROR(tr("Script FX for timeline object '%1' not found!").arg(label));
			return 0;
		}
		return endMs();
	}

	return 0;
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

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

FxTimeLineObj::FxTimeLineObj(const FxTimeLineObj &o)
{
	init();
	VarSet::cloneFrom(o);
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
	bool iseq = TimeLineItemData::isEqual(o);
	return iseq
			&& trackId == o->trackId
			&& posMs == o->posMs
			&& lenMs == o->lenMs
			&& label == o->label
			&& configDat == o->configDat;
}

/**
 * @brief startTime of timeline object
 * @return absolute time [ms] in relation to timeline origin
 */
int FxTimeLineObj::startAtMs() const
{
	return posMs;
}

int FxTimeLineObj::fadeInDurationMs() const
{
	int fadeinms = 0;
	switch (m_linkedObjType) {
	case LINKED_FX_SCENE:
	case LINKED_FX_AUDIO:
		if (m_fadeInDurationMs > 0) {
			fadeinms = m_fadeInDurationMs;
		}
		else {
			// get linked FxItem
			FxItem *fx = linkedFxItem();
			if (fx)
				fadeinms = fx->fadeInTime();
			else
				LOGERROR(tr("linked FX for timeline object '%1' not found!").arg(label));
		}
		break;
	default:
		break;
	}

	return fadeinms;
}

int FxTimeLineObj::fadeOutDurationMs() const
{
	int fadeoutms = 0;
	switch (m_linkedObjType) {
	case LINKED_FX_SCENE:
	case LINKED_FX_AUDIO:
		if (m_fadeOutDurationMs > 0) {
			fadeoutms = m_fadeOutDurationMs;
		}
		else {
			// get linked FxItem
			FxItem *fx = linkedFxItem();
			if (!fx) {
				LOGERROR(tr("linked FX for timeline object '%1' not found!").arg(label));
			} else {
				fadeoutms = fx->fadeOutTime();
			}
		}
		break;
	default:
		break;
	}

	return fadeoutms;
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
		if (m_fadeOutDurationMs > 0)
			return endMs() - m_fadeOutDurationMs;
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
			LOGERROR(tr("Audio FX for timeline object '%1' not found!").arg(label));
			return 0;
		}
		int fadeout_ms = fadeOutDurationMs();
		if (fadeout_ms > 0) {
			return endMs() - fadeout_ms;
		} else {
			return endMs();
		}
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

bool FxTimeLineObj::linkObjToFxItem(FxItem *fx)
{
	if (!fx)
		return false;

	m_fxID = fx->id();

	int minLenMs = -1;
	// preset of base time values, such as minimal execution duration of item
	switch (fx->fxType()) {
	case FX_SCENE:
		m_linkedObjType = LINKED_FX_SCENE;
		minLenMs = fx->durationHint();
		if (minLenMs <= 0)	// set default duration to 2 seconds, if there is no other value given.
			minLenMs = 2000;
		m_maxDurationMs = minLenMs;
		// m_colorBG = 0x923d0c;
		// m_colorBorder = 0x923d0c;
		break;
	case FX_AUDIO:
		m_linkedObjType = LINKED_FX_AUDIO;
		minLenMs = fx->durationHint();
		if (minLenMs <= 0)
			minLenMs = 30000;
		m_maxDurationMs = minLenMs;
		break;
	case FX_SCRIPT:
		m_linkedObjType = LINKED_FX_SCRIPT;
		minLenMs = fx->durationHint();
		m_maxDurationMs = minLenMs;
		// m_colorBG = 0x413f32;
		// m_colorBorder = 0x413f32;
		break;

	default:
		return false;
	}

	return true;
}

void FxTimeLineObj::init()
{
	setClass(PrefVarCore::TIMELINE_OBJ,"TLObj");

	addExistingVar(trackId, "TrackID");
	addExistingVar(posMs,"PosMs");
	addExistingVar(lenMs,"LenMs");
	addExistingVar(label, "Label");
	addExistingVar(configDat, "configDat");
	// in TimeLineData
	addExistingVar(reinterpret_cast<int&>(m_linkedObjType), "Type");
	addExistingVar(m_fxID, "FxId");
	addExistingVar(m_maxDurationMs, "MaxDurMs");
	addExistingVar(m_fadeInDurationMs, "FadeInMs");
	addExistingVar(m_fadeOutDurationMs, "FadeOutMs");

}

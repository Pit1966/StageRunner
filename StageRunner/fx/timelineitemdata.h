#ifndef TIMELINEITEMDATA_H
#define TIMELINEITEMDATA_H

#include "fx/fx_defines.h"

class FxItem;

class TimeLineItemData
{
protected:
	// FxTimeLineItem parameters
	LINKED_OBJ_TYPE m_linkedObjType	= LINKED_UNDEF;
	int m_fxID						= 0;			// this is the FxID for type LINKED_FXIEM
	int m_maxDurationMs				= 0;			// this is set by linked FxItems: Audio, Script, Clip
	int m_fadeInDurationMs			= 0;			// this is the time from start to full fade in [ms]
	int m_fadeOutDurationMs			= 0;			// this is the time from fadeout begin positon up to the end [ms]


	// temp
	mutable FxItem *m_fxItemP	= nullptr;

public:
	bool operator ==(const TimeLineItemData &o) {
		return isEqual(&o);
	}
	bool isEqual(const TimeLineItemData *o) {
		return m_linkedObjType == o->m_linkedObjType
				&& m_fxID == o->m_fxID
				&& m_maxDurationMs == o->m_maxDurationMs
				&& m_fadeInDurationMs == o->m_fadeInDurationMs
				&& m_fadeOutDurationMs == o->m_fadeOutDurationMs;
	}
	void cloneItemDataFrom(const TimeLineItemData *o) {
		m_linkedObjType = o->m_linkedObjType;
		m_fxID = o->m_fxID;
		m_maxDurationMs = o->m_maxDurationMs;
		m_fadeInDurationMs = o->m_fadeInDurationMs;
		m_fadeOutDurationMs = o->m_fadeOutDurationMs;
	}
	FxItem * linkedFxItem() const;
	LINKED_OBJ_TYPE linkedObjType() const {return m_linkedObjType;}
};

#endif // TIMELINEITEMDATA_H

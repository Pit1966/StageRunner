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

	// temp
	mutable FxItem *m_fxItemP	= nullptr;

public:
	bool operator ==(const TimeLineItemData &o) {
		return isEqual(&o);
	}
	bool isEqual(const TimeLineItemData *o) {
		return m_linkedObjType == o->m_linkedObjType
				&& m_fxID == o->m_fxID
				&& m_maxDurationMs == o->m_maxDurationMs;
	}
	void cloneItemDataFrom(const TimeLineItemData *o) {
		m_linkedObjType = o->m_linkedObjType;
		m_fxID = o->m_fxID;
		m_maxDurationMs = o->m_maxDurationMs;
	}
	FxItem * linkedFxItem() const;
};

#endif // TIMELINEITEMDATA_H

#include "timelineitemdata.h"

#include "fx/fxitem_includes.h"

FxItem *TimeLineItemData::linkedFxItem() const
{
	if (m_fxItemP && FxItem::exists(m_fxItemP))
		return m_fxItemP;

	m_fxItemP = FxItem::findFxById(m_fxID);
	return m_fxItemP;
}

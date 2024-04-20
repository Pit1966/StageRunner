#include "fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/exttimelineitem.h"
#include "mods/timeline/timelineitem.h"

FxTimeLineEditWidget::FxTimeLineEditWidget()
{

}

bool FxTimeLineEditWidget::setFxTimeLineItem(FxTimeLineItem *fxt)
{
	m_curFxItem = fxt;

	// clear all tracks (and items) in the widget
	clear();

	// Each track has a its own list with item/objs
	// t is also the trackID !!
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		int trackID = t;
		// get references to the source and destination list for this track
		const VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];
		// QList<TimeLineItem*> &timelinelist = m_itemLists[t];

		// now copy the elements of the track
		// we have to convert a FxTimeLineObj which is used by the fx control unit to an TimeLineItem, which
		// is used in the TimeLineWidget

		for (int i=0; i<varset.size(); i++) {
			FxTimeLineObj *obj = varset.at(i);
			TimeLineItem *tli = addTimeLineItem(obj->posMs, obj->lenMs, obj->label, trackID);
			ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);
			extTLI->m_fxID = obj->m_fxID;
			extTLI->m_linkedObjType = LINKED_OBJ_TYPE(obj->m_linkedObjType);
		}
	}

	return true;
}

bool FxTimeLineEditWidget::copyToFxTimeLineItem(FxTimeLineItem *fxt)
{
	if (!fxt)
		return false;

	// clear all tracks (and FxTimeLineObjs) in the FxTimeLineItem
	// fxt->clear();

	// Each track has a its own list with item/objs
	// t is also the trackID !!
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		int trackID = t;
		// get references to the source and destination list for this track
		// const QList<TimeLineItem*> &timelinelist = m_itemLists[t];
		VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];

		int i = -1;
		// now copy the elements of the track
		// we have to convert a FxTimeLineObj which is used by the fx control unit to an TimeLineItem, which
		// is used in the TimeLineWidget
		while (++i < m_itemLists[trackID].size()) {
			TimeLineItem *tli = at(trackID, i);
			ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);

			FxTimeLineObj *obj = new FxTimeLineObj(tli->position(), tli->duration(), tli->label(), tli->trackID());
			if (extTLI) {
				obj->m_fxID = extTLI->m_fxID;
				obj->m_linkedObjType = extTLI->m_linkedObjType;
			}

			if (i < varset.size()) {
				if (varset.at(i)->isEqual(obj)) {
					// item not modified
					delete obj;
				} else {
					// delete old item in timeline
					delete varset.at(i);
					// and set new one at the same position
					varset[i] = obj;
					fxt->setModified(true);
				}
			}
			else {
				varset.append(obj);
				fxt->setModified(true);
			}
		}
	}

	return true;
}

void FxTimeLineEditWidget::closeEvent(QCloseEvent */*event*/)
{
	qDebug() << "close event" << m_curFxItem;
	if (m_curFxItem)
		copyToFxTimeLineItem(m_curFxItem);

	deleteLater();
}

TimeLineItem *FxTimeLineEditWidget::createNewTimeLineItem(TimeLineWidget *timeline, int trackId)
{
	ExtTimeLineItem * tlItem = new ExtTimeLineItem(timeline, trackId);
	qDebug() << "new item";
	return tlItem;
}

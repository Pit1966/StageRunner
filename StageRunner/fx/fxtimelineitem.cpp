#include "fxtimelineitem.h"

#include "fxtimelineobj.h"


FxTimeLineItem::FxTimeLineItem(FxList *fxList)
	: QObject()
	, FxItem(fxList)
	, m_timelines{}
	, m_timeLineDurationMs(0)
{
	init();
}

FxTimeLineItem::FxTimeLineItem(const FxTimeLineItem &o)
	: QObject()
	, FxItem(o.myParentFxList)
	, m_timelines{}
	, m_timeLineDurationMs(0)
{
	init();
	cloneFrom(o);

	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		for (int i=0; i<o.m_timelines[t].size(); i++) {
			FxTimeLineObj *obj = new FxTimeLineObj(*o.m_timelines[t].at(i));
			m_timelines[t].append(obj);
		}
	}
}

FxTimeLineItem::~FxTimeLineItem()
{

}

void FxTimeLineItem::clear()
{
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		m_timelines[t].clear();
	}
}

int FxTimeLineItem::timeLineObjCount(uint trackID) const
{
	if (trackID < TIMELINE_MAX_TRACKS)
		return m_timelines[trackID].size();

	return 0;
}

FxTimeLineObj *FxTimeLineItem::timeLineObjAt(uint trackID, int idx) const
{
	if (trackID < TIMELINE_MAX_TRACKS)
		return m_timelines[trackID].at(idx);

	return nullptr;
}

qint32 FxTimeLineItem::loopValue() const
{
	return 0;
}

void FxTimeLineItem::setLoopValue(qint32 val)
{
	Q_UNUSED(val)
}

void FxTimeLineItem::resetFx()
{
}

void FxTimeLineItem::init()
{
	myFxType = FX_TIMELINE;
	myclass = PrefVarCore::FX_TIMELINE_ITEM;

	addExistingVar(m_timeLineDurationMs, "DurationMs");
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		m_timelines[t].parentVoid = this;
		addExistingVarSetList(m_timelines[t], QString("Timeline%1").arg(t), PrefVarCore::TIMELINE_OBJ);
	}
}

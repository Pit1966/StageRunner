#ifndef FXTIMELINEITEM_H
#define FXTIMELINEITEM_H

#include "fxitem.h"
#include "tool/varset.h"
#include "tool/varsetlist.h"
#include "mods/timeline/timeline_defines.h"
#include "fx/fx_defines.h"

#include <QCoreApplication>

class FxTimeLineObj;

class FxTimeLineItem : public QObject, public FxItem
{
	Q_OBJECT			// Q_DECLARE_TR_FUNCTIONS(FxTimeLineItem)

private:
	VarSetList<FxTimeLineObj*>m_timelines[TIMELINE_MAX_TRACKS];

public:
	FxTimeLineItem(FxList *fxList);
	FxTimeLineItem(const FxTimeLineItem &o);
	virtual ~FxTimeLineItem();

	void clear();
	int timeLineObjCount(uint trackID) const;
	FxTimeLineObj *timeLineObjAt(uint trackID, int idx) const;

	qint32 loopValue() const override;
	void setLoopValue(qint32 val) override;
	void resetFx() override;

private:
	void init();

friend class FxTimeLineEditWidget;

};

#endif // FXTIMELINEITEM_H

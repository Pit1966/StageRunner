#include "fxtimelineitem.h"

FxTimeLineItem::FxTimeLineItem(FxList *fxList)
	: FxItem(fxList)
{
	init();
}

FxTimeLineItem::FxTimeLineItem(const FxTimeLineItem &o)
	: FxItem(o.myParentFxList)
{
	init();
	cloneFrom(o);
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
}

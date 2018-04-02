#include "fxcueitem.h"
//#include "fxlist.h"


FxCueItem::FxCueItem(FxList *fxList)
	: FxItem(fxList)
	, m_init(false)
{
}

FxCueItem::FxCueItem(const FxCueItem &o)
	: FxItem(o)
	, m_init(o.m_init)
{
}

FxCueItem::~FxCueItem()
{

}

qint32 FxCueItem::loopValue() const
{
	return 0;
}

void FxCueItem::setLoopValue(qint32 val)
{
	Q_UNUSED(val)
}

void FxCueItem::resetFx()
{

}

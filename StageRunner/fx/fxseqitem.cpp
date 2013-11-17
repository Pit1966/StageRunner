#include "fxseqitem.h"

#include "fxlist.h"
#include "fxitemobj.h"
#include "appcentral.h"
#include "fxsceneitem.h"
#include "fxaudioitem.h"

FxSeqItem::FxSeqItem()
	: FxItem()
	, seqList(new FxList)
	, itemObj(new FxItemObj(this))
{
	init();
}

FxSeqItem::~FxSeqItem()
{
	delete seqList;
	delete itemObj;
}

qint32 FxSeqItem::fadeInTime()
{
	return 0;
}

void FxSeqItem::setFadeInTime(qint32 val)
{
	Q_UNUSED(val);
}

qint32 FxSeqItem::fadeOutTime()
{
	return 0;
}

void FxSeqItem::setFadeOutTime(qint32 val)
{
	Q_UNUSED(val);
}

void FxSeqItem::init()
{
	myFxType = FX_SEQUENCE;
	myclass = PrefVarCore::FX_SEQUENCE_ITEM;
	addExistingVar(widgetPos,"seqWidgetPos");
	addExistingVarSetList(seqList->nativeFxList(),"Sequence",PrefVarCore::FX_ITEM);
}

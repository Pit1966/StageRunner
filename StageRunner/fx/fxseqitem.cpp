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

qint32 FxSeqItem::loopValue()
{
	return loopTimes;
}

void FxSeqItem::setLoopValue(qint32 val)
{
	loopTimes = val;
}

void FxSeqItem::init()
{
	myFxType = FX_SEQUENCE;
	myclass = PrefVarCore::FX_SEQUENCE_ITEM;
	addExistingVar(widgetPos,"seqWidgetPos");
	addExistingVar(loopTimes,"LoopTimes");
	addExistingVar(seqList->showColumnIdFlag,"FxListShowId");
	addExistingVar(seqList->showColumnPredelayFlag,"FxListShowPreDelay");
	addExistingVar(seqList->showColumnFadeinFlag,"FxListShowFadeInTime");
	addExistingVar(seqList->showColumnHoldFlag,"FxListShowHoldTime");
	addExistingVar(seqList->showColumnFadeoutFlag,"FxListShowFadeOutTime");
	addExistingVar(seqList->showColumnPostdelayFlag,"FxListShowPostDelay");

	addExistingVarSetList(seqList->nativeFxList(),"Sequence",PrefVarCore::FX_ITEM);
}

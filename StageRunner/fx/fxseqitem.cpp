//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "fxseqitem.h"

#include "fxlist.h"
#include "fxitemobj.h"
#include "appcentral.h"
#include "fxsceneitem.h"
#include "fxaudioitem.h"
#include "fxitemtool.h"

FxSeqItem::FxSeqItem(FxList *fxList)
	: FxItem(fxList)
	, seqList(new FxList(this))
	, itemObj(new FxItemObj(this))
{
	init();
}

FxSeqItem::FxSeqItem(const FxSeqItem &o)
	: FxItem(o.parentFxList())
	, seqList(new FxList(this))
	, itemObj(new FxItemObj(this))
{
	init();
	copyFrom(o);
}

FxSeqItem::~FxSeqItem()
{
	delete seqList;
	delete itemObj;
}

void FxSeqItem::copyFrom(const FxSeqItem &o)
{
	cloneFrom(o);
	seqList->copyFrom(*o.seqList);
	setLoopValue(o.loopValue());
	blackOtherSeqOnStart = o.blackOtherSeqOnStart;
	stopOtherSeqOnStart = o.stopOtherSeqOnStart;
}

qint32 FxSeqItem::fadeInTime() const
{
	return 0;
}

void FxSeqItem::setFadeInTime(qint32 val)
{
	Q_UNUSED(val);
}

qint32 FxSeqItem::fadeOutTime() const
{
	return 0;
}

void FxSeqItem::setFadeOutTime(qint32 val)
{
	Q_UNUSED(val);
}

qint32 FxSeqItem::loopValue() const
{
	return seqList->myLoopTimes;
}

void FxSeqItem::setLoopValue(qint32 val)
{
	seqList->setLoopTimes(val);
}

void FxSeqItem::resetFx()
{

}

bool FxSeqItem::isRandomized() const
{
	return seqList->myRandomizedFlag;
}

void FxSeqItem::setRandomized(bool state)
{
	seqList->setRandomized(state);
}

void FxSeqItem::init()
{
	myFxType = FX_SEQUENCE;
	myclass = PrefVarCore::FX_SEQUENCE_ITEM;
	addExistingVar(widgetPos,"WidgetPos");
	addExistingVar(blackOtherSeqOnStart,"BlackOtherSeqOnStart");
	addExistingVar(stopOtherSeqOnStart,"StopOtherSeqOnStart");
	addExistingVar(seqList->myLoopFlag,"ListIsLooped");
	addExistingVar(seqList->myLoopTimes,"LoopTimes");
	addExistingVar(seqList->myRandomizedFlag,"RandomizedList");
	addExistingVar(seqList->myAutoProceedFlag,"AutoProceedList");
	addExistingVar(seqList->showColumnFadeinFlag,"FxListShowFadeInTime");
	addExistingVar(seqList->showColumnFadeoutFlag,"FxListShowFadeOutTime");
	addExistingVar(seqList->showColumnIdFlag,"FxListShowId");
	addExistingVar(seqList->showColumnHoldFlag,"FxListShowHoldTime");
	addExistingVar(seqList->showColumnPredelayFlag,"FxListShowPreDelay");
	addExistingVar(seqList->showColumnPostdelayFlag,"FxListShowPostDelay");
	addExistingVar(seqList->showColumnKeyFlag,"FxListShowKey");
	addExistingVar(seqList->showColumnMoveFlag,"FxListShowMoveDelay");


	addExistingVarSetList(seqList->nativeFxList(),"Sequence",PrefVarCore::FX_ITEM);
}

//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef FXSEQITEM_H
#define FXSEQITEM_H

#include "fxitem.h"

class FxList;
class FxItemObj;

class FxSeqItem : public FxItem
{
public:
	FxList *seqList;
	pbool blackOtherSeqOnStart;
	pbool stopOtherSeqOnStart;

private:

	pstring widgetPos;
	FxItemObj *itemObj;

public:
	FxSeqItem(FxList *fxList);
	FxSeqItem(const FxSeqItem &o);
	~FxSeqItem();

	void copyFrom(const FxSeqItem &o);

	qint32 fadeInTime() const override;
	void setFadeInTime(qint32 val) override;
	qint32 fadeOutTime() const override;
	void setFadeOutTime(qint32 val) override;
	qint32 loopValue() const override;
	void setLoopValue(qint32 val) override;
	void resetFx() override;
	bool isRandomized() const override;
	void setRandomized(bool state) override;
	QString widgetPosition() const override {return widgetPos;}
	void setWidgetPosition(const QString & geometry) override {widgetPos = geometry;}
	FxList *fxList() const override {return seqList;}

private:
	void init();
};

#endif // FXSEQITEM_H

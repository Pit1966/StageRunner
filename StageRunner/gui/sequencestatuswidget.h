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

#ifndef SEQUENCESTATUSWIDGET_H
#define SEQUENCESTATUSWIDGET_H

#include "ui_sequencestatuswidget.h"
#include <QReadWriteLock>
#include <QHash>
#include <QListWidget>

class FxSeqItem;
class FxItem;
class Executer;

class SeqStatusListItem : public QListWidgetItem
{
protected:
	Executer *myExec;
	FxItem *myFx;

public:
	SeqStatusListItem(Executer *exec, FxItem *fx, QListWidget *list = 0);
	inline FxItem * fxItem() const {return myFx;}
	inline void setFxItem(FxItem *fx) {myFx = fx;}
};


class SequenceStatusWidget : public QWidget, public Ui::SequenceStatusWidget
{
	Q_OBJECT
private:
	QReadWriteLock *rwlock;
	QHash<FxItem*,SeqStatusListItem*>fx_hash;
	QHash<Executer*,SeqStatusListItem*>exec_hash;

public:
	SequenceStatusWidget(QWidget *parent = 0);
	~SequenceStatusWidget();

	bool appendSequence(FxSeqItem *seq);
	bool removeSequence(FxSeqItem *seq);
	bool updateSequence(FxSeqItem *seq);

	bool appendExecuter(Executer *exec);
	bool removeExecuter(Executer *exec);
	bool updateExecuter(Executer *exec);

private:
	void update_display();

public slots:
	void propagateExecuter(Executer *exec);
	void insertExecuter(Executer *exec);
	void scratchExecuter(Executer *exec);

};

#endif // SEQUENCESTATUSWIDGET_H

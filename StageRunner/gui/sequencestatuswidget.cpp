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

#include "sequencestatuswidget.h"
#include "fxseqitem.h"
#include "executer.h"

#define READLOCK QReadLocker lock(rwlock);Q_UNUSED(lock);
#define WRITELOCK QWriteLocker lock(rwlock);Q_UNUSED(lock);
#define UNLOCK lock.unlock();

/// @todo this class uses locking, but this is probably not necessary, cause this is
/// a widget class. We do not need locking if all functions are called from main thread only.

SeqStatusListItem::SeqStatusListItem(Executer *exec, FxItem *fx, QListWidget *list)
	: QListWidgetItem(list)
	, myExec(exec)
	, myFx(fx)
{
}

SequenceStatusWidget::SequenceStatusWidget(QWidget *parent) :
	QWidget(parent)
{
	rwlock = new QReadWriteLock(QReadWriteLock::Recursive);
	setupUi(this);
}

SequenceStatusWidget::~SequenceStatusWidget()
{
	delete rwlock;
}


bool SequenceStatusWidget::appendSequence(FxSeqItem *seq)
{
	///@todo: Find Executer for initialisation of SeqStatusListItem
	WRITELOCK;
	SeqStatusListItem *item = new SeqStatusListItem(0,seq,seqListWidget);
	item->setText(seq->name());
	fx_hash.insert(seq,item);

	return true;
}

bool SequenceStatusWidget::removeSequence(FxSeqItem *seq)
{
	bool removed = false;
	WRITELOCK;

	SeqStatusListItem *item = fx_hash.value(seq);
	if (item) {
		delete item;
		removed = true;
	}
	fx_hash.remove(seq);

	return removed;
}

bool SequenceStatusWidget::updateSequence(FxSeqItem *seq)
{
	WRITELOCK;
	SeqStatusListItem *item = fx_hash.value(seq);
	if (item) {

		return true;
	}

	return false;
}

bool SequenceStatusWidget::appendExecuter(Executer *exec)
{
	WRITELOCK;
	FxItem *fx = exec->originFx();
	// if (fx && fx->fxType() != FX_SEQUENCE) return false;

	QString name;

	SeqStatusListItem *item = new SeqStatusListItem(exec,fx,seqListWidget);
	exec_hash.insert(exec,item);

	if (fx) {
		fx_hash.insert(fx,item);
		if (exec->parentFx()) {
			name = exec->parentFx()->name() +": ";
		}
		name += fx->name();
	} else {
		name = "FxSequence";
	}
	item->setText(name);

	return true;
}

bool SequenceStatusWidget::removeExecuter(Executer *exec)
{
	bool removed = false;
	WRITELOCK;

	FxItem *fx = exec->originFx();

	SeqStatusListItem *item = exec_hash.value(exec);
	if (item) {
		delete item;
		removed = true;
	}

	exec_hash.remove(exec);
	fx_hash.remove(fx);

	return removed;

}

bool SequenceStatusWidget::updateExecuter(Executer *exec)
{
	WRITELOCK;
	SeqStatusListItem *item = exec_hash.value(exec);
	if (item) {
		FxItem *fx = exec->originFx();
		if (fx != item->fxItem()) {
			fx_hash.remove(item->fxItem());
			fx_hash.insert(fx,item);
			item->setFxItem(fx);
			if (fx) {
				QString name;
				if (exec->parentFx()) {
					name = exec->parentFx()->name() +": ";
				}
				name += fx->name();
				item->setText(name);
			}
		}
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
			item->setBackground(Qt::green);
			break;
		case Executer::EXEC_PAUSED:
			item->setBackground(Qt::red);
			break;
		default:
			item->setBackground(palette().base().color());
		}
		return true;
	}

	return false;
}

void SequenceStatusWidget::update_display()
{
	execHashSizeNum->setText(QString::number(exec_hash.size()));
	fxHashSizeNum->setText(QString::number(fx_hash.size()));
}

void SequenceStatusWidget::propagateExecuter(Executer *exec)
{
	READLOCK;

	if (exec_hash.contains(exec)) {
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
		case Executer::EXEC_PAUSED:
		case Executer::EXEC_FINISH:
			UNLOCK;
			updateExecuter(exec);
			break;
		default:
			UNLOCK;
			removeExecuter(exec);
		}
	} else {
		switch (exec->state()) {
		case Executer::EXEC_RUNNING:
		case Executer::EXEC_PAUSED:
		case Executer::EXEC_FINISH:
			UNLOCK;
			appendExecuter(exec);
			break;
		default:
			break;
		}
	}
	update_display();
}

void SequenceStatusWidget::insertExecuter(Executer *exec)
{
	appendExecuter(exec);
	update_display();
}

void SequenceStatusWidget::scratchExecuter(Executer *exec)
{
	removeExecuter(exec);
	update_display();
}

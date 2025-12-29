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

#include "execloop.h"
#include "log.h"
#include "toolclasses.h"
#include "execcenter.h"
#include "fxcontrol.h"
#include "executer.h"

#include <QObject>
#include <QEventLoop>
#include <QDebug>

FxExecLoop::FxExecLoop(FxControl &unit_fx)
	: QObject()
	, fxContrlRef(unit_fx)
{
	init();
}

void FxExecLoop::startLoopTimer()
{
	loop_timer.setInterval(10);
	loop_timer.start();
	loop_status = STAT_RUNNING;
}

void FxExecLoop::stopLoopTimer()
{
	loop_status = STAT_IDLE;
	loop_timer.stop();
}

void FxExecLoop::init()
{
	loop_status = STAT_IDLE;

	connect(&loop_timer,SIGNAL(timeout()),this,SLOT(processPendingEvents()));
}

void FxExecLoop::processPendingEvents()
{
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = fxContrlRef.execCenter.lockAndGetExecuterList();
	int execcnt = execlist.size();

	QMutableListIterator<Executer*> it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		switch (exec->state()) {
		case Executer::EXEC_PAUSED:
			if (exec->type() < Executer::EXEC_SCRIPT)
				exec->setTargetTimeToCurrent();
			break;
		case Executer::EXEC_RUNNING:
		case Executer::EXEC_FINISH:
			// qDebug() << "current runtime" << exec->currentRunTimeMs() << exec->currentTargetTimeMs();
			if (exec->processTimeReached()) {
				bool active = exec->processExecuter();
				if (!active) {
					exec->destroyLater();
				}
			}
			exec->processProgress();
			break;
		default:
			break;
		}

		if (execlist.size() != execcnt) // size of list changed
			break;			/// @todo reloop here??
	}

	// Don't forget to unlock the executer list
	fxContrlRef.execCenter.unlockExecuterList();
}

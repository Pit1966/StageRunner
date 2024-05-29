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

#include "execcenter.h"
#include "fxitem.h"
#include "fxlist.h"
#include "executer.h"
#include "timelineexecuter.h"
#include "log.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/audiocontrol.h"
#include "system/lightcontrol.h"
#include "system/fxcontrol.h"

#include <QTimer>

ExecCenter::ExecCenter(AppCentral &app_central)
	: QObject()
	, myApp(app_central)
{
	executerList.setName("ExecCenter::executerList");
	removeQueue.setName("ExecCenter::removeQueue");

	remove_timer.setInterval(300);
	connect(&remove_timer,SIGNAL(timeout()),this,SLOT(test_remove_queue()));
}

ExecCenter::~ExecCenter()
{
	while(executerList.size()) {
		delete executerList.lockTakeFirst();
	}
}

Executer *ExecCenter::findExecuter(const FxItem *fx)
{
	executerList.lock();
	for (Executer *exec : qAsConst(executerList)) {
		if (exec->originFx() == fx) {
			executerList.unlock();
			return exec;
		}
	}
	executerList.unlock();
	return nullptr;
}

/**
 * @brief Get a list of executers that has the given FX as origin
 * @param fx
 * @return QList<Executer*>
 *
 * @note. Be aware. The pointer in the returned may not be safe to use in threads.
 * The list is locked while searching but this is no guaranty that the Executer is still alive shortly after
 *
 * You may want to use lockAndGetExecuterList() from your calling code.
 */
QList<Executer *> ExecCenter::getExecutersFor(const FxItem *fx)
{
	QList<Executer*> list;
	executerList.lock();
	for (Executer *exec : qAsConst(executerList)) {
		if (exec->originFx() == fx) {
			list.append(exec);
		}
	}
	executerList.unlock();
	return list;
}

bool ExecCenter::exists(Executer *exec)
{
	return executerList.lockContains(exec);
}

bool ExecCenter::useExecuter(Executer *exec)
{
	bool ok = true;
	executerList.lock();
	if (!executerList.contains(exec)) {
		ok = false;
	} else {
		exec->use_cnt++;
	}
	executerList.unlock();
	return ok;
}

bool ExecCenter::freeExecuter(Executer *exec)
{
	bool ok = true;
	executerList.lock();
	if (!executerList.contains(exec)) {
		ok = false;
	} else {
		exec->use_cnt--;
		if (exec->use_cnt < 0) {
			DEBUGERROR("Use counter for executer ran below zero !!");
			exec->use_cnt = 0;
		}
	}
	executerList.unlock();

	return ok;
}

MutexQList<Executer *> &ExecCenter::lockAndGetExecuterList()
{
	executerList.lock();
	return executerList;
}

void ExecCenter::unlockExecuterList()
{
	executerList.unlock();
}

FxListExecuter *ExecCenter::newFxListExecuter(FxList *fxlist, FxItem *fxitem)
{

	FxListExecuter *exec = new FxListExecuter(myApp, fxlist);
	executerList.lockAppend(exec);
	exec->setOriginFx(fxitem);
	if (fxlist) fxlist->resetFxItemsForNewExecuter();

	connect(exec,SIGNAL(deleteMe(Executer*)),this,SLOT(deleteExecuter(Executer*)),Qt::QueuedConnection);
	connect(exec,SIGNAL(changed(Executer*)),this,SLOT(on_executer_changed(Executer*)),Qt::DirectConnection);
	connect(exec,SIGNAL(sceneExecuterStartSignal(FxSceneItem*)),myApp.unitLight,SLOT(startFxSceneExecuter(FxSceneItem*)));

	emit executerCreated(exec);
	return exec;
}

FxListExecuter *ExecCenter::getCreateFxListExecuter(FxList *fxlist)
{
	FxListExecuter *exec = findFxListExecuter(fxlist);
	if (!exec) {
		return newFxListExecuter(fxlist, 0);
	}
	return exec;
}

FxListExecuter *ExecCenter::findFxListExecuter(FxList *fxlist)
{
	executerList.lock();
	foreach (Executer * exec, executerList) {
		FxListExecuter *listexec = qobject_cast<FxListExecuter*>(exec);
		if (listexec) {
			if (listexec->fxList == fxlist) {
				executerList.unlock();
				return listexec;
			}
		}
	}
	executerList.unlock();
	return 0;
}

FxListExecuter *ExecCenter::findFxListExecuter(const FxItem *fx)
{
	return qobject_cast<FxListExecuter*>(findExecuter(fx));
}

SceneExecuter *ExecCenter::newSceneExecuter(FxSceneItem *scene, FxItem *parentFx)
{
	// qDebug() << thread()->objectName() << Q_FUNC_INFO;

	SceneExecuter *exec = new SceneExecuter(myApp,scene,parentFx);
	executerList.lockAppend(exec);

	connect(exec,SIGNAL(deleteMe(Executer*)),this,SLOT(deleteExecuter(Executer*)),Qt::QueuedConnection);
	connect(exec,SIGNAL(changed(Executer*)),this,SLOT(on_executer_changed(Executer*)),Qt::DirectConnection);

	emit executerCreated(exec);
	return exec;
}

ScriptExecuter *ExecCenter::newScriptExecuter(FxScriptItem *script, FxItem *parentFx)
{
	ScriptExecuter *exec = new ScriptExecuter(myApp,script,parentFx);

	executerList.lockAppend(exec);
	// DEVELTEXT("add script executor: %x",exec);

	connect(exec,SIGNAL(deleteMe(Executer*)),this,SLOT(deleteExecuter(Executer*)),Qt::QueuedConnection);
	connect(exec,SIGNAL(changed(Executer*)),this,SLOT(on_executer_changed(Executer*)),Qt::DirectConnection);
	connect(exec,SIGNAL(wantedDeleteFxScene(FxSceneItem*)),&myApp,SLOT(deleteFxSceneItem(FxSceneItem*)));

	emit executerCreated(exec);
	return exec;
}

ScriptExecuter *ExecCenter::findScriptExecuter(const FxItem *fx)
{
	return dynamic_cast<ScriptExecuter*>(findExecuter(fx));
}

TimeLineExecuter *ExecCenter::newTimeLineExecuter(FxTimeLineItem *timeline, FxItem *parentFx)
{
	TimeLineExecuter *exec = new TimeLineExecuter(myApp, timeline, parentFx);

	executerList.lockAppend(exec);

	connect(myApp.unitAudio, SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)), exec, SLOT(onAudioStatusChanged(AUDIO::AudioCtrlMsg)), Qt::DirectConnection);
	connect(exec, SIGNAL(deleteMe(Executer*)), this, SLOT(deleteExecuter(Executer*)), Qt::QueuedConnection);
	connect(exec, SIGNAL(changed(Executer*)), this, SLOT(on_executer_changed(Executer*)), Qt::DirectConnection);
	connect(exec, SIGNAL(wantedDeleteFxScene(FxSceneItem*)), &myApp, SLOT(deleteFxSceneItem(FxSceneItem*)));

	emit executerCreated(exec);
	return exec;
}

TimeLineExecuter *ExecCenter::findTimeLineExecuter(const FxItem *fx)
{
	return dynamic_cast<TimeLineExecuter*>(findExecuter(fx));
}

void ExecCenter::queueRemove(Executer *exec)
{
	removeQueue.lock();
	if (!removeQueue.contains(exec)) {
		removeQueue.append(exec);
		remove_timer.start();
	}
	removeQueue.unlock();
}

void ExecCenter::deleteExecuter(Executer *exec)
{
	MutexQListLocker listlock(executerList);
	if (!executerList.contains(exec)) {
		DEBUGERROR("Executer does not exist when trying to remove: %p",exec);
		return;
	}

	if (exec->use_cnt > 0) {
		DEBUGERROR("Executer is in use when trying to remove -> try later");
		queueRemove(exec);
		return;
	}

	// DEBUGTEXT("remove and destroy executor: %d: %x count: %d",exec->type(), exec, executerList.count(exec));
	if (executerList.removeOne(exec)) {
		emit executerDeleted(exec);
		delete exec;
	}
}

bool ExecCenter::executeScriptCmd(const QString &cmd)
{
	qDebug() << "execute script command: from Server?" << cmd;

	return ScriptExecuter::executeSingleCmd(cmd);
}

void ExecCenter::test_remove_queue()
{
	removeQueue.lock();
	QMutableListIterator<Executer*>it(removeQueue);
	while (it.hasNext()) {
		Executer *exec = it.next();
		executerList.lock();
		if (exec->use_cnt <= 0) {
			if (executerList.lockRemoveOne(exec)) {
				emit executerDeleted(exec);
				delete exec;
				it.remove();
				if (removeQueue.isEmpty()) {
					remove_timer.stop();
				}
			}
		}
		executerList.unlock();
	}
	removeQueue.unlock();
}

void ExecCenter::on_executer_changed(Executer *exec)
{
	emit executerChanged(exec);
}

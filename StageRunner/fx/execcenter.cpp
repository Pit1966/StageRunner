#include "execcenter.h"
#include "log.h"
#include "executer.h"
#include "appcentral.h"
#include "lightcontrol.h"
#include "fxcontrol.h"
#include "fxitem.h"
#include "fxlist.h"

#include <QTimer>

ExecCenter::ExecCenter(AppCentral &app_central) :
	QObject()
  ,myApp(app_central)
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
	foreach (Executer * exec, executerList) {
		if (exec->originFx() == fx) {
			executerList.unlock();
			return exec;
		}
	}
	executerList.unlock();
	return 0;
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

	connect(exec,SIGNAL(deleteMe(Executer*)),this,SLOT(deleteExecuter(Executer*)),Qt::QueuedConnection);
	connect(exec,SIGNAL(changed(Executer*)),this,SLOT(on_executer_changed(Executer*)),Qt::DirectConnection);
	connect(exec,SIGNAL(wantedDeleteFxScene(FxSceneItem*)),&myApp,SLOT(deleteFxSceneItem(FxSceneItem*)));

	emit executerCreated(exec);
	return exec;
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

	if (executerList.removeOne(exec)) {
		delete exec;
		emit executerDeleted(exec);
	}
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
				delete exec;
				emit executerDeleted(exec);
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

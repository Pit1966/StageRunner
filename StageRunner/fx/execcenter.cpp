#include "execcenter.h"

#include "executer.h"
#include "appcentral.h"
#include "lightcontrol.h"
#include "fxcontrol.h"

#include <QTimer>

ExecCenter::ExecCenter(AppCentral &app_central) :
	QObject()
  ,myApp(app_central)
{
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
	QMutexLocker lock(&delete_mutex);
	if (!executerList.lockContains(exec)) return false;

	exec->use_cnt++;
	return true;
}

void ExecCenter::freeExecuter(Executer *exec)
{
	QMutexLocker lock(&delete_mutex);
	if (!executerList.lockContains(exec)) return;

	exec->use_cnt--;
	if (exec->use_cnt < 0) {
		DEBUGERROR("Use counter for executer ran below zero !!");
		exec->use_cnt = 0;
	}
}

void ExecCenter::lockDelete()
{
	delete_mutex.lock();
}

void ExecCenter::unlockDelete()
{
	delete_mutex.unlock();
}

FxListExecuter *ExecCenter::newFxListExecuter(FxList *fxlist)
{

	FxListExecuter *exec = new FxListExecuter(myApp, fxlist);
	executerList.lockAppend(exec);

	connect(exec,SIGNAL(deleteMe(Executer*)),this,SLOT(deleteExecuter(Executer*)),Qt::QueuedConnection);
	connect(exec,SIGNAL(changed(Executer*)),this,SLOT(on_executer_changed(Executer*)),Qt::DirectConnection);

	emit executerCreated(exec);
	return exec;
}

FxListExecuter *ExecCenter::getCreateFxListExecuter(FxList *fxlist)
{
	FxListExecuter *exec = findFxListExecuter(fxlist);
	if (!exec) {
		return newFxListExecuter(fxlist);
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
	QMutexLocker lock(&delete_mutex);
	if (!executerList.lockContains(exec)) {
		DEBUGERROR("Executer does not exist when trying to remove");
		return;
	}
	if (exec->use_cnt > 0) {
		DEBUGERROR("Executer is in use when trying to remove -> try later");
		queueRemove(exec);
		return;
	}

	if (executerList.lockRemoveOne(exec)) {
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
		delete_mutex.lock();
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
		delete_mutex.unlock();
	}
	removeQueue.unlock();
}

void ExecCenter::on_executer_changed(Executer *exec)
{
	emit executerChanged(exec);
}

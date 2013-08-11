#include "execcenter.h"

#include "executer.h"
#include "appcentral.h"

ExecCenter::ExecCenter(AppCentral *app_central) :
	QObject()
  ,myApp(app_central)
{
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

void ExecCenter::deleteExecuter(Executer *exec)
{
	delete_mutex.lock();
	if (executerList.lockRemoveOne(exec)) {
		delete exec;
	}
	delete_mutex.unlock();
}

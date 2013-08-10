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

FxListExecuter *ExecCenter::newFxListExecuter(FxList *fxlist)
{

	FxListExecuter *exec = new FxListExecuter(myApp, fxlist);
	executerList.lockAppend(exec);

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

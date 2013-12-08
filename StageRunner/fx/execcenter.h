#ifndef EXECCENTER_H
#define EXECCENTER_H

#include <QObject>

#include "toolclasses.h"

class AppCentral;
class AudioControl;
class LightControl;
class FxItem;
class Executer;
class FxListExecuter;
class FxList;

class ExecCenter : public QObject
{
	Q_OBJECT
private:
	AppCentral *myApp;

	MutexQList<Executer*>executerList;
	QMutex delete_mutex;

public:
	ExecCenter(AppCentral *app_central);
	~ExecCenter();

	Executer * findExecuter(const FxItem *fx);
	bool exists(Executer *exec);
	void lockDelete();
	void unlockDelete();

	FxListExecuter * newFxListExecuter(FxList *fxlist);
	FxListExecuter * getCreateFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(const FxItem *fx);

signals:

public slots:
	void deleteExecuter(Executer *exec);

};

#endif // EXECCENTER_H

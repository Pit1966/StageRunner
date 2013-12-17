#ifndef EXECCENTER_H
#define EXECCENTER_H

#include <QObject>
#include <QTimer>

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
	AppCentral &myApp;

	MutexQList<Executer*>executerList;
	MutexQList<Executer*>removeQueue;
	QMutex delete_mutex;
	QTimer remove_timer;

public:
	ExecCenter(AppCentral &app_central);
	~ExecCenter();

	Executer * findExecuter(const FxItem *fx);
	bool exists(Executer *exec);
	bool useExecuter(Executer *exec);
	void freeExecuter(Executer *exec);

	void lockDelete();
	void unlockDelete();

	FxListExecuter * newFxListExecuter(FxList *fxlist);
	FxListExecuter * getCreateFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(const FxItem *fx);

	void queueRemove(Executer *exec);

signals:
	void executerCreated(Executer *);
	void executerDeleted(Executer *);
	void executerChanged(Executer *);

public slots:
	void deleteExecuter(Executer *exec);

private slots:
	void test_remove_queue();
	void on_executer_changed(Executer *exec);
};

#endif // EXECCENTER_H

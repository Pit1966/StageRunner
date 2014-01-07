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
class FxSceneItem;
class SceneExecuter;

class ExecCenter : public QObject
{
	Q_OBJECT
private:
	AppCentral &myApp;

	MutexQList<Executer*>executerList;
	MutexQList<Executer*>removeQueue;
	QTimer remove_timer;

public:
	ExecCenter(AppCentral &app_central);
	~ExecCenter();

	Executer * findExecuter(const FxItem *fx);
	bool exists(Executer *exec);
	bool useExecuter(Executer *exec);
	bool freeExecuter(Executer *exec);

	inline MutexQList<Executer*> & executerListRef() {return executerList;}
	MutexQList<Executer*> & lockAndGetExecuterList();
	void unlockExecuterList();

	FxListExecuter * newFxListExecuter(FxList *fxlist, FxItem *fxitem);
	FxListExecuter * getCreateFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(const FxItem *fx);

	SceneExecuter * newSceneExecuter(FxSceneItem *scene);

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

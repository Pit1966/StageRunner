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

public:
	ExecCenter(AppCentral *app_central);
	~ExecCenter();
	FxListExecuter * newFxListExecuter(FxList *fxlist);
	FxListExecuter * getCreateFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(FxList *fxlist);

signals:

public slots:

};

#endif // EXECCENTER_H

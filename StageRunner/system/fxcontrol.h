#ifndef FXCONTROL_H
#define FXCONTROL_H

#include "config.h"
#include "toolclasses.h"

#include <QObject>


class AppCentral;
class FxSeqItem;
class FxSceneItem;
class FxAudioItem;
class FxPlayListItem;
class FxExecLoop;
class ExecLoopThreadInterface;
class Executer;
class FxListExecuter;

class FxControl : public QObject
{
	Q_OBJECT
public:
	AppCentral &myApp;
	ExecLoopThreadInterface *execLoopInterface;

	MutexQList<Executer*>activeFxExecuters;				///< a list of Executer objects that are processed by FxExecLoop

public:
	FxControl(AppCentral &appCentral);
	~FxControl();
	bool setExecLoopEnabled(bool state);

	FxListExecuter * startFxSequence(FxSeqItem *fxseq);
	bool stopFxSequence(FxSeqItem *fxseq);
	int stopAllFxSequence();
	FxListExecuter * startFxAudioPlayList(FxPlayListItem *fxplay);

private:
	void appendLoopExecuter(Executer* exec);

signals:
	void executerChanged(Executer *exec);

public slots:

};

#endif // FXCONTROL_H

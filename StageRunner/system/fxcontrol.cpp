#include "fxcontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "execcenter.h"
#include "executer.h"
#include "fxseqitem.h"
#include "fxsceneitem.h"
#include "fxplaylistitem.h"
#include "execloop.h"
#include "execloopthreadinterface.h"
#include "toolclasses.h"

FxControl::FxControl(AppCentral &appCentral) :
	QObject()
  , myApp(appCentral)
{
	execLoopInterface = new ExecLoopThreadInterface(*this);
}

FxControl::~FxControl()
{
	delete execLoopInterface;
}

bool FxControl::setExecLoopEnabled(bool state)
{
	bool ok = true;
	if (state) {
		ok = execLoopInterface->startThread();
		if (ok) {
			// Do connects here

		}
	} else {
		ok = execLoopInterface->stopThread();
	}
	return ok;

}

FxListExecuter * FxControl::startFxSequence(FxSeqItem *fxseq)
{
	if (!FxItem::exists(fxseq)) {
		qDebug("FxSeqItem not found in pool");
		return 0;
	}

	// Create an executor for the sequence
	FxListExecuter *fxexec = myApp.execCenter->newFxListExecuter(fxseq->seqList);
	fxexec->setOriginFx(fxseq);
	// Give control for executer to FxControl loop
	appendLoopExecuter(fxexec);

	return fxexec;
}

bool FxControl::stopFxSequence(FxSeqItem *fxseq)
{

}


/**
 * @brief Stop executing of all running FxSeqItems
 * @return Number of stopped sequences
 *
 *  This does not affect the fade state of the scenes and running audio.
 */
int FxControl::stopAllFxSequence()
{
	int count = 0;
	activeFxExecuters.lock();
	QMutableListIterator<Executer*>it(activeFxExecuters);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (!exec->state() != Executer::EXEC_DELETED) {
			count++;
			exec->destroyLater();
		}
	}
	activeFxExecuters.unlock();
	return count;
}

FxListExecuter *FxControl::startFxAudioPlayList(FxPlayListItem *fxplay)
{
	if (!FxItem::exists(fxplay)) {
		qDebug("FxAudioPlayListItem not found in pool");
		return 0;
	}

	// Create an executor for the sequence
	FxListExecuter *fxexec = myApp.execCenter->newFxListExecuter(fxplay->fxPlayList);
	fxexec->setOriginFx(fxplay);
	// Give control for executer to FxControl loop
	appendLoopExecuter(fxexec);

	return fxexec;
}

void FxControl::appendLoopExecuter(Executer *exec)
{
	exec->setExecLoopFlag(true);
	activeFxExecuters.lockAppend(exec);
}

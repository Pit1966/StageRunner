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
#include "fxlistwidget.h"
#include "audiocontrol.h"

FxControl::FxControl(AppCentral &appCentral, ExecCenter &exec_center) :
	QObject()
  , myApp(appCentral)
  , execCenter(exec_center)
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
	FxListExecuter *fxexec = myApp.execCenter->newFxListExecuter(fxseq->seqList,fxseq);


	// Give control for executer to FxControl loop
	fxexec->activateProcessing();

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
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (!exec->state() != Executer::EXEC_DELETED) {
			count++;
			exec->destroyLater();
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;
}

FxListExecuter *FxControl::startFxAudioPlayList(FxPlayListItem *fxplay)
{
	if (execCenter.findExecuter(fxplay))
		return 0;

	return continueFxAudioPlayList(fxplay,0);
}

/**
 * @brief Starts or continues the playback of a FxAudioPlayList
 * @param fxplay A Pointer to the FxPlayListItem instance that contains the playlist
 * @param fxaudio A valid Pointer to the FxAudioItem that should be started
 * @return A pointer to a valid Executer or NULL
 *
 */
FxListExecuter *FxControl::continueFxAudioPlayList(FxPlayListItem *fxplay, FxAudioItem *fxaudio)
{
	// Check if FxItem is valid
	if (!FxItem::exists(fxplay)) {
		qDebug("FxAudioPlayListItem not found in pool");
		return 0;
	}

	// First try to find an existing Executer
	FxListExecuter *exe = myApp.execCenter->findFxListExecuter(fxplay);
	if (!exe) {
		exe = myApp.execCenter->newFxListExecuter(fxplay->fxPlayList, fxplay);
		exe->setCurrentFx(fxaudio);
	} else {
		myApp.unitAudio->fadeoutFxAudio(exe,1000);
		// exe->setCurrentFx(0);
		exe->setNextFx(fxaudio);
	}

	// Maybe there is a FxListWidget window opened. Than we can do some monitoring
	FxListWidget *wid = FxListWidget::findFxListWidget(fxplay->fxPlayList);
	if (wid) {
		connect(exe,SIGNAL(currentFxChanged(FxItem*)),wid,SLOT(markFx(FxItem*)));
		connect(exe,SIGNAL(nextFxChanged(FxItem*)),wid,SLOT(selectFx(FxItem*)));
	}

	exe->activateProcessing();

	return exe;
}


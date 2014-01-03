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
#include "fxlistwidgetitem.h"
#include "fxlist.h"

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
	bool found = false;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() == fxseq && exec->type() == Executer::EXEC_FXLIST) {
			if (exec->state() != Executer::EXEC_DELETED) {
				found = true;
				// Stop Sequence Current FxItem
				FxItem *fx = reinterpret_cast<FxListExecuter*>(exec)->currentFx();
				if (FxItem::exists(fx)) {
					myApp.executeFxCmd(fx,CMD_FX_STOP,exec);
				}
				// Stop Sequence Executer
				exec->setPaused(true);
				exec->destroyLater();
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return found;
}


/**
 * @brief Stop executing of all running FxSeqItems
 * @return Number of stopped sequences
 *
 *  This does not affect the fade state of the scenes and running audio.
 */
int FxControl::stopAllFxSequences()
{
	int count = 0;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() && exec->originFx()->fxType() == FX_SEQUENCE) {
			if (exec->state() != Executer::EXEC_DELETED) {
				count++;
				exec->setPaused(true);
				exec->destroyLater();
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;
}

bool FxControl::pauseFxPlaylist(FxPlayListItem *fxplay)
{
	if (!FxItem::exists(fxplay)) return false;

	bool found = false;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext() && !found) {
		Executer *exec = it.next();
		if (exec->originFx() == fxplay) {
			exec->setPaused(true);

			found = true;
			// Stop current Sound
			if (exec->type() == Executer::EXEC_FXLIST) {
				FxAudioItem *fxa = reinterpret_cast<FxListExecuter*>(exec)->currentFxAudio();
				myApp.unitAudio->stopFxAudio(fxa);
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return found;
}

bool FxControl::stopFxPlayList(FxPlayListItem *fxplay)
{
	if (!FxItem::exists(fxplay)) return false;

	bool found = false;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext() && !found) {
		Executer *exec = it.next();
		if (exec->originFx() == fxplay) {
			found = true;
			// Stop FxPlaylist (This stops and deletes the executer)
			if (exec->state() != Executer::EXEC_DELETED) {
				exec->setPaused(true);
				exec->destroyLater();
			}
			// Stop current Sound
			if (exec->type() == Executer::EXEC_FXLIST) {
				FxAudioItem *fxa = reinterpret_cast<FxListExecuter*>(exec)->currentFxAudio();
				myApp.unitAudio->stopFxAudio(fxa);
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return found;
}

int FxControl::pauseAllFxPlaylist()
{
	int count = 0;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() && exec->originFx()->fxType() == FX_AUDIO_PLAYLIST) {
			exec->setPaused(true);
			count++;
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;
}

int FxControl::stopAllFxPlaylists()
{
	int count = 0;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() && exec->originFx()->fxType() == FX_AUDIO_PLAYLIST) {
			exec->setPaused(true);
			if (exec->state() != Executer::EXEC_DELETED) {
				exec->destroyLater();
				count++;
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;

}

FxListExecuter *FxControl::startFxAudioPlayList(FxPlayListItem *fxplay)
{
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
		if (exe->isRunning()) {
			myApp.unitAudio->fadeoutFxAudio(exe,1000);
			exe->setNextFx(fxaudio);
		} else {
			if (!fxaudio) {
				if (!exe->nextFx()) {
					// continue play at last position
					exe->setNextFx(exe->currentFx());
				}
			} else {
				exe->setNextFx(fxaudio);
			}
			exe->setPaused(false);
		}
	}
	// Determine what the FxListWidget Window is where the FxPlayListItem lives in
	FxList *fxlist = fxplay->parentFxList();
	FxListWidget *wid = FxListWidget::findFxListWidget(fxlist);
	if (wid) {
		FxListWidgetItem *listitem = wid->getFxListWidgetItemFor(fxplay);
		connect(exe,SIGNAL(playListProgressChanged(int,int)),listitem,SLOT(setActivationProgress(int,int)),Qt::UniqueConnection);
	}

	// Maybe there is a FxListWidget window opened. Than we can do some monitoring
	wid = FxListWidget::findFxListWidget(fxplay->fxPlayList);
	if (wid) {
		connect(exe,SIGNAL(currentFxChanged(FxItem*)),wid,SLOT(markFx(FxItem*)),Qt::UniqueConnection);
		connect(exe,SIGNAL(nextFxChanged(FxItem*)),wid,SLOT(selectFx(FxItem*)),Qt::UniqueConnection);
		connect(myApp.unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),wid,SLOT(propagateAudioStatus(AudioCtrlMsg)),Qt::UniqueConnection);
		connect(wid,SIGNAL(fxItemSelected(FxItem*)),exe,SLOT(selectNextFx(FxItem*)),Qt::UniqueConnection);
	}

	exe->activateProcessing();

	return exe;
}


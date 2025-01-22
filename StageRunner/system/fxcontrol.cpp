//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "fxcontrol.h"
#include "config.h"
#include "log.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/audiocontrol.h"
#include "appcontrol/execloop.h"
#include "appcontrol/execloopthreadinterface.h"
#include "fx/execcenter.h"
#include "fx/executer.h"
#include "fx/fxlist.h"
#include "fx/fxplaylistitem.h"
#include "fx/fxsceneitem.h"
#include "fx/fxscriptitem.h"
#include "fx/fxseqitem.h"
#include "fx/fxtimelineitem.h"
#include "fx/timelineexecuter.h"
#include "gui/fxlistwidget.h"
#include "gui/fxlistwidgetitem.h"
#include "gui/fxscriptwidget.h"
#include "tool/toolclasses.h"
#include "widgets/fxtimelineeditwidget.h"


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

	// See what has to be done before starting the sequence
	if (fxseq->stopOtherSeqOnStart) {
		myApp.sequenceStop();
	}
	if (fxseq->blackOtherSeqOnStart) {
		myApp.lightBlack(200);
	}


	// Create an executor for the sequence
	FxListExecuter *fxexec = myApp.execCenter->newFxListExecuter(fxseq->seqList,fxseq);

	// Determine what the FxListWidget is where the Sequence comes from
	FxListWidget *parentwid = FxListWidget::findParentFxListWidget(fxseq);
	if (parentwid) {
		FxListWidgetItem *listitem = parentwid->getFxListWidgetItemFor(fxseq);
		connect(fxexec,SIGNAL(listProgressStepChanged(int,int)),listitem,SLOT(setActivationProgress(int,int)),Qt::UniqueConnection);
	}

	// Determine the FxListWidget that shows the current sequence (if opened)
	FxListWidget *seqwid = FxListWidget::findFxListWidget(fxseq->seqList);
	if (seqwid) {
		connect(fxexec,SIGNAL(currentFxChanged(FxItem*)),seqwid,SLOT(markFx(FxItem*)),Qt::UniqueConnection);
		connect(fxexec,SIGNAL(nextFxChanged(FxItem*)),seqwid,SLOT(selectFx(FxItem*)),Qt::UniqueConnection);
	}


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
				/// @todo: Search for child executers. Both. SceneExecuter and FxListExecuter
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

ScriptExecuter *FxControl::startFxScript(FxScriptItem *fxscript)
{
	if (!FxItem::exists(fxscript)) {
		qDebug("FxScriptItem not found in pool");
		return nullptr;
	}

	ScriptExecuter *fxexec = myApp.execCenter->findScriptExecuter(fxscript);
	if (fxexec && fxexec->isMultiStartDisabled()) {
		LOGTEXT(tr("<font color=green>Start script</font> %1 -> <font color=darkOrange>canceled multiple start</font>")
				.arg(fxscript->name()));
		return nullptr;
	}

	return _startFxScript(fxscript);
}

void FxControl::handleDmxInputScriptEvent(FxScriptItem *fxscript, uchar dmxVal)
{
	if (dmxVal > 30) {
		ScriptExecuter *fxexec = myApp.execCenter->findScriptExecuter(fxscript);
		if (fxexec) {
			// no multistart when started via DMX
			if (fxexec->isStartedByDmx())
				return;

			if (fxexec->isMultiStartDisabled()) {
				LOGTEXT(tr("<font color=green>Hooked DMX: Start script</font> %1 -> <font color=darkOrange>canceled multiple start</font>")
						.arg(fxscript->name()));
				return;
			}

			// ok we start another script
		}

		fxexec = _startFxScript(fxscript);
		if (fxexec) {
			fxexec->setDmxStarted(true);
		}
	}
	else if (dmxVal < 20) {
		ScriptExecuter *fxexec = myApp.execCenter->findScriptExecuter(fxscript);
		if (fxexec) {
			// no multistart when started via DMX
			if (fxexec->isStartedByDmx()) {
				stopFxScript(fxscript);
			}
		}

	}
}

bool FxControl::stopFxScript(FxScriptItem *fxscript)
{
	bool found = false;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() == fxscript && exec->type() == Executer::EXEC_SCRIPT) {
			if (exec->state() != Executer::EXEC_DELETED) {
				found = true;
				/// @todo: Search for child executers. Both. SceneExecuter and FxListExecuter
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


int FxControl::stopAllFxScripts(FxItem *fx)
{
	int count = 0;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->type() == Executer::EXEC_SCRIPT) {
			if (exec->state() != Executer::EXEC_DELETED) {
				if (fx && exec->originFx() != fx)
					continue;
				count++;
				exec->setFinish();
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;
}

TimeLineExecuter *FxControl::startFxTimeLine(FxTimeLineItem *fxtimeline, int atMs)
{
	if (!FxItem::exists(fxtimeline)) {
		qWarning("FxTimeLineItem not found in pool");
		return nullptr;
	}

	TimeLineExecuter *exec = myApp.execCenter->findTimeLineExecuter(fxtimeline);
	if (exec && exec->isPaused()) {
		exec->setPaused(false);
		return exec;
	}
	else if (exec && exec->isMultiStartDisabled()) {
		LOGTEXT(tr("<font color=green>Start timeline</font> %1 -> <font color=darkOrange>canceled multiple start</font>")
				.arg(fxtimeline->name()));
		return nullptr;
	} else {
		LOGTEXT(tr("<font color=green>Start timeline</font> %1").arg(fxtimeline->name()));
	}

	// Create an executor for the script
	exec = myApp.execCenter->newTimeLineExecuter(fxtimeline, fxtimeline->parentFxItem());
	bool isContinue = false;
	if (atMs > 0) {
		exec->setReplayPosition(atMs);
		isContinue = true;
	}

	// Determine what the FxListWidget is where the FxTimeLineItem comes from
	FxListWidget *parentwid = FxListWidget::findParentFxListWidget(fxtimeline);
	if (parentwid) {
		FxListWidgetItem *listitem = parentwid->getFxListWidgetItemFor(fxtimeline);
		connect(exec, SIGNAL(listProgressChanged(int,int)), listitem, SLOT(setActivationProgress(int,int)));
		connect(exec, SIGNAL(executerStatusChanged(FxItem*,QString)), listitem, SLOT(setFxStatus(FxItem*,QString)));
	}

	// Determine wheter there is a FxTimeLineEditWidget that is currently the editor for this timeline
	FxTimeLineEditWidget *editwid = FxTimeLineEditWidget::findParentFxTimeLinePanel(fxtimeline);
	if (editwid) {
		connect(exec, SIGNAL(timeLineStatusChanged(int)), editwid, SLOT(onChildRunStatusChanged(int)));
		connect(exec, SIGNAL(playPosChanged(int)), editwid->timeLineWidget(), SLOT(setCursorPos(int)));
	}

	// Give control for executer to FxControl loop
	if (!exec->activateProcessing(isContinue))
		LOGERROR(tr("Could not activate timeline: %1").arg(fxtimeline->name()));

	return exec;
}

bool FxControl::stopFxTimeLine(FxTimeLineItem *fxtimeline)
{
	bool found = false;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->originFx() == fxtimeline && exec->type() == Executer::EXEC_TIMELINE) {
			if (exec->state() != Executer::EXEC_DELETED) {
				found = true;
				/// @todo: Search for child executers. Both. SceneExecuter and FxListExecuter
				// Stop TimeLine Executer
				exec->setPaused(true);
				exec->destroyLater();
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return found;
}

int FxControl::stopAllTimeLines()
{
	int count = 0;
	// This fetches a reference to the executer list and locks it!!
	MutexQList<Executer*> &execlist = execCenter.lockAndGetExecuterList();

	QMutableListIterator<Executer*>it(execlist);
	while (it.hasNext()) {
		Executer *exec = it.next();
		if (exec->type() == Executer::EXEC_TIMELINE) {
			if (exec->state() != Executer::EXEC_DELETED) {
				count++;
				exec->setFinish();
				// exec->destroyLater();
			}
		}
	}

	// Don't forget to unlock the executer list
	execCenter.unlockExecuterList();
	return count;
}

ScriptExecuter *FxControl::_startFxScript(FxScriptItem *fxscript)
{
	LOGTEXT(tr("<font color=green>Start script</font> %1").arg(fxscript->name()));

	// Create an executor for the script
	ScriptExecuter *fxexec = myApp.execCenter->newScriptExecuter(fxscript, fxscript->parentFxItem());

	// Determine what the FxListWidget is where the script comes from
	FxListWidget *parentwid = FxListWidget::findParentFxListWidget(fxscript);
	if (parentwid) {
		/// @todo script
		FxListWidgetItem *listitem = parentwid->getFxListWidgetItemFor(fxscript);
		connect(fxexec,SIGNAL(listProgressStepChanged(int,int)),listitem,SLOT(setActivationProgress(int,int)),Qt::UniqueConnection);
	}

	// Determine the FxScriptWidget that is the editing widget of the current script
	FxScriptWidget *scrwid = FxScriptWidget::findParentFxScriptWidget(fxscript);
	if (scrwid) {
		/// @todo script
//		connect(fxexec,SIGNAL(currentFxChanged(FxItem*)),seqwid,SLOT(markFx(FxItem*)),Qt::UniqueConnection);
//		connect(fxexec,SIGNAL(nextFxChanged(FxItem*)),seqwid,SLOT(selectFx(FxItem*)),Qt::UniqueConnection);
	}


	// Give control for executer to FxControl loop
	fxexec->activateProcessing();

	return fxexec;
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
		connect(exe,SIGNAL(listProgressStepChanged(int,int)),listitem,SLOT(setActivationProgress(int,int)),Qt::UniqueConnection);
		connect(exe,SIGNAL(executerStatusChanged(FxItem*,QString)),listitem,SLOT(setFxStatus(FxItem*,QString)));
	}

	// Maybe there is a FxListWidget window opened. Than we can do some monitoring
	wid = FxListWidget::findFxListWidget(fxplay->fxPlayList);
	if (wid) {
		connect(exe,SIGNAL(currentFxChanged(FxItem*)),wid,SLOT(markFx(FxItem*)),Qt::UniqueConnection);
		connect(exe,SIGNAL(nextFxChanged(FxItem*)),wid,SLOT(selectFx(FxItem*)),Qt::UniqueConnection);
		connect(myApp.unitAudio,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),wid,SLOT(propagateAudioStatus(AUDIO::AudioCtrlMsg)),Qt::UniqueConnection);
		connect(wid,SIGNAL(fxItemSelected(FxItem*)),exe,SLOT(selectNextFx(FxItem*)),Qt::UniqueConnection);
	}

	exe->activateProcessing();

	return exe;
}


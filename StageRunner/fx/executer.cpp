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

#include "executer.h"
#include "execcenter.h"
#include "log.h"
#include "fxlist.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "videocontrol.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxscriptitem.h"
#include "fxclipitem.h"
#include "usersettings.h"
#include "qtstatictools.h"
#include "dmxchannel.h"
#include "system/qt_versions.h"

#include <QStringList>

using namespace LIGHT;
using namespace SCRIPT;

Executer::Executer(AppCentral &app_central, FxItem *originFx)
	: myApp(app_central)
	, originFxItem(originFx)
	, parentFxItem(0)
	, eventTargetTimeMs(0)
	, lastProgressTimeMs(0)
	, isWaitingForAudio(false)
	, myState(EXEC_IDLE)
	, use_cnt(0)
{
	runTime.start();
}

Executer::~Executer()
{
	// qDebug() << "Executer destroyed" << getIdString();
}

void Executer::setIdString(const QString &str)
{
	idString = str;
}

/**
 * @brief Base implementation of executer processing
 * @return true, if processing was done succesful
 *
 * This function should be reimplemented in derivated classes.
 */
bool Executer::processExecuter()
{
	return false;
}

/**
 * @brief You may want to reimplement this function to perform progress calculation of the executer
 */
void Executer::processProgress()
{
}

QString Executer::getIdString() const
{
	if (idString.isEmpty()) {
		return QString("0x%1").arg(quint64(this));
	} else {
		return idString;
	}
}

/**
 * @brief Executer::destroyExecuterLater
 */
void Executer::destroyLater()
{
	if (myState != EXEC_DELETED) {
		myState = EXEC_DELETED;
		emit deleteMe(this);
	}
}

bool Executer::activateProcessing()
{
	if (myState == EXEC_IDLE) {
		myState = EXEC_RUNNING;
		return true;
	}
	else if (myState == EXEC_RUNNING || myState == EXEC_PAUSED) {
		return true;
	}

	return false;
}

bool Executer::deactivateProcessing()
{
	switch (myState) {
	case EXEC_RUNNING:
	case EXEC_PAUSED:
	case EXEC_FINISH:
		myState = EXEC_IDLE;
		return true;
	case EXEC_DELETED:
		return false;
	case EXEC_IDLE:
		return false;
	}
	return false;
}

bool Executer::setPaused(bool state)
{
	switch (myState) {
	case EXEC_RUNNING:
		if (state) {
			myState = EXEC_PAUSED;
			emit changed(this);
			return true;
		}
		break;
	case EXEC_PAUSED:
		if (!state) {
			myState = EXEC_RUNNING;
			emit changed(this);
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

bool Executer::setFinish()
{
	if (myState != EXEC_FINISH) {
		myState = EXEC_FINISH;
		emit changed(this);
		return true;
	}

	return false;
}



FxListExecuter::FxListExecuter(AppCentral &app_central, FxList *fx_list)
	: Executer(app_central)
	, fxList(fx_list)
	, curFx(0)
	, nxtFx(0)
	, m_playbackProgress(0)
	, m_currentInitialVolume(-1)
	, m_loopCount(0)
{
	init();
}

FxListExecuter::~FxListExecuter()
{
	emit listProgressStepChanged(0,0);
}

void FxListExecuter::setCurrentFx(FxItem *fx)
{
	if (fx != curFx) {
		if (fx) {
			fx->initForSequence();
		}
		curFx = fx;
		emit currentFxChanged(fx);
		setNextFx(0);
	}
}

void FxListExecuter::setNextFx(FxItem *fx)
{
	if (fx != nxtFx) {
		nxtFx = fx;
		emit nextFxChanged(fx);
	}
}

FxAudioItem * FxListExecuter::currentFxAudio()
{
	if (curFx && curFx->fxType() == FX_AUDIO) {
		return reinterpret_cast<FxAudioItem*>(curFx);
	} else {
		return 0;
	}
}

/**
 * @brief process next event in the FxList
 * @return true, if something was processed
 */
bool FxListExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) return false;
	if (eventTargetTimeMs > runTime.elapsed()) return false;

	FxItem *fx = 0;
	qint64 cue_time = 0;

	// First we have to check if there is already an active FX
	if (!curFx) {
		fx = move_to_next_fx();
	} else {
		fx = curFx;
	}

	// Execute cues until cue time is not NULL or we have reached fxList end
	while (fx && cue_time == 0 && myState == EXEC_RUNNING) {
		// Execute next CUE command in FxScene and give me the estimated execution time in ms
		cue_time = cue_fx(fx);
		// if time < 0: fx is completely processed. Move to next
		if (cue_time < 0) {
			fx = move_to_next_fx();
			if (fx) {
				cue_time = 0;
			} else {
				if (originFxItem) {
					if (originFxItem->loopValue() > 0) {
						fx = move_to_next_fx();
						cue_time = 0;
					}
				}
			}
		}
	}

	eventTargetTimeMs += cue_time;

	return (fx != 0);
}

void FxListExecuter::setFxList(FxList *fx_list)
{
	fxList = fx_list;
}

void FxListExecuter::audioCtrlReceiver(AudioCtrlMsg msg)
{
	if (msg.executer != this)
		return;		// Not for me

//	qDebug("%s: Received audio msg: AudioStatus %d, AudioCmd %d"
//		   ,Q_FUNC_INFO,msg.currentAudioStatus,msg.ctrlCmd);
	if (msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED) {
		if (isWaitingForAudio) {
			if (msg.currentAudioStatus == AUDIO_RUNNING) {
				qDebug("   Wait for audio end");
			}
			else if (msg.currentAudioStatus == AUDIO_IDLE) {
				qDebug("   Audio now idle");
				eventTargetTimeMs = runTime.elapsed();
				isWaitingForAudio = false;
			}
			else if (msg.currentAudioStatus == AUDIO_ERROR) {
				DEBUGERROR("An error occured in sequence executer while waiting for audio '%s'"
						   ,msg.fxAudio->name().toLocal8Bit().data());
				if (curFx && curFx->fxType() == FX_AUDIO) {
					FxAudioItem *fxa = reinterpret_cast<FxAudioItem*>(curFx);
					fxa->setSeqStatus(AUDIO_OFF);
					fxa->resetFx();
					setNextFx(0);
				}
				move_to_next_fx();
				isWaitingForAudio = false;
				eventTargetTimeMs = runTime.elapsed();
			}
			else if (msg.currentAudioStatus == AUDIO_NO_FREE_SLOT) {
				DEBUGERROR("No free audio slot found in sequence executer '%s' for audio '%s'"
						   ,originFx()->name().toLocal8Bit().data()
						   ,msg.fxAudio->name().toLocal8Bit().data());
				setNextFx(0);
				destroyLater();
			}
		}
	}
	else if (msg.ctrlCmd == CMD_STATUS_REPORT) {
		if (originFxItem && originFxItem->fxType() == FX_AUDIO_PLAYLIST && fxList->size()) {
			emit listProgressStepChanged(m_playbackProgress, msg.progress);
		}
	}
}

void FxListExecuter::lightCtrlReceiver(FxSceneItem *fxs)
{
	if (debug > 1) qDebug("FxListExecuter(%p): Scene '%s' status changed to '%s'"
		   , this, fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::sceneCueReceiver(FxSceneItem *fxs)
{
	if (debug > 1) qDebug("FxListExecuter(%p): Scene '%s' cue ended. Status: '%s'"
		   , this, fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::init()
{
	connect(myApp.unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	// connect(myApp.unitAudio,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(myApp.unitLight,SIGNAL(sceneChanged(FxSceneItem*)),this,SLOT(lightCtrlReceiver(FxSceneItem*)),Qt::DirectConnection);
	connect(myApp.unitLight,SIGNAL(sceneCueReady(FxSceneItem*)),this,SLOT(sceneCueReceiver(FxSceneItem*)),Qt::DirectConnection);
}

/**
 * @brief Gets the next valid FX from fxList and makes it the current FX
 * @return Pointer to current FX object or NULL if fxList was fully processed
 */
FxItem *FxListExecuter::move_to_next_fx()
{
	// Find the following FX in fxList of the current active or set the first FX in fxList as current
	FxItem * next = 0;
	if (nxtFx && FxItem::exists(nxtFx)) {
		next = nxtFx;
		fxList->resetFxItems(nxtFx);
		setNextFx(0);
	}
	else if (fxList->isRandomized()) {
		next = fxList->findSequenceRandomFxItem();
	}
	else if (!curFx) {
		next = fxList->getFirstFx();
		if (next) next->resetFx();
	}
	else {
		next = fxList->findSequenceFollower(curFx);
		if (next) next->resetFx();
	}

	if (originFxItem && fxList->size()) {
		if (originFxItem->fxType() == FX_AUDIO_PLAYLIST || originFxItem->fxType() == FX_SEQUENCE) {
			int pos = 0;
			while (pos < fxList->size() && fxList->at(pos) != next) {
				pos++;
			}
			m_playbackProgress = pos * 1000 / fxList->size();
			emit listProgressStepChanged(m_playbackProgress,0);
		}
	}

	// Check for fxList end
	if (!next) {
		// Check if playback of list is looped
		m_loopCount++;
		if (m_loopCount < fxList->loopTimes()) {
			fxList->resetFxItemsForNewExecuter();
			fxList->resetFxItems();
			if (fxList->isRandomized()) {
				next = fxList->findSequenceRandomFxItem();
			} else {
				next = fxList->findSequenceFirstItem();
			}
		}
	}

	setCurrentFx(next);

//	QString nextName = next!=0?next->name():QString("NULL");
//	qDebug() << "move to next fx" << nextName;

	return next;
}

/**
 * @brief Execute next CUE command in Fx sequence
 * @param fx Pointer to the FxItem that will be executed
 * @return cue time in ms (time the execution of the command lasts) or -1, if Fx is not valid or Fx processing has finished
 */
qint64 FxListExecuter::cue_fx(FxItem *fx)
{
	if (!FxItem::exists(fx)) return -1;

	qint64 cue_time = 0;

	switch (fx->fxType()) {
	case FX_SCENE:
		cue_time = cue_fx_scene(reinterpret_cast<FxSceneItem*>(fx));
		break;
	case FX_AUDIO:
		cue_time = cue_fx_audio(reinterpret_cast<FxAudioItem*>(fx));
		break;
	default:
		DEBUGERROR("Executing '%s' is not supported by Sequences and Playlists"
				   ,fx->name().toLocal8Bit().data());
		return -1;
	}

	emit changed(this);
	return cue_time;
}

/**
 * @brief Execute next CUE command in Fx sequence
 * @param scene Pointer to the FxSceneItem that will be executed
 * @return cue time in ms (time the execution of the command lasts) or -1, if Fx is at the end of the
 */
qint64 FxListExecuter::cue_fx_scene(FxSceneItem *scene)
{
	qint64 cue_time = 0;

	switch (scene->seqStatus()) {
	case SCENE_OFF:
		cue_time = scene->preDelay();
		if (cue_time == -1) {
			cue_time = 0;
			scene->setSeqStatus(SCENE_FADE_OUT);
			emit sceneExecuterStartSignal(scene);
		}
		else if (cue_time < 0) {
			cue_time = 0;
			scene->setSeqStatus(SCENE_PRE_DELAY);
		}
		else {
			scene->setSeqStatus(SCENE_PRE_DELAY);
		}
		break;
	case SCENE_PRE_DELAY:
		cue_time = scene->fadeInTime();
		if ( ! scene->initSceneCommand(MIX_INTERN,CMD_SCENE_FADEIN,cue_time) ) {
			LOGERROR(tr("FxScene: %1 is already active").arg(scene->name()));
		}
		myApp.unitLight->setSceneActive(scene);
		scene->setSeqStatus(SCENE_FADE_IN);
		break;
	case SCENE_FADE_IN:
		cue_time = scene->holdTime();
		scene->setSeqStatus(SCENE_HOLD);
		break;
	case SCENE_HOLD:
		cue_time = scene->fadeOutTime();
		scene->initSceneCommand(MIX_INTERN,CMD_SCENE_FADEOUT,cue_time);
		scene->setSeqStatus(SCENE_FADE_OUT);
		break;
	case SCENE_FADE_OUT:
		cue_time = scene->postDelay();
		scene->setSeqStatus(SCENE_POST_DELAY);
		break;
	case SCENE_POST_DELAY:
	default:
		scene->setSeqStatus(SCENE_OFF);
		cue_time = -1;
	}

	return cue_time;
}

qint64 FxListExecuter::cue_fx_audio(FxAudioItem *audio)
{
	qint64 cue_time = 0;
	switch (audio->seqStatus()) {
	case AUDIO_OFF:
		cue_time = audio->preDelay();
		if (cue_time == -1) {
			myApp.unitAudio->startFxAudioAt(audio,this);
			audio->setSeqStatus(AUDIO_POST_DELAY);
			cue_time = audio->postDelay();
			if (cue_time < 0)
				cue_time = 0;
			return cue_time;
		}
		else if (cue_time < 0) {
			cue_time = 0;
		}
		audio->setSeqStatus(AUDIO_PRE_DELAY);
		break;
	case AUDIO_PRE_DELAY:
		if (audio->holdTime()) {
			cue_time = audio->holdTime();
		} else {
			isWaitingForAudio = true;
			cue_time = 1000000;
		}
		if (originFx()->fxType() == FX_AUDIO_PLAYLIST) {
			myApp.unitAudio->startFxAudioInSlot(audio,myApp.userSettings->pAudioPlayListChannel,this);
		} else {
			myApp.unitAudio->startFxAudioAt(audio,this);
		}
		audio->setSeqStatus(AUDIO_PLAYTIME);
		break;
	case AUDIO_PLAYTIME:
		cue_time = audio->fadeOutTime();
		myApp.unitAudio->fadeoutFxAudio(audio,cue_time);
		audio->setSeqStatus(AUDIO_FADEOUT);
		break;
	case AUDIO_FADEOUT:
		cue_time = audio->postDelay();
		audio->setSeqStatus(AUDIO_POST_DELAY);
		break;
	case AUDIO_POST_DELAY:
	default:
		audio->setSeqStatus(AUDIO_OFF);
		cue_time = -1;
	}

	return cue_time;
}

void FxListExecuter::selectNextFx(FxItem *fx)
{
	qDebug("set Next Fx %s",fx?fx->name().toLocal8Bit().data():"NULL");
	setNextFx(fx);
}

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

bool SceneExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) return false;

	bool active = true;
	qint64 cue_time = 0;

	while (active && cue_time == 0) {
		switch (sceneState) {
		case SCENE_OFF:
			cue_time = curScene->preDelay();
			sceneState = SCENE_PRE_DELAY;
			if (cue_time < 0)
				cue_time = 0;
			break;
		case SCENE_PRE_DELAY:
			cue_time = curScene->fadeInTime();
			if ( ! curScene->initSceneCommand(MIX_INTERN,CMD_SCENE_FADEIN,cue_time) ) {
				LOGERROR(tr("FxScene: %1 is already active").arg(curScene->name()));
			}
			myApp.unitLight->setSceneActive(curScene);
			sceneState = SCENE_FADE_IN;
			break;
		case SCENE_FADE_IN:
			cue_time = curScene->holdTime();
			sceneState = SCENE_HOLD;
			break;
		case SCENE_HOLD:
			cue_time = curScene->fadeOutTime();
			curScene->initSceneCommand(MIX_INTERN,CMD_SCENE_FADEOUT,cue_time);
			// We skip the postDelay parameter since it makes no sense in a SceneExecuter
			sceneState = SCENE_POST_DELAY;
			break;
		case SCENE_FADE_OUT:
			cue_time = curScene->postDelay();
			sceneState = SCENE_POST_DELAY;
			break;
		case SCENE_POST_DELAY:
		default:
			cue_time = 0;
			eventTargetTimeMs = runTime.elapsed();
			sceneState = SCENE_OFF;
			active = false;
		}

		eventTargetTimeMs += cue_time;
	}

	return active;
}


SceneExecuter::SceneExecuter(AppCentral &app_central, FxSceneItem *scene, FxItem *parentFx)
	: Executer(app_central,scene)
	, curScene(scene)
	, sceneState(LIGHT::SCENE_OFF)
{
	parentFxItem = parentFx;
}

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

bool ScriptExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) return false;

	bool proceed = true;
	bool active = true;

	while (proceed) {
		FxScriptLine *line = m_script.at(m_currentLineNum);
		if (line) {
			bool reExecLineDelayed = false;
			if (line->command().size() > 2 && !line->command().startsWith("#"))
				proceed = executeLine(line, reExecLineDelayed);

			if (!reExecLineDelayed)
				m_currentLineNum++;
		} else {
			proceed = false;
			active = false;
			LOGTEXT(tr("<font color=darkGreen>Script</font> '%1' <font color=darkGreen>finished</font>")
					.arg(m_fxScriptItem->name()));
		}
	}

	return active;
}

void ScriptExecuter::processProgress()
{
	if (runTime.elapsed() > lastProgressTimeMs) {
		lastProgressTimeMs += 50;
		int perMille = 0;
		if (m_script.execDuration() > 0) {
			qint64 timeMs = runTime.elapsed() % m_script.execDuration();
			perMille = timeMs * 1000 / m_script.execDuration();
		}
		emit listProgressStepChanged(perMille, 0);
	}

	if (state() == EXEC_FINISH && m_breakOnCancel) {
		if (m_currentLineNum < m_script.size()) {
			// fadeout all scenes that where cloned by this executer
			for (FxSceneItem *fxs : m_clonedSceneList) {
				fxs->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT, 200);
			}
			LOGTEXT(tr("<font color=darkGreen>Script</font> '%1' <font color=darkOrange>Canceled</font>")
					.arg(m_fxScriptItem->name()));
			destroyLater();
		}
	}
}

ScriptExecuter::ScriptExecuter(AppCentral &app_central, FxScriptItem *script, FxItem *parentFx)
	: Executer(app_central,script)
	, m_fxScriptItem(script)
	, m_currentLineNum(0)
	, m_breakOnCancel(false)
	, m_disableMultiStart(false)
{
	parentFxItem = parentFx;
	if (script)
		FxScriptItem::rawToScript(script->rawScript(), m_script);

	// qDebug("new script executer: %s: duration: %llims",script->name().toLocal8Bit().data(),m_script.execDuration());
}

ScriptExecuter::~ScriptExecuter()
{
	emit listProgressStepChanged(0, 0);

	while (!m_clonedSceneList.isEmpty()) {
		FxSceneItem *scene = m_clonedSceneList.takeFirst();
		if (!scene->isOnStageIntern() && !scene->isActive()) {
			emit wantedDeleteFxScene(scene);
		} else {
			scene->setDeleteOnFinished();
		}
	}
	// qDebug() << "script executer destroyed";
}

/**
 * @brief ScriptExecuter::getTargetFxItemFromPara
 * @param line
 * @param paras
 * @param fxList
 * @return
 *
 * This should parse strings like:
 * [id|text] number
 * [id|text] text
 * number
 * text
 *
 */
QString ScriptExecuter::getTargetFxItemFromPara(FxScriptLine *line , const QString &paras, FxItemList &fxList)
{
	QString returnparas;

	// parse parameter string
	QStringList tlist = QtStaticTools::parameterStringSplit(paras);

	int searchmode = 0;

	if (tlist.size()) {
		QString p1 = tlist.first();
		QString p1low = p1.toLower();
		if (p1low == "id") {
			searchmode = 1;
			tlist.removeFirst();
		}
		else if (p1low == "text" || p1low == "name") {
			searchmode = 2;
			tlist.removeFirst();
		}
		else {
			// Test if parameter maybe an ID or a FX name
			bool ok;
			int id = p1.toInt(&ok);
            Q_UNUSED(id)
			if (ok) {
				searchmode = 1;
			} else {
				searchmode = 2;
			}
		}
	}

	if (tlist.isEmpty()) {
		LOGERROR(tr("Script '%1':  Line #%2: <font color=darkOrange>Target parameter missing!</font>")
				 .arg(m_fxScriptItem->name())
				 .arg(line->lineNumber()));

		return returnparas;
	}

	if (searchmode == 1) { // search for an FX by ID
		int id = tlist.takeFirst().toInt();

		FxItem *fx = FxItem::findFxById(id);
		if (!fx) {
			searchmode = 0;
			LOGTEXT(tr("Script '%1':  Line #%2: <font color=darkOrange>FX with ID #%3 not found!</font>")
					.arg(m_fxScriptItem->name())
					.arg(line->lineNumber())
					.arg(id));
		} else {
			fxList.append(fx);
		}
	}
	else if (searchmode == 2) { // search for an FX by name
		QString name = tlist.takeFirst();
		QString sname = name;
		FxSearchMode sm = FXSM_EXACT;
		if (name.startsWith("*")) {
			sm = FXSM_RIGHT;
			sname = name.mid(1);
		}
		if (name.endsWith("*")) {
			sname.chop(1);
			if (sm == FXSM_RIGHT)
				sm = FXSM_LIKE;
			else
				sm = FXSM_LEFT;
		}
		QList<FxItem*>list = FxItem::findFxByName(sname,sm);
		if (list.isEmpty()) {
			LOGTEXT(tr("Script '<font color=#6666ff>%1</font>': Line #%2:  <font color=darkOrange>No FX with name '%3' found!</font>")
					.arg(m_fxScriptItem ? m_fxScriptItem->name() : QString("no fxitem"))
					.arg(line->lineNumber())
					.arg(name));
		}
		else {
			fxList.append(list);
		}
	}

	for (int t=0; t<tlist.size(); t++) {
		if (t>0)
			returnparas += " ";
		returnparas += tlist.at(t);
	}

	return returnparas;
}

QString ScriptExecuter::getFirstParaOfString(QString &parastr)
{
	parastr = parastr.simplified();

	QString first = parastr.section(' ', 0, 0);
	parastr.remove(0, first.size() + 1);

	return first;
}

int ScriptExecuter::getPos(QString &restPara)
{
	QStringList paras = restPara.split(' ', QT_SKIP_EMPTY_PARTS);
	int pos = -2;

	QMutableListIterator<QString> it(paras);
	while (it.hasNext()) {
		QString tag = it.next();
		if (tag.toLower() == "pos") {
			pos = -1;
			it.remove();
			if (it.hasNext()) {
				QString timestr = it.next();
				int time = QtStaticTools::timeStringToMS(timestr);
				pos = time;
				it.remove();
			}
			break;
		}
	}

	if (pos > -2) {
		restPara.clear();
		for (const QString &val : paras) {
			if (restPara.size())
				restPara += " ";
			restPara += val;
		}
	}

	return pos;
}


/**
 * @brief This function does return the temorary copied FxItems for a given parent FxItem
 * @param fx
 * @return
 *
 * @note Only FxItems that were created by this executer instance are returned!
 */
FxItemList ScriptExecuter::getExecuterTempCopiesOfFx(FxItem *fx) const
{
    QList<FxItem*> list;
    foreach (FxItem *fxcopy, m_clonedSceneList) {
        if (fxcopy->tempCopyOrigin() == fx)
            list.append(fxcopy);
    }
    return list;
}

bool ScriptExecuter::executeLine(FxScriptLine *line, bool & reExecDelayed)
{
	bool ok = true;

	if (line->execTimeMs() >= 0) {
		if (line->execTimeMs() <= currentRunTimeMs()) {
			reExecDelayed = false;
		} else {
			reExecDelayed = true;
			setEventTargetTimeAbsolute(line->execTimeMs());
			return false;
		}
	} else {
		reExecDelayed = false;
	}

	const QString &cmd = line->command();
	KEY_WORD key = FxScriptLine::keywords.keyNumber(cmd);
	switch (key) {
	case KW_WAIT:
		{
			qint64 delayMs = QtStaticTools::timeStringToMS(line->parameters());
			setEventTargetTimeRelative(delayMs);
		}
		return false;

	case KW_START:
		ok = executeCmdStart(line);
		break;

	case KW_STOP:
		ok = executeCmdStop(line);
		break;

	case KW_FADEIN:
		ok = executeFadeIn(line);
		break;

	case KW_FADEOUT:
		ok = executeFadeOut(line);
		break;

	case KW_LOOP:
		if (myState == EXEC_RUNNING) {
			ok = executeLoopExt(line);
		}
		break;

	case KW_YADI_DMX_MERGE:
		ok = executeYadiDMXMergeMode(line);
		break;

	case KW_GRAP_SCENE:
		ok = executeGrabScene(line);
		break;

	case KW_BLACK:
		ok = executeBlack(line);
		break;

	case KW_REMOTE:
		ok = executeRemote(line);
		break;

	case KW_FADE_VOL:
		ok = executeFadeVolume(line);
		break;

	case KW_MODE:
		ok = executeMode(line);
		break;

	default:
		ok = false;
		LOGERROR(tr("<font color=darkOrange>Command '%1' not supported by scripts</font>").arg(cmd));
		break;
	}

	if (!ok) {
		LOGERROR(tr("Script '<font color=#6666ff>%1</font>': Line #%2: <font color=darkOrange>Failed to execute script line</font> ('<font color=#6666ff>%3</font>')%4")
				 .arg(m_fxScriptItem ? m_fxScriptItem->name() : QString("no script name"))
				 .arg(line->lineNumber())
				 .arg(QString("%1 %2").arg(line->command(), line->parameters()))
				 .arg(m_lastScriptError.size() ? QString(": %1").arg(m_lastScriptError) : QString()));

		m_lastScriptError.clear();
	}

	return true;
}

bool ScriptExecuter::executeCmdStart(FxScriptLine *line)
{
	// Try to find the target fx here

	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	// qDebug() << Q_FUNC_INFO << restparas;

	bool ok = true;

	foreach (FxItem *fx, fxlist) {
		switch (fx->fxType()) {
		case FX_AUDIO:
		{
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
			int pos = getPos(restparas);
			// qDebug() << "pos" << pos << "restparas" << restparas;

			if (fxa->isFxClip) {
				ok &= myApp.unitVideo->startFxClip(static_cast<FxClipItem*>(fx));
			}
			else if (pos >= -1) {
				ok &= myApp.unitAudio->startFxAudioAt(fxa, this, pos);
			}
			else {
				ok &= myApp.unitAudio->startFxAudio(fxa, this);
			}
		}
			break;
		case FX_SCENE:
			/// @todo it would be better to copy the scene and actually fade in the new instance here
			/// Further the executer is not handed over to scene
			ok &= myApp.unitLight->startFxScene(static_cast<FxSceneItem*>(fx));
			break;
		default:
			LOGERROR(tr("Script '%1': Executing of target is not supported! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
		}
	}

	return ok;
}

bool ScriptExecuter::executeCmdStop(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	bool ok = true;

	foreach (FxItem *fx, fxlist) {

		switch (fx->fxType()) {
		case FX_SCENE:
			{
				FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
				if (!scene->isOnStageIntern()) {
					LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Target '%2' not on stage. Line %3")
							.arg(originFxItem->name())
							.arg(scene->name())
							.arg(line->lineNumber()));
					QList<FxItem*>fxlist = getExecuterTempCopiesOfFx(fx);
					foreach (FxItem *fx_cp, fxlist) {
						FxSceneItem *fxsc_cp = dynamic_cast<FxSceneItem*>(fx_cp);
						if (fxsc_cp) {
							LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Stop temp copy of '%2' -> '%3'")
									.arg(originFxItem->name())
									.arg(scene->name())
									.arg(fxsc_cp->name()));
							myApp.unitLight->stopFxScene(fxsc_cp);
						}
					}
				} else {
					LOGTEXT(tr("Script <font color=darkGreen>'%1'</font>: Stop FX '%2'")
							.arg(originFxItem->name()).arg(scene->name()));
					return myApp.unitLight->stopFxScene(scene);
				}
			}
			break;

		case FX_AUDIO:
			if (myApp.unitAudio->stopFxAudioWithID(fx->id())) {
				LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Stopped FX audio '%2'")
						.arg(originFxItem->name()).arg(fx->name()));
			}
			else {
				LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': FX audio '%2' not stopped, cause is already idle")
						.arg(originFxItem->name()).arg(fx->name()));
			}
			break;

		default:
			LOGERROR(tr("Script '%1': Execute stop command on target is not supported! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
		}

	}

	if (restparas.size()) {
		LOGERROR(tr("Script '%1': Command STOP does not support parameters: '%2'! Line #%3")
				 .arg(m_fxScriptItem->name())
				 .arg(restparas)
				 .arg(line->lineNumber()));
		ok = false;
	}
	return ok;
}

bool ScriptExecuter::executeFadeIn(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	qint64 fadein_time_ms = 0;
	// Do we have more parameters? This could be the fadein time
	if (restparas.size()) {
		fadein_time_ms = QtStaticTools::timeStringToMS(restparas);
	}

	bool ok = true;
	foreach (FxItem *fx, fxlist) {

		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': FADEIN must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		FxItemList clonelist = getExecuterTempCopiesOfFx(scene);

		// now clone FxSceneItem
		FxSceneItem *clonescene = nullptr;

		if (clonelist.size()) {
			clonescene = dynamic_cast<FxSceneItem*>(clonelist.first());
		} else {
			clonescene = new FxSceneItem(*scene);
			clonescene->setName(tr("%1:%2_tmp").arg(originFxItem->name()).arg(scene->name()));
			clonescene->setIsTempCopyOf(scene);
			m_clonedSceneList.append(clonescene);
		}
		if (!clonescene)
			continue;

		bool is_active = clonescene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN, fadein_time_ms);
		if (is_active) {
			myApp.unitLight->setSceneActive(clonescene);
		} else {
			m_lastScriptError = tr("not active");
		}
		ok &= is_active;
	}

	return ok;
}

bool ScriptExecuter::executeFadeOut(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	qint64 fadeout_ms = 0;
	bool fadeout_all = false;
	// Do we have more parameters? This could be the fadeout time
	if (restparas.size()) {
		if (restparas.toLower().startsWith("all")) {
			fadeout_all = true;
			restparas = restparas.mid(3).simplified();
		}
		fadeout_ms = QtStaticTools::timeStringToMS(restparas);
	}


	bool ok = true;
	foreach (FxItem *fx, fxlist) {

		if (fx->fxType() == FX_AUDIO) {
			FxAudioItem *fxa = dynamic_cast<FxAudioItem*>(fx);
			if (fxa) {
				if (fadeout_ms > 0) {
					myApp.unitAudio->fadeoutFxAudio(fxa, fadeout_ms);
				} else {
					myApp.unitAudio->stopFxAudio(fxa);
				}
			}
			continue;
		}

		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': FADEOUT must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		FxItemList clonelist;
		if (fadeout_all) {
			clonelist = FxItem::getTempCopiesOfFx(scene);
			clonelist.append(scene);
		} else {
			clonelist = getExecuterTempCopiesOfFx(scene);
		}
		foreach (FxItem *clonefx, clonelist) {
			FxSceneItem *clonescene = dynamic_cast<FxSceneItem*>(clonefx);
			if (!clonescene)
				continue;

			bool is_active = clonescene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT, fadeout_ms);
			if (! (is_active | fadeout_all) ) {
				ok = false;
				m_lastScriptError = tr("not active");
			}
		}
	}

	return ok;
}

bool ScriptExecuter::executeYadiDMXMergeMode(FxScriptLine *line)
{
	QStringList paras = line->parameters().split(" ", QT_SKIP_EMPTY_PARTS);

	if (paras.size() < 2) {
		m_lastScriptError = tr("Missing parameter. Format is: DMXMERGE <input> <mergemode>");
		return false;
	}

	quint32 input = paras[0].toInt()-1;
	if (input > 4) {
		m_lastScriptError = tr("Input number should be in range of 1 - 4");
		return false;
	}
	int mode = 0;
	QString mtxt = paras[1].toLower();

	if (mtxt == "htp" || mtxt.startsWith("def")) {
		mode = 0;
	}
	else if (mtxt == "dmxin") {
		mode = 3;
	}
	else if (mtxt == "usb") {
		mode = 4;
	}
	else {
		m_lastScriptError = tr("Unknown merge mode %1. Try HTP, DMXIN or USB").arg(paras[1]);
		return false;
	}

	bool ok = myApp.unitLight->setYadiInOutMergeMode(input, mode);
	if (!ok) {
		m_lastScriptError = tr("Could not send command to yadi device!");
	}

	return ok;
}

bool ScriptExecuter::executeLoopExt(FxScriptLine *line)
{
	line->incLoopCount();

	QStringList paras = line->parameters().split(" ", QT_SKIP_EMPTY_PARTS);
	if (paras.isEmpty()) { // simple loop without parameters from beginning
		m_currentLineNum = -1;
		return true;
	}

	QString targetline_str = paras.takeFirst();
	int linenum = targetline_str.toInt();
	if (linenum == 0 || linenum > m_script.size()) {
		m_lastScriptError = tr("Target line number not valid");
		return false;
	}

	if (paras.isEmpty()) {
		m_currentLineNum = linenum - 2;
		return true;
	}

	// evaluate loop count;
	QString targetcnt_str = paras.takeFirst();
	int targetcount = targetcnt_str.toInt();
	if (targetcount == 0) {
		m_lastScriptError = tr("Target loop count not valid");
		return false;
	}

	if (line->loopCount() >= targetcount) {			// loop count reached
		line->clearLoopCount();
	} else {
		m_currentLineNum = linenum - 2;
	}

	return true;
}

bool ScriptExecuter::executeGrabScene(FxScriptLine *line)
{
	QString parastr = line->parameters();

	QString source = getFirstParaOfString(parastr).toLower();
	if (!source.startsWith("in") && !source.startsWith("out")) {
		m_lastScriptError = tr("Missing universe source parameter: [INPUT | OUTPUT]");
		return false;
	}

	FxItemList fxlist;
	parastr = getTargetFxItemFromPara(line, parastr, fxlist);
	if (fxlist.isEmpty()) {
		m_lastScriptError = tr("Missing target scene parameter");
		return false;
	}

	for (FxItem *fx : fxlist) {
		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': GRAPSCENE must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		int backActiveChannels = 0;
		myApp.unitLight->fillSceneFromInputUniverses(scene, &backActiveChannels);
		if (backActiveChannels == 0) {
			LOGTEXT(tr("Grab into scene '%1': No tube was active -> set last tube to 1!")
					.arg(scene->name()));
			scene->tube(scene->tubeCount()-1)->targetValue = 1;
		}
		scene->setModified(false);
	}

	return true;
}

bool ScriptExecuter::executeBlack(FxScriptLine *line)
{
	QString parastr = line->parameters();

	QString type = getFirstParaOfString(parastr).toLower();

	if (type == "video") {
		if (myApp.unitVideo) {
			myApp.unitVideo->videoBlack(0);
		}

		return true;
	}
	else if (type == "scene" || type == "light") {
		int timems = 0;
		if (parastr.size())
			timems = QtStaticTools::timeStringToMS(parastr);

		myApp.unitLight->black(timems);
		return true;
	}

	m_lastScriptError = tr("Unknown parameter type '%1' for BLACK command").arg(type);
	return false;
}

bool ScriptExecuter::executeRemote(FxScriptLine *line)
{
	QString parastr = line->parameters();
	QString host = getFirstParaOfString(parastr);

	myApp.connectToRemote(host, NET_TCP_PORT, parastr);

	return true;
}

bool ScriptExecuter::executeFadeVolume(FxScriptLine *line)
{
	QString parastr = line->parameters();

	int slotno = 0;
	int targetVol = 0;
	int fadeTimeMs = 0;

	QString type = getFirstParaOfString(parastr).toLower();
	if (type == "slot") {
		slotno = getFirstParaOfString(parastr).toInt();
	}
	else {
		m_lastScriptError = tr("Unknown parameter type '%1' for FADEVOL command: (use [slot])").arg(type);
		return false;
	}

	if (!slotno) {
		m_lastScriptError = tr("Missing slot number in FADEVOL command");
		return false;
	}
	slotno--;

	QString targetVolStr = getFirstParaOfString(parastr);
	if (targetVolStr.endsWith('%')) {
		targetVolStr.chop(1);
		targetVol = targetVolStr.toInt();
		if ((targetVol < 0) || (targetVol > 100)) {
			m_lastScriptError = tr("Volume target out of range: %1. Must be [0%:100%]").arg(targetVol);
			return false;
		}
		targetVol = targetVol * MAX_VOLUME / 100;
	} else {
		targetVol = targetVolStr.toInt();
		if ((targetVol < 0) || (targetVol > MAX_VOLUME)) {
			m_lastScriptError = tr("Volume target out of range: %1. Must be [0:%2]").arg(targetVol).arg(MAX_VOLUME);
			return false;
		}
	}

	if (!parastr.isEmpty())
		fadeTimeMs = QtStaticTools::timeStringToMS(parastr);

	myApp.unitAudio->fadeVolTo(slotno, targetVol, fadeTimeMs);

	return true;
}

bool ScriptExecuter::executeMode(FxScriptLine *line)
{
	const QStringList paras = line->parameters().split(',');
	bool ok = true;

	for (const QString &opt : paras) {
		QString lc = opt.toLower();
		if (lc == "breakoncancel") {
			m_breakOnCancel = true;
		}
		else if (lc == "disablemultistart") {
			m_disableMultiStart = true;
		}
		else if (lc == "enablemultistart") {
			m_disableMultiStart = false;
		}
		else {
			m_lastScriptError += tr("Unknown MODE option: %1\n").arg(lc);
			ok = false;
		}
	}

	if (!ok)
		m_lastScriptError.chop(1);
	return ok;
}

bool ScriptExecuter::executeSingleCmd(const QString &linestr)
{
	QString cmd = linestr.section(' ',0,0);
	QString paras = linestr.section(' ',1);
	FxScriptLine line(cmd, paras);
	ScriptExecuter exe(AppCentral::ref(), nullptr, nullptr);

	bool reexecdelayed = false;
	bool ok = exe.executeLine(&line, reexecdelayed);

	return ok;
}

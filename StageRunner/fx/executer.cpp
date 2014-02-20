#include "executer.h"
#include "execcenter.h"
#include "log.h"
#include "fxlist.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "usersettings.h"

using namespace LIGHT;

Executer::Executer(AppCentral &app_central, FxItem *originFx)
	: myApp(app_central)
	, originFxItem(originFx)
	, parentFxItem(0)
	, eventTargetTimeMs(0)
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

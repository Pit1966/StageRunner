#include "executer.h"
#include "fxlist.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"


Executer::Executer(AppCentral &app_central)
	: myApp(app_central)
	, originFxItem(0)
	, curFx(0)
	, nxtFx(0)
	, eventTargetTimeMs(0)
	, isInExecLoopFlag(false)
	, isWaitingForAudio(false)
	, myState(EXEC_IDLE)
	, use_cnt(0)
{
	runTime.start();
}

Executer::~Executer()
{
	qDebug() << "Executer destroyed" << getIdString();
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

void Executer::setExecLoopFlag(bool state)
{
	isInExecLoopFlag = state;
	if (state) {
		myState = EXEC_RUNNING;
	} else {
		myState = EXEC_IDLE;
	}
}


FxListExecuter::FxListExecuter(AppCentral &app_central, FxList *fx_list)
	: Executer(app_central)
	, fxList(fx_list)
{
	init();
}

void FxListExecuter::setCurrentFx(FxItem *fx)
{
	if (fx != curFx) {
		curFx = fx;
		emit currentFxChanged(fx);
	}
}

void FxListExecuter::setNextFx(FxItem *fx)
{
	if (fx != nxtFx) {
		nxtFx = fx;
		emit nextFxChanged(fx);
	}
}

/**
 * @brief process next event in the FxList
 * @return true, if something was processed
 */
bool FxListExecuter::processExecuter()
{
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
	while (fx && cue_time == 0) {
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
	if (msg.executer != this || msg.ctrlCmd != CMD_AUDIO_STATUS_CHANGED)
		return;		// Not for me

	qDebug("%s: Received audio msg: AudioStatus %d, AudioCmd %d"
		   ,Q_FUNC_INFO,msg.currentAudioStatus,msg.ctrlCmd);

	if (isWaitingForAudio) {
		if (msg.currentAudioStatus == AUDIO_RUNNING) {
			qDebug("   Wait for audio end");
		}
		else if (msg.currentAudioStatus == AUDIO_IDLE) {
			qDebug("   Audio now idle");
			eventTargetTimeMs = runTime.elapsed();
			isWaitingForAudio = false;
		}
	}
}

void FxListExecuter::lightCtrlReceiver(FxSceneItem *fxs)
{
	qDebug("FxListExecuter(%p): Scene '%s' status changed to '%s'"
		   , this, fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::sceneCueReceiver(FxSceneItem *fxs)
{
	qDebug("FxListExecuter(%p): Scene '%s' cue ended. Status: '%s'"
		   , this, fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::init()
{
	currentAudioStatus = AUDIO_IDLE;

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
	if (!curFx) {
		next = fxList->getFirstFx();
	} else {
		next = fxList->findSequenceFollower(curFx);
	}

	setCurrentFx(next);

	return next;
}

/**
 * @brief Execute next CUE command in Fx sequence
 * @param fx Pointer to the FxItem that will be executed
 * @return cue time in ms (time the execution of the command lasts) or -1, if Fx is at the end of the
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
		return 0;
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
		scene->setSeqStatus(SCENE_PRE_DELAY);
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
		audio->setSeqStatus(AUDIO_PRE_DELAY);
		break;
	case AUDIO_PRE_DELAY:
		if (audio->holdTime()) {
			cue_time = audio->holdTime();
		} else {
			isWaitingForAudio = true;
			cue_time = 1000000;
		}
		myApp.unitAudio->startFxAudio(audio,this);
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

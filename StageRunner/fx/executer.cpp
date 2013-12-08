#include "executer.h"
#include "fxlist.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"


Executer::Executer(AppCentral *app_central)
	: myApp(app_central)
	, originFxItem(0)
	, curFx(0)
	, nxtFx(0)
	, eventTargetTimeMs(0)
	, isInExecLoopFlag(false)
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
	emit deleteMe(this);
}


FxListExecuter::FxListExecuter(AppCentral *app_central, FxList *fx_list)
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
		// Execute next CUE command in FxScene
		cue_time = cue_fx(fx);
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

void FxListExecuter::setPlayListInitialVolume(int vol)
{
	playlist_initial_vol = vol;
}

bool FxListExecuter::runExecuter(int idx)
{
	if (!fxList) return false;

	if (idx < 0) {
		if (nxtFx) {
			return runFxItem(nxtFx);
		} else {
			idx = 0;
		}
	}

	if (idx >= fxList->size()) return false;

	return runFxItem(fxList->getFxByListIndex(idx));
}

bool FxListExecuter::runFxItem(FxItem *fx)
{
	if (!fx) {
		if (curFx) {
			setCurrentFx(0);
			emit fxItemExecuted(0,this);
		}
		if (nxtFx) {
			setNextFx(0);
		}
		emit deleteMe(this);
		return false;
	}

	if (!fxList || !fxList->contains(fx)) {
		DEBUGERROR("FxListExecuter: fxList not initialized or Fx not in list");
		return false;
	}

	if (curFx) {
		switch(fx->fxType()) {
		case FX_AUDIO:
			unitAudio->fadeoutFxAudio(this);
			break;
		default:
			break;
		}
	}

	setCurrentFx(fx);

	bool started = false;

	switch(fx->fxType()) {
	case FX_AUDIO:
		started = runFxAudioItem(static_cast<FxAudioItem*>(fx));
		break;
	case FX_SCENE:
		started = runFxSceneItem(static_cast<FxSceneItem*>(fx));
		break;

	default:
		break;
	}

	FxItem * next = fxList->findSequenceFollower(fx);

	setNextFx(next);

	return started;
}

bool FxListExecuter::runFxAudioItem(FxAudioItem *fxa)
{
	bool started = false;

	if (playlist_initial_vol) {
		fxa->initialVolume = playlist_initial_vol;
	}
	if (unitAudio->startFxAudio(fxa, this)) {
		emit fxItemExecuted(fxa,this);
		started = true;
	}

	return started;
}

bool FxListExecuter::runFxSceneItem(FxSceneItem *fxs)
{
	bool started = false;

	if (unitLight->startFxSceneSimple(fxs)) {
		emit fxItemExecuted(fxs, this);
		started = true;
	}

	return started;
}

void FxListExecuter::fadeEndExecuter()
{
	if (currentFx()) {
		unitAudio->fadeoutFxAudio(this);
	}
	currentAudioStatus = AUDIO_IDLE;

	// This ends the executer
	runFxItem(0);
}

void FxListExecuter::audioCtrlReceiver(AudioCtrlMsg msg)
{
	if (isInExecLoopFlag) return;

	if (msg.executer != this)
		return;		// Not for me

	switch (msg.ctrlCmd) {
	case CMD_AUDIO_STOP:
	case CMD_AUDIO_FADEOUT:
		qDebug("%s audio msg: CMD:%d, AudioStatus:%d, Executer: %p",Q_FUNC_INFO,msg.ctrlCmd, msg.currentAudioStatus, msg.executer);
		setCurrentFx(0);
		emit fxItemExecuted(0,this);
		currentAudioStatus = AUDIO_IDLE;
		emit deleteMe(this);
		break;

	case CMD_STATUS_REPORT:
		if (curFx == msg.fxAudio && msg.currentAudioStatus != currentAudioStatus) {
			if (debug > 1) qDebug("%s audio msg: CMD:%d, AudioStatus:%d, Executer: %p",Q_FUNC_INFO,msg.ctrlCmd, msg.currentAudioStatus, msg.executer);
			if (msg.currentAudioStatus == AUDIO_IDLE) {
				runFxItem(nxtFx);
			}
			currentAudioStatus = msg.currentAudioStatus;
		}
		break;

	default:
		break;
	}

}

void FxListExecuter::lightCtrlReceiver(FxSceneItem *fxs)
{
	qDebug("FxListExecuter: Scene '%s' status changed to '%s'"
		   , fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::sceneCueReceiver(FxSceneItem *fxs)
{
	qDebug("FxListExecuter: Scene '%s' cue ended. Status: '%s'"
		   , fxs->name().toLocal8Bit().data(), fxs->statusString().toLocal8Bit().data());

}

void FxListExecuter::init()
{
	currentAudioStatus = AUDIO_IDLE;
	playlist_initial_vol = 0;

	unitLight = myApp->unitLight;
	unitAudio = myApp->unitAudio;
	unitFx = myApp->unitFx;

	connect(unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(unitAudio,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(unitLight,SIGNAL(sceneChanged(FxSceneItem*)),this,SLOT(lightCtrlReceiver(FxSceneItem*)),Qt::DirectConnection);
	connect(unitLight,SIGNAL(sceneCueReady(FxSceneItem*)),this,SLOT(sceneCueReceiver(FxSceneItem*)),Qt::DirectConnection);
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

	switch (fx->fxType()) {
	case FX_SCENE:
		return cue_fx_scene(reinterpret_cast<FxSceneItem*>(fx));
	case FX_AUDIO:
		return cue_fx_audio(reinterpret_cast<FxAudioItem*>(fx));
	default:
		return 0;
	}
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
		if ( scene->initSceneCommand(MIX_INTERN,CMD_SCENE_FADEIN,cue_time) ) {
			unitLight->setSceneActive(scene);
		}
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
		cue_time = audio->holdTime();
		unitAudio->startFxAudio(audio,this);
		audio->setSeqStatus(AUDIO_PLAYTIME);
		break;
	case AUDIO_PLAYTIME:
		cue_time = audio->fadeOutTime();
		unitAudio->fadeoutFxAudio(audio,cue_time);
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

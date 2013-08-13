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
{
}

Executer::~Executer()
{
	qDebug() << "Executer destroyed" << getIdString();

}

void Executer::setIdString(const QString &str)
{
	idString = str;
}

QString Executer::getIdString() const
{
	if (idString.isEmpty()) {
		return QString("0x%1").arg(quint64(this));
	} else {
		return idString;
	}
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
		unitAudio->fadeoutFxAudio(this);
	}

	setCurrentFx(fx);

	bool started = false;

	switch(fx->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
			if (playlist_initial_vol) {
				fxa->initialVolume = playlist_initial_vol;
			}
			if (unitAudio->startFxAudio(fxa, this)) {
				emit fxItemExecuted(fx,this);
				started = true;
			}
		}
		break;
	default:
		break;
	}

	FxItem * next = fxList->findSequenceFollower(fx);

	setNextFx(next);

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

void FxListExecuter::init()
{
	currentAudioStatus = AUDIO_IDLE;
	playlist_initial_vol = 0;

	unitLight = myApp->unitLight;
	unitAudio = myApp->unitAudio;

	connect(unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(unitAudio,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
}



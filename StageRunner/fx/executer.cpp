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
		if (curFx) {
			return runFxItem(curFx);
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
			curFx = 0;
			emit currentFxChanged(0);
			emit fxItemExecuted(0,0);
		}
		if (nxtFx) {
			nxtFx = 0;
			emit nextFxChanged(0);
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


	if (curFx != fx) {
		curFx = fx;
		emit currentFxChanged(fx);
	}

	switch(fx->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
			if (playlist_initial_vol)
				fxa->initialVolume = playlist_initial_vol;
			unitAudio->startFxAudio(fxa, this);
			emit fxItemExecuted(fx,this);
		}
		break;
	default:
		break;
	}


	FxItem * next = fxList->findSequenceFollower(fx);
	if (next != nxtFx) {
		qDebug("emit nextFxChanged: %p",next);
		nxtFx = next;
		emit nextFxChanged(next);
	}

	return true;
}

void FxListExecuter::fadeEndExecuter()
{
	if (currentFx()) {
		unitAudio->fadeoutFxAudio(this);
		qDebug() << "fadeEnd" << currentFx()->name();
	}
	currentAudioStatus = AUDIO_IDLE;

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
		curFx = 0;
		currentAudioStatus = AUDIO_IDLE;

		emit deleteMe(this);
		break;
	case CMD_STATUS_REPORT:
		if (curFx == msg.fxAudio && msg.currentAudioStatus != currentAudioStatus) {
			qDebug("%s audio msg: CMD:%d, AudioStatus:%d, Executer: %p",Q_FUNC_INFO,msg.ctrlCmd, msg.currentAudioStatus, msg.executer);
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



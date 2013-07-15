#include "fxlistexecuter.h"
#include "fxlist.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"

FxListExecuter::FxListExecuter(AppCentral *app_central, FxList *fx_list)
	: myApp(app_central)
	, fxList(fx_list)
{
	init();
}

void FxListExecuter::setFxList(FxList *fx_list)
{
	fxList = fx_list;
}

bool FxListExecuter::runExecuter(int idx)
{
	if (!fxList) return false;
	fxList->setAutoProceedSequence(true);

	if (idx < 0) {
		if (fxList->nextFx()) {
			return runFxItem(fxList->nextFx());
		} else {
			idx = 0;
		}
	}

	if (idx >= fxList->size()) return false;

	return runFxItem(fxList->getFxByListIndex(idx));
}

bool FxListExecuter::runFxItem(FxItem *fx)
{
	if (!FxItem::exists(fx)) return false;

	if (curFx) {

		unitAudio->fadeoutFxAudio(static_cast<FxAudioItem*>(curFx));
	}

	curFx = fx;

	switch(fx->fxType()) {
	case FX_AUDIO:
		unitAudio->startFxAudio(static_cast<FxAudioItem*>(fx));
		break;
	default:
		break;
	}

	fxList->setNextFx(fxList->findSequenceFollower(fx));

	return true;
}

void FxListExecuter::audioCtrlReceiver(AudioCtrlMsg msg)
{
	switch (msg.ctrlCmd) {
	case CMD_AUDIO_STOP:
	case CMD_AUDIO_FADEOUT:
		curFx = 0;
		currentAudioStatus = AUDIO_IDLE;
		break;
	case CMD_STATUS_REPORT:
		if (curFx == msg.fxAudio && msg.currentAudioStatus != currentAudioStatus) {
			qDebug("%s audio msg: CMD:%d, AudioStatus:%d",Q_FUNC_INFO,msg.ctrlCmd, msg.currentAudioStatus);
			if (msg.currentAudioStatus == AUDIO_IDLE) {
				runFxItem(fxList->findSequenceFollower(curFx));
				curFx = 0;
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
	curFx = 0;
	currentAudioStatus = AUDIO_IDLE;

	unitLight = myApp->unitLight;
	unitAudio = myApp->unitAudio;

	connect(unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(unitAudio,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
}

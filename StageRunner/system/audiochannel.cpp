#include "audiochannel.h"

#include "fx/fxaudioitem.h"
#include "system/log.h"
#include "config.h"

#include <QTime>
#include <QApplication>

AudioSlot::AudioSlot()
	: QObject()
{

	run_status = AUDIO_IDLE;
	slotNumber = -1;

	media_obj = new Phonon::MediaObject();
	audio_out = new Phonon::AudioOutput(Phonon::MusicCategory);
	path = Phonon::createPath(media_obj, audio_out);

	connect(media_obj,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(setAudioState(Phonon::State,Phonon::State)));
	connect(media_obj,SIGNAL(finished()),this,SLOT(setFinished()));

}

AudioSlot::~AudioSlot()
{
	delete audio_out;
	delete media_obj;

}

bool AudioSlot::startFxAudio(FxAudioItem *fxa)
{
	LOGTEXT(tr("Start FxAudio: %1 in audio slot %2")
			.arg(fxa->displayName()).arg(slotNumber+1));

	run_status = AUDIO_INIT;
	run_time.start();

	Phonon::MediaSource source(fxa->filePath());
	media_obj->setCurrentSource(source);

	media_obj->play();


	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			DEBUGTEXT("FxAudio started: '%s'' (%dms delayed)",fxa->displayName().toAscii().data(),run_time.elapsed());
			ok = true;
			run_time.start();
		}
		else if (run_status == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->displayName().toAscii().data());
			break;
		}

	}
	if (media_obj->state() == Phonon::StoppedState) {
		DEBUGERROR("FxAudio '%s'' did not start after %dms -> Audio Slot %d set to Error/Idle state"
				   ,fxa->displayName().toAscii().data(),run_time.elapsed(),slotNumber+1);
		run_status = AUDIO_ERROR;
	}

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	if (run_status != AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		media_obj->stop();
		run_status = AUDIO_IDLE;
		return true;
	}
	return false;
}

void AudioSlot::setAudioState(Phonon::State newstate, Phonon::State oldstate)
{
	// Q_UNUSED(oldstate);
	// AudioStatus oldstat = run_status;

	switch (newstate) {
	case Phonon::LoadingState:
		DEBUGTEXT("AudioSlot:%d Loading",slotNumber+1);
		run_status = AUDIO_INIT;
		break;
	case Phonon::PlayingState:
		DEBUGTEXT("AudioSlot:%d Playing",slotNumber+1);
		run_status = AUDIO_RUNNING;
		break;
	case Phonon::StoppedState:
		DEBUGTEXT("AudioSlot:%d Stopped",slotNumber+1);
		if (run_status != AUDIO_INIT) {
			run_status = AUDIO_IDLE;
		}
		break;
	case Phonon::PausedState:
		DEBUGTEXT("AudioSlot:%d Paused",slotNumber+1);
		if (run_status != AUDIO_INIT) {
			run_status = AUDIO_IDLE;
		}
		break;
	case Phonon::ErrorState:
		DEBUGTEXT("AudioSlot:%d Error",slotNumber+1);
		run_status = AUDIO_ERROR;
		break;
	case Phonon::BufferingState:
		DEBUGTEXT("AudioSlot:%d Buffering",slotNumber+1);
		run_status = AUDIO_INIT;
		break;

	default:
		break;
	}

	if (oldstate != newstate) {
		// qDebug("emit status: %d %d",run_status, slotNumber);
		AudioCtrlMsg msg(run_status,slotNumber);
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::setFinished()
{
	DEBUGTEXT("AudioSlot:%d finished",slotNumber+1);
}


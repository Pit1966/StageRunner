#include "audiochannel.h"
#include "audioformat.h"

#include "fx/fxaudioitem.h"
#include "system/log.h"
#include "config.h"
#include "audioiodevice.h"

#include <QTime>
#include <QApplication>
#ifdef IS_QT5
#include <QMediaContent>
#endif

AudioSlot::AudioSlot()
	: QObject()
{

	run_status = AUDIO_IDLE;
	slotNumber = -1;
	probe_init_f = false;

	audio_io = new AudioIODevice(AudioFormat::defaultFormat());
	audio_output = new QAudioOutput(AudioFormat::defaultFormat(),this);

	connect(audio_output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(on_audio_output_status_changed(QAudio::State)));
	connect(audio_io,SIGNAL(readReady()),this,SLOT(on_audio_io_read_ready()));
	connect(audio_io,SIGNAL(vuLevelChanged(int,int)),this,SLOT(on_vulevel_changed(int,int)));

}

AudioSlot::~AudioSlot()
{
	delete audio_output;
	delete audio_io;
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa)
{
	LOGTEXT(tr("Start FxAudio: %1 in audio slot %2")
			.arg(fxa->displayName()).arg(slotNumber+1));

	run_status = AUDIO_INIT;
	run_time.start();

	// Emit Control Msg to send Status of Volume
	AudioCtrlMsg msg(slotNumber);
	msg.volume = fxa->initialVolume;
	emit audioCtrlMsgEmitted(msg);

	// Set Filename of audio file
	audio_io->setSourceFilename(fxa->filePath());
	// Start decoding of audio file to default (pcm 44100, 2Ch, 16bit) format
	audio_io->start();
	// Feed Audio Device with decoded data
	audio_output->start(audio_io);

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			DEBUGTEXT("FxAudio started: '%s'' (%dms delayed)",fxa->displayName().toLatin1().data(),run_time.elapsed());
			ok = true;
			run_time.start();
		}
		else if (run_status == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->displayName().toLatin1().data());
			break;
		}

	}

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	emit vuLevelChanged(slotNumber,0,0);

	if (run_status != AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		audio_output->stop();
		audio_io->stop();

		return true;
	}

	return false;
}

void AudioSlot::setVolume(qint32 vol)
{

	LOGTEXT(tr("Change Volume for Audio Fx in slot %1: %2").arg(slotNumber+1).arg(vol));
}

void AudioSlot::on_audio_output_status_changed(QAudio::State state)
{
	qDebug("new audio output state: %d",state);
	AudioStatus current_state = run_status;

	switch (state) {
	case QAudio::ActiveState:
		run_status = AUDIO_RUNNING;
		break;
	case QAudio::IdleState:
		run_status = AUDIO_IDLE;
		break;
	case QAudio::SuspendedState:
	case QAudio::StoppedState:
		run_status = AUDIO_IDLE;
		audio_output->stop();
		break;
	}

	if (current_state != run_status) {
		// qDebug("emit status: %d %d",run_status, slotNumber);
		AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status);
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::on_audio_io_read_ready()
{
	qDebug("Audio io ready read");
	audio_output->stop();
	audio_io->stop();
}

void AudioSlot::on_vulevel_changed(int left, int right)
{
	emit vuLevelChanged(slotNumber, left, right);
}

#ifndef IS_QT5
void AudioSlot::setPhononAudioState(Phonon::State newstate, Phonon::State oldstate)
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
		probe_init_f = true;
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
		qDebug("emit status: %d %d",run_status, slotNumber);
		AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status);
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::setPhononFinished()
{
	DEBUGTEXT("AudioSlot:%d finished",slotNumber+1);
}
#endif

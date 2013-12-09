#include "audiochannel.h"
#include "audioformat.h"

#include "fx/fxaudioitem.h"
#include "system/log.h"
#include "config.h"
#include "audioiodevice.h"
#include "audiocontrol.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/usersettings.h"
#include "executer.h"
#include "execcenter.h"

#include <QTime>
#include <QApplication>
#ifdef IS_QT5
#include <QMediaContent>
#endif

AudioSlot::AudioSlot(AudioControl *parent)
	: QObject()
{
	audio_ctrl = parent;

	run_status = AUDIO_IDLE;
	slotNumber = -1;
	fade_out_initial_vol = 0;
	current_fx = 0;
	current_executer = 0;
	master_volume = MAX_VOLUME;

	// Vol Set Logging
	volset_timer.setSingleShot(true);
	volset_timer.setInterval(500);
	connect(&volset_timer,SIGNAL(timeout()),this,SLOT(on_volset_timer_finished()));

	audio_io = new AudioIODevice(AudioFormat::defaultFormat());
	audio_output = new QAudioOutput(AudioFormat::defaultFormat(),this);
	if (parent->myApp.userSettings->pAudioBufferSize > 90) {
		audio_output->setBufferSize(parent->myApp.userSettings->pAudioBufferSize);
	}

	connect(audio_output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(on_audio_output_status_changed(QAudio::State)));
	connect(audio_io,SIGNAL(readReady()),this,SLOT(on_audio_io_read_ready()),Qt::QueuedConnection);
	connect(audio_io,SIGNAL(vuLevelChanged(int,int)),this,SLOT(on_vulevel_changed(int,int)),Qt::QueuedConnection);
	connect(audio_io,SIGNAL(audioDurationDetected(qint64)),this,SLOT(setAudioDurationMs(qint64)));

	//Fadeout Timeline
	connect(&fadeout_timeline,SIGNAL(valueChanged(qreal)),this,SLOT(on_fade_out_frame_changed(qreal)));
	connect(&fadeout_timeline,SIGNAL(finished()),this,SLOT(on_fade_out_finished()));

}

AudioSlot::~AudioSlot()
{
	if (run_status != AUDIO_IDLE) {
		stopFxAudio();
	}

	delete audio_output;
	delete audio_io;
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa, Executer *exec)
{
	if (!audio_output) return false;

	if (exec && AppCentral::instance()->execCenter->useExecuter(exec)) {
		LOGTEXT(tr("Start FxAudio: %1 in audio slot %2 with Executer: %3")
				.arg(fxa->name()).arg(slotNumber+1).arg(exec->getIdString()));
	} else {
		exec = 0;
		LOGTEXT(tr("Start FxAudio: %1 in audio slot %2")
				.arg(fxa->name()).arg(slotNumber+1));
	}

	current_fx = fxa;
	current_executer = exec;
	run_status = AUDIO_INIT;
	run_time.start();

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = fxa->initialVolume;
	msg.executer = exec;
	emit audioCtrlMsgEmitted(msg);

	// Set Filename of audio file
	audio_io->setSourceFilename(fxa->filePath());
	// Start decoding of audio file to default (pcm 44100, 2Ch, 16bit) format
	audio_io->start(fxa->loopTimes);
	// Feed Audio Device with decoded data and set initial Volume
	setVolume(fxa->initialVolume);
	audio_output->start(audio_io);

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			if (debug > 1) DEBUGTEXT("FxAudio started: '%s'' (%dms delayed)"
									 ,fxa->name().toLocal8Bit().data(),run_time.elapsed());
			ok = true;
			run_time.start();
			current_fx = fxa;
		}
		else if (run_status == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->name().toLocal8Bit().data());
			current_fx = 0;
			break;
		}

	}

	if (exec) AppCentral::instance()->execCenter->freeExecuter(exec);

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	if (fadeout_timeline.state() == QTimeLine::Running) {
		fadeout_timeline.stop();
	}

	emit vuLevelChanged(slotNumber,0,0);

	if (run_status != AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		audio_io->stop();
		audio_output->stop();

		return true;
	} else {
		if (audio_io->isOpen()) {
			LOGERROR(tr("Stop Audio playing in slot %1: But audio channel is idle").arg(slotNumber+1));
			audio_output->stop();
			audio_io->stop();
		}
		return false;
	}

	return false;
}

bool AudioSlot::fadeoutFxAudio(int time_ms)
{
	// Fadeout only possible if audio is running
	if (audio_output->state() != QAudio::ActiveState) return false;

	if (!FxItem::exists(current_fx)) return false;

	// Set Fadeout time
	fade_out_initial_vol = current_volume;
	fadeout_timeline.setDuration(time_ms);

	// and start the time line ticker
	fadeout_timeline.start();
	LOGTEXT(tr("Fade out for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(current_fx->name()).arg(time_ms));

	return true;
}

void AudioSlot::setVolume(int vol)
{
	if (!audio_output) return;

	float level = (float)vol / MAX_VOLUME;
	if (master_volume >= 0) {
		level *= (float)master_volume / MAX_VOLUME;
	}
#ifdef IS_QT5
	audio_output->setVolume(level);
#endif
	current_volume = vol;

	volset_text = tr("Change Volume for Audio Fx in slot %1: %2 ").arg(slotNumber+1).arg(vol);
	volset_timer.start();
}

void AudioSlot::setMasterVolume(int vol)
{
	master_volume = vol;
	if (run_status != AUDIO_IDLE) {
		setVolume(current_volume);
	}
}

FxAudioItem *AudioSlot::currentFxAudio()
{
	if (!FxItem::exists(current_fx)) {
		return 0;
	}
	return current_fx;
}

Executer *AudioSlot::currentExecuter()
{
	if (current_fx) {
		return current_executer;
	} else {
		return 0;
	}
}

/**
 * @brief Get the playback position of current Audio Stream.
 * @return Time in ms or -1 if there is no audio running
 */
int AudioSlot::currentRunTime()
{
	if (run_status == AUDIO_IDLE || !current_fx || !FxItem::exists(current_fx)) {
		return -1;
	}
	return run_time.elapsed();
}

void AudioSlot::emit_audio_play_progress()
{
	if (!FxItem::exists(current_fx)) return;

	if (!current_fx->audioDuration || run_status != AUDIO_RUNNING) return;

	qint64 soundlen = current_fx->audioDuration;
	int loop = audio_io->currentLoop();
	int per_mille = (run_time.elapsed() - soundlen * loop) * 1000 / soundlen;

	emit audioProgressChanged(slotNumber, current_fx, per_mille);

	AudioCtrlMsg msg(current_fx,slotNumber);
	msg.currentAudioStatus = run_status;
	msg.progress = per_mille;
	msg.loop = loop;
	msg.executer = current_executer;
	if (current_fx->loopTimes > 1) {
		msg.maxloop = current_fx->loopTimes;
	}
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::on_audio_output_status_changed(QAudio::State state)
{
	AudioStatus current_state = run_status;
	if (state == QAudio::StoppedState && !audio_io->isDecodingFinished()) {
		DEBUGERROR("Audio is in Stopped State -> This might be an buffer underrun for an audio channel");
	}

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
		break;
	}

	if (current_state != run_status) {
		// qDebug("emit status: %d %d %p",run_status, slotNumber, current_fx);
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,run_status,current_executer);
		msg.fxAudio = current_fx;
		emit audioCtrlMsgEmitted(msg);

		if (run_status == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0,0);
		}
	}
}

void AudioSlot::on_audio_io_read_ready()
{
	// qDebug("Audio io ready read");
	audio_output->stop();
	audio_io->stop();
}

void AudioSlot::on_vulevel_changed(int left, int right)
{
	emit vuLevelChanged(slotNumber, left, right);
	emit_audio_play_progress();
}

void AudioSlot::on_fade_out_frame_changed(qreal value)
{
	// calculate new volume from timeline value
	qreal new_volume = fade_out_initial_vol;
	new_volume -= value * fade_out_initial_vol;
	// some rounding before cast to interger
	new_volume += 0.5f;

	// set volume in audio output
	setVolume(new_volume);

	// send message in order to update GUI
	AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status,current_executer);
	msg.volume = new_volume;
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::on_fade_out_finished()
{
	stopFxAudio();

	LOGTEXT(tr("Fade out finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(current_fx->name()));
}

void AudioSlot::on_volset_timer_finished()
{
	LOGTEXT(volset_text);
}

void AudioSlot::audioCtrlReceiver(AudioCtrlMsg msg)
{
	// Test if Message is for me
	if (msg.slotNumber != slotNumber) return;

	switch (msg.ctrlCmd) {
	case CMD_AUDIO_STOP:
		stopFxAudio();
		break;
	case CMD_AUDIO_START:
		if (FxItem::exists(msg.fxAudio)) {
			if (debug > 2) DEBUGTEXT("%s: received: startAudio on channel :%d"
									 ,Q_FUNC_INFO,msg.slotNumber+1);
			startFxAudio(msg.fxAudio,msg.executer);
		} else {
			DEBUGERROR("%s: Audio Fx Start: FxAudioItem is not in global FX list",Q_FUNC_INFO);
		}
		break;
	case CMD_AUDIO_CHANGE_VOL:
		setVolume(msg.volume);
		break;
	case CMD_AUDIO_FADEOUT:
		fadeoutFxAudio(msg.fadetime);
		break;
	default:
		DEBUGERROR("%s: Unsupported command received: %d",Q_FUNC_INFO,msg.ctrlCmd);
		break;
	}

}

void AudioSlot::setAudioDurationMs(qint64 ms)
{
	if (!FxItem::exists(current_fx)) return;

	current_fx->audioDuration = ms;
}

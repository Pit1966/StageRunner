#include "audiochannel.h"
#include "audioformat.h"

#include "fxaudioitem.h"
#include "fxplaylistitem.h"
#include "system/log.h"
#include "config.h"
#include "audioiodevice.h"
#include "audiocontrol.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/usersettings.h"
#include "executer.h"
#include "execcenter.h"
#include "fxlist.h"
#include "audioplayer.h"

#include <QTime>
#include <QApplication>
#ifdef IS_QT5
#include <QMediaContent>
#include <QMediaPlayer>
#endif

AudioSlot::AudioSlot(AudioControl *parent, int pSlotNumber)
	: QObject()
{
	audio_ctrl = parent;

	run_status = AUDIO_IDLE;
	slotNumber = pSlotNumber;
	fade_initial_vol = 0;
	fade_target_vol = 0;
	current_fx = 0;
	current_executer = 0;
	master_volume = MAX_VOLUME;
	is_experimental_audio_f = false;

	// Vol Set Logging
	volset_timer.setSingleShot(true);
	volset_timer.setInterval(500);
	connect(&volset_timer,SIGNAL(timeout()),this,SLOT(on_volset_timer_finished()));

	audio_io = new AudioIODevice(AudioFormat::defaultFormat());
	audio_output = new QAudioOutput(AudioFormat::defaultFormat(),this);
	audio_player = new AudioPlayer(*this);
	audio_player->setVolume(100);
	if (parent->myApp.userSettings->pAudioBufferSize > 90) {
		audio_output->setBufferSize(parent->myApp.userSettings->pAudioBufferSize);
	}

	connect(audio_output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(on_audio_output_status_changed(QAudio::State)));
	connect(audio_io,SIGNAL(readReady()),this,SLOT(on_audio_io_read_ready()),Qt::QueuedConnection);
	connect(audio_io,SIGNAL(vuLevelChanged(int,int)),this,SLOT(on_vulevel_changed(int,int)),Qt::QueuedConnection);
	connect(audio_io,SIGNAL(audioDurationDetected(qint64)),this,SLOT(setAudioDurationMs(qint64)));

	//Fadeout Timeline
	connect(&fade_timeline,SIGNAL(valueChanged(qreal)),this,SLOT(on_fade_frame_changed(qreal)));
	connect(&fade_timeline,SIGNAL(finished()),this,SLOT(on_fade_finished()));

	// QMediaPlayer (experimental audio) connects
	connect(audio_player,SIGNAL(statusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)));
	connect(audio_player,SIGNAL(durationChanged(qint64)),this,SLOT(setAudioDurationMs(qint64)));
	connect(audio_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_media_playstate_changed(QMediaPlayer::State)));
}

AudioSlot::~AudioSlot()
{
	if (run_status != AUDIO_IDLE) {
		stopFxAudio();
	}

	delete audio_player;
	delete audio_output;
	delete audio_io;
}

/**
 * @brief Select Audio Slot (prepare for audio start)
 * @return true, if Audio status was IDLE when selecting
 *
 *  This function sets the audio slot to AUDIO_INIT status. This is for preparing
 *  audio start. The slot is now occupied.
 */
bool AudioSlot::select()
{
	if (run_status <= AUDIO_IDLE) {
		run_status = AUDIO_INIT;
		return true;
	}
	return false;
}

void AudioSlot::unselect()
{
	run_status = AUDIO_IDLE;
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 startPosMs, int initVol)
{
	if (AppCentral::instance()->isExperimentalAudio()) {
		is_experimental_audio_f = true;
		if (!audio_player) return false;
	} else {
		is_experimental_audio_f = false;
		if (!audio_output) return false;
	}


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
	run_status = AUDIO_BUFFERING;
	run_time.start();

	// Find out what the initial volume for audio is
	qint32 targetVolume = fxa->initialVolume;
	if (initVol >= 0) {
		targetVolume = initVol;
	}
	else if (fxa->parentFxList() && fxa->parentFxList()->parentFx()) {
		FxItem *fx = fxa->parentFxList()->parentFx();
		if (FxItem::exists(fx) && fx->fxType() == FX_AUDIO_PLAYLIST) {
			targetVolume = reinterpret_cast<FxPlayListItem*>(fx)->initialVolume;
			LOGTEXT(tr("Set initial volume for audio playlist member to %1").arg(targetVolume));
		}
	}

	// set initial Volume
	if (fxa->fadeInTime() > 0) {
		setVolume(0);
		fadeinFxAudio(targetVolume,fxa->fadeInTime());
	}
	else if (startPosMs != 0 && initVol == -1) {
		setVolume(0);
		fadeinFxAudio(targetVolume,200);
	}
	else {
		setVolume(targetVolume);
	}

	// Set Filename of audio file
	if (is_experimental_audio_f) {
		audio_player->setSourceFilename(fxa->filePath());
	} else {
		audio_io->setSourceFilename(fxa->filePath());
		// Start decoding of audio file to default (pcm 48000, 2Ch, 16bit) format
		audio_io->start(fxa->loopTimes);
	}


	// Set Audio Buffer Size
	if (audio_ctrl->myApp.userSettings->pAudioBufferSize > 90) {
		if (!is_experimental_audio_f) {
			audio_output->setBufferSize(audio_ctrl->myApp.userSettings->pAudioBufferSize);
		}
	}

	if (startPosMs < 0) {
		// We get the start play position in ms from the last played seek position in the FxAudio instance
		if (is_experimental_audio_f) {
			if (fxa->seekPosition() == 0) {
				audio_player->seekPlayPosMs(0);
			}
			else if (fxa->seekPosition() > 0) {
				audio_player->seekPlayPosMs(fxa->seekPosition());
			}
		} else {
			audio_io->seekPlayPosMs(fxa->seekPosition());
		}
		fxa->setSeekPosition(0);
	} else {
		if (is_experimental_audio_f) {
			if (startPosMs == 0) {
				audio_player->seekPlayPosMs(0);
			}
			else if (startPosMs > 0)
				audio_player->seekPlayPosMs(startPosMs);
		} else {
			audio_io->seekPlayPosMs(startPosMs);
		}
	}

	// Feed audio device with decoded data
	if (is_experimental_audio_f) {
		audio_player->start(fxa->loopTimes);
	} else {
		audio_output->start(audio_io);
	}

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = current_volume;
	msg.executer = exec;
	emit audioCtrlMsgEmitted(msg);

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			current_fx->startInProgress = false;
			float vol = 0;
			if (is_experimental_audio_f) {
				vol = audio_player->volume();
			} else {
				vol = audio_output->volume();
			}
			/*if (debug > 1) */DEBUGTEXT("FxAudio started: '%s'' (%dms delayed, vol: %f)"
									 ,fxa->name().toLocal8Bit().data(),run_time.elapsed(),vol);
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
	if (fade_timeline.state() == QTimeLine::Running) {
		fade_timeline.stop();
	}

	if (current_fx)
		current_fx->startInProgress = false;

	emit vuLevelChanged(slotNumber,0,0);

	if (run_status > AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		if (is_experimental_audio_f) {
			audio_player->stop();
		} else {
			audio_io->stop();
			audio_output->stop();
		}

		return true;
	} else {
		if (audio_io->isOpen()) {
			LOGERROR(tr("Stop Audio playing in slot %1: But audio channel is idle").arg(slotNumber+1));
			if (is_experimental_audio_f) {
				audio_player->stop();
			} else {
				audio_output->stop();
				audio_io->stop();
			}
		}
		return false;
	}

	return false;
}

bool AudioSlot::fadeoutFxAudio(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("Fade out time = %d",time_ms);
		return false;
	}
	// Fadeout only possible if audio is running
	if (is_experimental_audio_f) {
		if (audio_player->state() != QMediaPlayer::PlayingState) return false;
	} else {
		if (audio_output->state() != QAudio::ActiveState) return false;
	}

	if (!FxItem::exists(current_fx)) return false;

	fade_mode = AUDIO_FADE_OUT;

	// Set Fadeout time
	fade_initial_vol = current_volume;
	fade_target_vol = targetVolume;
	fade_timeline.setDuration(time_ms);

	// and start the time line ticker
	fade_timeline.start();
	LOGTEXT(tr("Fade out for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(current_fx->name()).arg(time_ms));

	return true;
}

bool AudioSlot::fadeinFxAudio(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("Fade in time = %d",time_ms);
		return false;
	}
	// Fadeout only possible if audio is running
	// if (audio_output->state() != QAudio::ActiveState) return false;

	if (!FxItem::exists(current_fx)) return false;

	fade_mode = AUDIO_FADE_IN;

	// Set Fadein time
	fade_initial_vol = current_volume;
	fade_target_vol = targetVolume;
	fade_timeline.setDuration(time_ms);

	// and start the time line ticker
	fade_timeline.start();
	LOGTEXT(tr("Fade in for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(current_fx->name()).arg(time_ms));



	return true;
}

void AudioSlot::setVolume(int vol)
{

	float level;

	if (is_experimental_audio_f) {
		level = float(vol) * 100 / MAX_VOLUME;
		if (master_volume >= 0) {
			level *= (float)master_volume / MAX_VOLUME;
		}
	} else {
		level = float(vol) / MAX_VOLUME;
		if (master_volume >= 0) {
			level *= (float)master_volume / MAX_VOLUME;
		}
	}

#ifdef IS_QT5
	if (is_experimental_audio_f) {
		if (!audio_player) return;
		audio_player->setVolume(int(level));
	} else {
		if (!audio_output) return;
		audio_output->setVolume(level);
	}
#endif

	current_volume = vol;

	volset_text = tr("Change Volume for Audio Fx in slot %1: %2 ").arg(slotNumber+1).arg(vol);
	volset_timer.start();
}

void AudioSlot::setMasterVolume(int vol)
{
	master_volume = vol;
	if (run_status > AUDIO_IDLE) {
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
int AudioSlot::currentRunTime() const
{
	if (run_status <= AUDIO_IDLE || !current_fx || !FxItem::exists(current_fx)) {
		return -1;
	}
	return run_time.elapsed();
}

void AudioSlot::emit_audio_play_progress()
{
	if (!FxItem::exists(current_fx)) return;

	if (!current_fx->audioDuration || run_status != AUDIO_RUNNING) return;

	qint64 soundlen = current_fx->audioDuration;

	int loop;
	int per_mille;
	qint64 time_ms;

	if (is_experimental_audio_f) {
		loop = audio_player->currentLoop();
		time_ms = audio_player->currentPlayPosMs();
		per_mille = time_ms * 1000 / soundlen;

	} else {
		loop = audio_io->currentLoop();
		time_ms = audio_io->currentPlayPosMs();
		per_mille = time_ms * 1000 / soundlen;
	}

	emit audioProgressChanged(slotNumber, current_fx, per_mille);

	AudioCtrlMsg msg(current_fx,slotNumber);
	msg.currentAudioStatus = run_status;
	msg.progress = per_mille;
	msg.progressTime = time_ms;
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
		DEBUGERROR("Audio is in Stopped State while decoding -> This might be an buffer underrun for an audio channel");
	}

	switch (state) {
	case QAudio::ActiveState:
		run_status = AUDIO_RUNNING;
		if (current_fx)
			current_fx->startInProgress = false;
		break;
	case QAudio::IdleState:
		run_status = AUDIO_IDLE;
		break;
	case QAudio::SuspendedState:
	case QAudio::StoppedState:
		run_status = AUDIO_IDLE;
		break;
	}

	if (audio_io->audioError() != AUDIO_ERR_NONE) {
		run_status = AUDIO_ERROR;
	}

	if (current_state != run_status) {
		// qDebug("emit status: %d %d %p",run_status, slotNumber, current_fx);
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,run_status,current_executer);
		msg.fxAudio = current_fx;

		if (run_status == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0,0);
			emit audioProgressChanged(slotNumber,current_fx,0);
			msg.progress = 0;
		}
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::on_audio_io_read_ready()
{
	// qDebug("Audio io ready read");
	audio_output->stop();
	audio_io->stop();
}

void AudioSlot::on_media_status_changed(QMediaPlayer::MediaStatus status)
{
	AudioStatus cur_status = run_status;

	if (debug > 1) qDebug("Channel %d: Media Status changed: %d",slotNumber+1,status);

	switch (status) {
	case QMediaPlayer::UnknownMediaStatus:
	case QMediaPlayer::NoMedia:
		run_status = AUDIO_ERROR;
		break;
	case QMediaPlayer::LoadingMedia:
		run_status = AUDIO_INIT;
		break;
	case QMediaPlayer::LoadedMedia:
		run_status = AUDIO_IDLE;
		break;
	case QMediaPlayer::StalledMedia:
		run_status = AUDIO_ERROR;
		DEBUGERROR("Audio has stalled on channel %1. Buffer underrun?",slotNumber+1);
		break;
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		run_status = AUDIO_RUNNING;
		if (current_fx)
			current_fx->startInProgress = false;
		break;
	case QMediaPlayer::EndOfMedia:
		run_status = AUDIO_IDLE;
		break;
	case QMediaPlayer::InvalidMedia:
		run_status = AUDIO_ERROR;
		break;
	}

	if (cur_status != run_status) {
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,run_status,current_executer);
		msg.fxAudio = current_fx;

		if (run_status == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0,0);
			emit audioProgressChanged(slotNumber,current_fx,0);
			msg.progress = 0;
		}
		emit audioCtrlMsgEmitted(msg);

	}
}

void AudioSlot::on_media_playstate_changed(QMediaPlayer::State state)
{
	AudioStatus cur_status = run_status;

	if (state == QMediaPlayer::PausedState) {
		run_status = AUDIO_IDLE;
	}
	else if (state == QMediaPlayer::PlayingState) {
		run_status = AUDIO_RUNNING;
		if (current_fx)
			current_fx->startInProgress = false;
	}
	else if (state == QMediaPlayer::StoppedState) {
		// run_status = AUDIO_IDLE;
	}

	if (cur_status != run_status) {
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,run_status,current_executer);
		msg.fxAudio = current_fx;

		if (run_status == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0,0);
			emit audioProgressChanged(slotNumber,current_fx,0);
			msg.progress = 0;
		}
		emit audioCtrlMsgEmitted(msg);

	}
}

void AudioSlot::on_vulevel_changed(int left, int right)
{
	emit vuLevelChanged(slotNumber, left, right);
	emit_audio_play_progress();

	// Investigate if audio must be faded out or ended
	if (current_fx) {
		bool stopaudio = false;
		if (current_fx->stopAtSeekPos > 0 && currentPlayPosMs() >= current_fx->stopAtSeekPos)
				stopaudio = true;
		if (current_fx->holdTime() > 0 && run_time.elapsed() >= current_fx->fadeInTime() + current_fx->holdTime())
				stopaudio = true;

		if (stopaudio) {
			if (current_fx->fadeOutTime() > 0) {
				if (fade_timeline.state() != QTimeLine::Running)
					fadeoutFxAudio(0,current_fx->fadeOutTime());
			} else {
				stopFxAudio();
			}
		}
	}
}

void AudioSlot::on_fade_frame_changed(qreal value)
{
	// calculate new volume from timeline value
	qreal new_volume;
	if (fade_mode == AUDIO_FADE_OUT) {
		new_volume = fade_initial_vol;
		new_volume -= value * qAbs(fade_initial_vol - fade_target_vol);
		// some rounding before cast to integer
		new_volume += 0.5f;
	}
	else if (fade_mode == AUDIO_FADE_IN) {
		new_volume = fade_initial_vol;
		new_volume += value * qAbs(fade_target_vol - fade_initial_vol);
		// some rounding before cast to integer
		new_volume += 0.5f;
	}
	else {
		return;
	}

	// set volume in audio output
	setVolume(new_volume);

	// send message in order to update GUI
	AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status,current_executer);
	msg.volume = new_volume;
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::on_fade_finished()
{
	if (fade_mode == AUDIO_FADE_OUT) {
		stopFxAudio();

		LOGTEXT(tr("Audio fade out finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(current_fx->name()));
	}
	else if (fade_mode == AUDIO_FADE_IN) {
		LOGTEXT(tr("Audio fade in finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(current_fx->name()));

	}
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
			startFxAudio(msg.fxAudio,msg.executer,0,msg.volume);
		} else {
			DEBUGERROR("%s: Audio Fx Start: FxAudioItem is not in global FX list",Q_FUNC_INFO);
		}
		break;
	case CMD_AUDIO_START_AT:
		if (FxItem::exists(msg.fxAudio)) {
			if (debug > 2) DEBUGTEXT("%s: received: startAudio on channel %d AT LAST POSITION"
									 ,Q_FUNC_INFO,msg.slotNumber+1);
			startFxAudio(msg.fxAudio,msg.executer,-1,msg.volume);
		} else {
			DEBUGERROR("%s: Audio Fx Start: FxAudioItem is not in global FX list",Q_FUNC_INFO);
		}
		break;
	case CMD_AUDIO_CHANGE_VOL:
		setVolume(msg.volume);
		break;
	case CMD_AUDIO_FADEOUT:
		fadeoutFxAudio(0,msg.fadetime);
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

bool AudioSlot::seekPosMs(qint64 ms)
{
	bool seek = false;
	if (current_fx && run_status == AUDIO_RUNNING) {
		if (is_experimental_audio_f) {
			seek = audio_player->seekPlayPosMs(ms);
		} else {
			seek = audio_io->seekPlayPosMs(ms);
		}
		volset_text = tr("Audio: '%1' seek to %2ms").arg(current_fx->name()).arg(ms);
		if (!volset_timer.isActive())
			volset_timer.start();
	}
	return seek;
}

bool AudioSlot::seekPosPerMille(int perMille)
{
	bool seek = false;
	if (current_fx && run_status == AUDIO_RUNNING) {
		qint64 seekpos = current_fx->audioDuration * perMille / 1000;
		if (is_experimental_audio_f) {
			seek = audio_player->seekPlayPosMs(seekpos);
		} else {
			seek = audio_io->seekPlayPosMs(seekpos);
		}
		volset_text = tr("Audio: '%1' seek to %2ms (%3pMille)")
				.arg(current_fx->name()).arg(seekpos).arg(perMille);
		if (!volset_timer.isActive())
			volset_timer.start();
	}
	return seek;
}

/**
 * @brief Get the current play position of audio stream
 * @return play position (ms) or -1 if audio channel is idle
 */
qint64 AudioSlot::currentPlayPosMs() const
{
	if (run_status != AUDIO_RUNNING && run_status != AUDIO_PAUSED)
		return -1;
	if (is_experimental_audio_f) {
		return audio_player->currentPlayPosMs();
	} else {
		return audio_io->currentPlayPosMs();
	}
}

void AudioSlot::storeCurrentSeekPos()
{
	if (run_status == AUDIO_RUNNING && FxItem::exists(current_fx)) {
		current_fx->setSeekPosition(is_experimental_audio_f?audio_player->currentPlayPosMs():audio_io->currentPlayPosMs());
	}
}

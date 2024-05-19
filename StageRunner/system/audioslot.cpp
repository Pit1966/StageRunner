//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "audioslot.h"
#include "audioformat.h"

#include "system/audiooutput/mediaplayeraudiobackend.h"
#include "system/audiooutput/iodeviceaudiobackend.h"

#ifdef USE_SDL
#  include "system/audiooutput/sdl2audiobackend.h"
#endif

#include "fxaudioitem.h"
#include "fxclipitem.h"
#include "fxplaylistitem.h"
#include "system/log.h"
#include "config.h"
#include "audioiodevice.h"
#include "audiocontrol.h"
#include "videocontrol.h"
#include "appcentral.h"
#include "usersettings.h"
#include "executer.h"
#include "execcenter.h"
#include "fxlist.h"
#include "audioplayer.h"
#include "qtstatictools.h"

#include <QApplication>


AudioSlot::AudioSlot(AudioControl *parent, int pSlotNumber, AudioOutputType audioEngineType, const QString &devName)
	: QObject()
	, slotNumber(pSlotNumber)
	, audio_ctrl(parent)
	, audio_player(nullptr)
	, run_status(AUDIO_IDLE)
	, current_fx(nullptr)
	, current_executer(nullptr)
	, fade_mode(AUDIO_FADE_IDLE)
	, fade_initial_vol(0)
	, fade_target_vol(0)
	, current_volume(0)
	, master_volume(MAX_VOLUME)
	, m_isFFTEnabled(false)
	, m_dmxVolLockState(DMX_AUDIO_NOT_LOCKED)
	, m_lastAudioError(AUDIO_ERR_NONE)
{
//	qDebug() << "init AudioSlot" << pSlotNumber;

	// Vol Set Logging
	volset_timer.setSingleShot(true);
	volset_timer.setInterval(500);
	connect(&volset_timer,SIGNAL(timeout()),this,SLOT(on_volset_timer_finished()));


	if (audioEngineType == OUT_SDL2) {
#ifdef USE_SDL
		audio_player = new SDL2AudioBackend(*this);
#else
		qWarning("Configured SDL output is not compiled in!");
#endif
	}
	else if (audioEngineType == OUT_DEVICE) {
		audio_player = new IODeviceAudioBackend(*this, devName);
	}
	else if (audioEngineType == OUT_MEDIAPLAYER) {
		audio_player = new MediaPlayerAudioBackend(*this);
		audio_player->setVolume(MAX_VOLUME,MAX_VOLUME);
	}
	else {
		// no audio -> create a mediaplayer object
		audio_player = new MediaPlayerAudioBackend(*this);
		audio_player->setVolume(MAX_VOLUME,MAX_VOLUME);
	}

	if (audio_player->audioError()) {
		m_lastAudioError = audio_player->audioError();
		m_lastErrorText = audio_player->audioErrorString();
	}


	if (m_lastAudioError == AUDIO_ERR_NONE || m_lastAudioError == AUDIO_ERR_DECODER ) {
	}

	if (parent->myApp.userSettings->pAudioBufferSize >= 16384) {
		audio_player->setAudioBufferSize(parent->myApp.userSettings->pAudioBufferSize);
	}

	//Fadeout Timeline
	connect(&fade_timeline,SIGNAL(valueChanged(qreal)),this,SLOT(on_fade_frame_changed(qreal)));
	connect(&fade_timeline,SIGNAL(finished()),this,SLOT(on_fade_finished()));

	// General audio interface connects
	if (audio_player) {
		connect(audio_player,SIGNAL(statusChanged(AUDIO::AudioStatus)),this,SLOT(onPlayerStatusChanged(AUDIO::AudioStatus)));
		connect(audio_player,SIGNAL(mediaDurationChanged(qint64)),this,SLOT(setAudioDurationMs(qint64)));
		connect(audio_player,SIGNAL(vuLevelChanged(qreal,qreal)),this,SLOT(on_vulevel_changed(qreal,qreal)),Qt::QueuedConnection);
		connect(audio_player,SIGNAL(frqSpectrumChanged(FrqSpectrum*)),this,SLOT(on_frqSpectrum_changed(FrqSpectrum*)));
	}
}

AudioSlot::~AudioSlot()
{
	if (run_status != AUDIO_IDLE) {
		if (run_status < VIDEO_INIT)
			stopFxAudio();
	}

	delete audio_player;
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

bool AudioSlot::startFxAudio(const AudioCtrlMsg &msg)
{
	int startPosMs = 0;
	if (msg.ctrlCmd == CMD_AUDIO_START_AT)
		startPosMs = -1;

	bool ok = startFxAudio(msg.fxAudio, msg.executer, startPosMs, msg.volume, msg.fadetime);

	if (msg.isDmxVolumeLocked)
		setDmxVolLockState(DMX_AUDIO_LOCKED);

	return ok;
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 startPosMs, int initVol, int fadeInMs)
{
	if (!audio_player) {
		run_status = AUDIO_ERROR;
		return -1;
	}

	current_fx = fxa;
	current_executer = exec;
	run_status = AUDIO_BUFFERING;
	run_time.start();

	//
	qint64 target_pos_ms = startPosMs;

	// Find out what the initial volume for audio is
	qint32 targetVolume = fxa->initialVolume;
	if (initVol >= 0) {
		targetVolume = initVol;
	}
	else if (fxa->parentFxList() && fxa->parentFxList()->parentFx()) {
		FxItem *fx = fxa->parentFxList()->parentFx();
		if (FxItem::exists(fx) && fx->fxType() == FX_AUDIO_PLAYLIST) {
			FxPlayListItem *play = reinterpret_cast<FxPlayListItem*>(fx);
			targetVolume = play->initialVolume;
			LOGTEXT(tr("Set initial volume for audio playlist member to %1").arg(targetVolume));
		}
	}

	// Set Filename of audio file
	if (!audio_player->setSourceFilename(fxa->filePath(), fxa->name())) {
		QString msg = tr("FX '%1': Could not start FX audio with file '%2'")
				.arg(fxa->fxNamePath(), fxa->filePath());
		POPUPERRORMSG(__func__, msg);
		return false;
	}

	// Set Audio Buffer Size
	if (audio_ctrl->myApp.userSettings->pAudioBufferSize  >= 16384) {
		audio_player->setAudioBufferSize(audio_ctrl->myApp.userSettings->pAudioBufferSize);
		LOGTEXT(tr("Set audio buffer size to: %1").arg(audio_ctrl->myApp.userSettings->pAudioBufferSize));
	}

	if (startPosMs < 0) {
		// We get the start play position in ms from the last played seek position in the FxAudio instance
		if (fxa->seekPosition() == 0) {
			target_pos_ms = 0;
			audio_player->seekPlayPosMs(0);
		}
		else if (fxa->seekPosition() > 0) {
			target_pos_ms = fxa->seekPosition();
			audio_player->seekPlayPosMs(fxa->seekPosition());
		}
		// Reset the seek position to zero
		fxa->setSeekPosition(0);
	} else {
		audio_player->seekPlayPosMs(startPosMs);
	}

	// set initial Volume
	if (fadeInMs < 0)
		fadeInMs = fxa->fadeInTime();
	if (fadeInMs > 0) {
		setVolume(0);
		fadeinFxAudio(targetVolume, fadeInMs);
	}
	else if (startPosMs > 0 && initVol == -1) {
		setVolume(0);
		fadeinFxAudio(targetVolume,200);
	}
	else {
		setVolume(targetVolume);
	}

	// Start playing
	audio_player->setStartDelay(fxa->preDelay());
	audio_player->start(fxa->loopTimes);

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = current_volume;
	msg.executer = exec;
	emit audioCtrlMsgEmitted(msg);

	QString delayMsg = fxa->preDelay() > 0 ? tr(" with <font color=orange>%1ms delay</font>").arg(fxa->preDelay()) : QString();

	if (exec && AppCentral::instance()->execCenter->useExecuter(exec)) {
		LOGTEXT(tr("<font color=green>Start</font> <b>%1</b> <font color=green>in audio slot %2</font> with Executer: %3 at %4 with volume %5%6")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(exec->getIdString(), QtStaticTools::msToTimeString(target_pos_ms))
				.arg(targetVolume)
				.arg(delayMsg));
	} else {
		exec = nullptr;
		LOGTEXT(tr("<font color=green>Start</font> <b>%1</b> <font color=green>in audio slot %2</font> at time %3 with volume %4%5")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(QtStaticTools::msToTimeString(target_pos_ms))
				.arg(targetVolume)
				.arg(delayMsg));
	}

	/// @todo remove this wait loop. Do it with timer or callback ...
	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			current_fx->startInProgress = false;
			current_fx->startSlot = -1;
			qreal vol = audio_player->volume();
			/*if (debug > 1) */DEBUGTEXT("FxAudio started: '%s'' (%dms delayed, vol: %f)"
									 ,fxa->name().toLocal8Bit().data(),run_time.elapsed(),vol);
			ok = true;
			run_time.start();
			current_fx = fxa;
		}
		else if (run_status == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->name().toLocal8Bit().data());
			current_fx = nullptr;
			break;
		}
		else if (run_status == AUDIO_PAUSED) {
			ok = true;
			current_fx = fxa;
			qDebug() << Q_FUNC_INFO << "Audio paused on start";
		}
	}

	if (exec)
		AppCentral::instance()->execCenter->freeExecuter(exec);

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	if (fade_timeline.state() == QTimeLine::Running) {
		fade_timeline.stop();
	}

	if (current_fx)
		current_fx->startInProgress = false;

	emit vuLevelChanged(slotNumber,0.0,0.0);

	if (run_status >= VIDEO_RUNNING) {
		// do not stop video here. Uncomment next 2 lines, if you want to stop video too
		// AudioCtrlMsg msg(slotNumber, CMD_AUDIO_STATUS_CHANGED, AUDIO_IDLE);
		// emit audioCtrlMsgEmitted(msg);
		return true;
	}
	else if (run_status > AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		audio_player->stop();
		return true;
	}
	else {
		AudioStatus s = audio_player->state();
		if (s == AUDIO_PAUSED || s == AUDIO_NO_STATE) {
			return true;
		}
		else if (s != AUDIO_IDLE) {
			LOGERROR(tr("Stop Audio playing in slot %1: But audio channel is idle").arg(slotNumber+1));
			audio_player->stop();
		}
		return false;
	}
}

bool AudioSlot::pauseFxAudio(bool state)
{
	if (state) {
		if (run_status != AUDIO_RUNNING)
			return false;

		audio_player->pause(true);
		run_status = AUDIO_PAUSED;
	} else {
		if (run_status != AUDIO_PAUSED)
			return false;

		audio_player->pause(false);
	}

	return true;
}

bool AudioSlot::fadeoutFxAudio(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("AudioSlot::fadeoutFxAudio Fade out time = %d",time_ms);
		return false;
	}

	if (audio_player->state() != AUDIO_RUNNING)
		return false;

	if (!FxItem::exists(current_fx))
		return false;

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

/**
 * @brief AudioSlot::fadeinFxAudio
 * @param targetVolume
 * @param time_ms
 * @return
 *
 * @note must be called from main thread !!
 */
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

/**
 * @brief AudioSlot::setVolume
 * @param vol
 *
 * @note this is called, when software changes the volume, but not when Volume slider is moved manually
 */
void AudioSlot::setVolume(int vol)
{
	m_dmxVolLockState = DMX_AUDIO_NOT_LOCKED;
	_setVolume(vol);
}

bool AudioSlot::setVolumeFromDMX(int dmxvol)
{
//	qDebug() << "set dmx vol state" << m_dmxVolLockState << dmxvol;

	switch (m_dmxVolLockState) {
	case DMX_AUDIO_LOCKED:
		_setVolume(dmxvol);
		break;

	case DMX_AUDIO_INC_SEARCH:
		if (dmxvol >= current_volume) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
			_setVolume(dmxvol);
		}
		break;

	case DMX_AUDIO_DEC_SEARCH:
		if (dmxvol <= current_volume) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
			_setVolume(dmxvol);
		}
		break;

	case DMX_AUDIO_NOT_LOCKED:
		if (qAbs(dmxvol - current_volume) < 2) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
		}
		else if (dmxvol < current_volume) {
			m_dmxVolLockState = DMX_AUDIO_INC_SEARCH;
		}
		else if (dmxvol > current_volume) {
			m_dmxVolLockState = DMX_AUDIO_DEC_SEARCH;
		}
		break;

	default:
		break;
	}

//	qDebug() << "   set dmx vol state" << m_dmxVolLockState << dmxvol;

	return m_dmxVolLockState == DMX_AUDIO_LOCKED;
}

void AudioSlot::setMasterVolume(int vol)
{
	master_volume = vol;
	if (run_status > AUDIO_IDLE && run_status < VIDEO_INIT) {
		setVolume(current_volume);
	}
}

FxAudioItem *AudioSlot::currentFxAudio()
{
	if (!FxItem::exists(current_fx)) {
		return nullptr;
	}
	return current_fx;
}

Executer *AudioSlot::currentExecuter()
{
	if (current_fx) {
		return current_executer;
	} else {
		return nullptr;
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
	qint64 per_mille;
	qint64 time_ms;

	loop = audio_player->currentLoop();
	time_ms = audio_player->currentPlayPosMs();
	per_mille = time_ms * 1000 / soundlen;

	emit audioProgressChanged(slotNumber, current_fx, int(per_mille));

	AudioCtrlMsg msg(current_fx,slotNumber);
	msg.currentAudioStatus = run_status;
	msg.progress = int(per_mille);
	msg.progressTime = int(time_ms);
	msg.loop = loop;
	msg.executer = current_executer;
	if (current_fx->loopTimes > 1) {
		msg.maxloop = current_fx->loopTimes;
	}
	emit audioCtrlMsgEmitted(msg);
}


void AudioSlot::onPlayerStatusChanged(AudioStatus status)
{
	AudioStatus cur_status = run_status;
	if (status != cur_status) {
		// qDebug() << "onPlayerStatusChanged" << status << cur_status << audio_player->currentAudioCmd();

		switch (status) {
		case AUDIO_MEDIA_STALLED:
			DEBUGERROR("Audio has stalled on channel %1. Buffer underrun?",slotNumber+1);
			run_status = AUDIO_ERROR;
			break;
		case AUDIO_RUNNING:
			if (current_fx) {
				current_fx->startInProgress = false;
				current_fx->startSlot = -1;
			}
			LOGTEXT(tr("Slot %1: Audio buffer size used: %2").arg(slotNumber+1).arg(audio_player->audioBufferSize()));
			//fall through
		default:
			run_status = status;
		}
	}

	if (cur_status != run_status) {
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,run_status,current_executer);
		msg.fxAudio = current_fx;

		if (run_status == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0.0,0.0);
			emit audioProgressChanged(slotNumber,current_fx,0);
			msg.progress = 0;
		}
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioSlot::on_vulevel_changed(qreal left, qreal right)
{
	emit vuLevelChanged(slotNumber, left, right);
	emit_audio_play_progress();

	// Investigate if audio must be faded out or ended
	if (current_fx) {
		if (!FxItem::exists(current_fx)) {
			current_fx = nullptr;
			DEBUGERROR("%s: Fx pointer invalid",Q_FUNC_INFO);
			return;
		}

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

void AudioSlot::on_frqSpectrum_changed(FrqSpectrum *spec)
{
	emit frqSpectrumChanged(slotNumber,spec);
}

void AudioSlot::on_fade_frame_changed(qreal value)
{
	// calculate new volume from timeline value
	qreal new_volume;
	if (fade_mode == AUDIO_FADE_OUT) {
		new_volume = fade_initial_vol;
		new_volume -= value * qAbs(fade_initial_vol - fade_target_vol);
		// some rounding before cast to integer
		new_volume += 0.5;
	}
	else if (fade_mode == AUDIO_FADE_IN) {
		new_volume = fade_initial_vol;
		new_volume += value * qAbs(fade_target_vol - fade_initial_vol);
		// some rounding before cast to integer
		new_volume += 0.5;
	}
	else {
		return;
	}

	// set volume in audio output
	setVolume(new_volume);

	// send message in order to update GUI
	AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,run_status,current_executer);
	msg.volume = int(new_volume);
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::on_fade_finished()
{
	if (fade_mode == AUDIO_FADE_OUT) {
		stopFxAudio();

		LOGTEXT(tr("Audio fade out finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(currentFxName()));
	}
	else if (fade_mode == AUDIO_FADE_IN) {
		LOGTEXT(tr("Audio fade in finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(currentFxName()));

	}
}

void AudioSlot::on_volset_timer_finished()
{
	LOGTEXT(volset_text);
}

void AudioSlot::audioCtrlReceiver(const AUDIO::AudioCtrlMsg &msg)
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
			//startFxAudio(msg.fxAudio,msg.executer,0,msg.volume);
			startFxAudio(msg);
		} else {
			DEBUGERROR("%s: Audio Fx Start: FxAudioItem is not in global FX list",Q_FUNC_INFO);
		}
		break;
	case CMD_AUDIO_START_AT:
		if (FxItem::exists(msg.fxAudio)) {
			if (debug > 2) DEBUGTEXT("%s: received: startAudio on channel %d AT LAST POSITION"
									 ,Q_FUNC_INFO,msg.slotNumber+1);
			//startFxAudio(msg.fxAudio,msg.executer,-1,msg.volume);
			startFxAudio(msg);
		} else {
			DEBUGERROR("%s: Audio Fx Start: FxAudioItem is not in global FX list",Q_FUNC_INFO);
		}
		break;
	case CMD_AUDIO_CHANGE_VOL:
		setVolume(msg.volume);
		break;
	case CMD_AUDIO_FADEIN:
		fadeinFxAudio(msg.volume,msg.fadetime);
		break;
	case CMD_AUDIO_FADEOUT:
		fadeoutFxAudio(0,msg.fadetime);
		break;
	case CMD_AUDIO_PAUSE:
		pauseFxAudio(!(run_status == AUDIO_PAUSED));
		break;
	case CMD_VIDEO_START:
		startFxClipVideoControls(msg.fxAudio, msg.executer);
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
		seek = audio_player->seekPlayPosMs(ms);
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
		seek = audio_player->seekPlayPosMs(seekpos);
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
	return audio_player->currentPlayPosMs();
}

void AudioSlot::storeCurrentSeekPos()
{
	if (run_status == AUDIO_RUNNING && FxItem::exists(current_fx)) {
		current_fx->setSeekPosition(audio_player->currentPlayPosMs());
	}
}


int AudioSlot::audioOutputBufferSize() const
{
	if (audio_player)
		return audio_player->audioBufferSize();

	return 0;
}

void AudioSlot::setFFTEnabled(bool state)
{
	if (state != m_isFFTEnabled) {
		m_isFFTEnabled = state;
		if (audio_player)
			audio_player->setFFTEnabled(state);
	}
}

QString AudioSlot::currentFxName() const
{
	if (FxItem::exists(current_fx)) {
		return current_fx->name();
	} else {
		return tr("No FX was set");
	}
}

void AudioSlot::sdlEmitProgress()
{
	emit_audio_play_progress();
}

/**
 * @brief Prepare audio slot for video control
 * @return true on success (means slot was free)
 */
bool AudioSlot::selectFxClipVideo()
{
	if (run_status <= AUDIO_IDLE) {
		run_status = VIDEO_INIT;
		return true;
	}
	return false;
}

/**
 * @brief Start FxClipItem audio portion.
 * @param fx
 *
 * The FxClip video/audio is not really handled here. This is only to deal with volume and progress.
 */
void AudioSlot::startFxClipVideoControls(FxAudioItem *fx, Executer *exec)
{
	current_fx = fx;			// this is an FxClipItem !!
	run_status = VIDEO_RUNNING;
	current_volume = fx->initialVolume;

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fx,slotNumber, CMD_VIDEO_STATUS_CHANGED, exec);
	msg.volume = current_volume;
	msg.executer = exec;
	msg.currentAudioStatus = VIDEO_RUNNING;
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::setFxClipVideoCtrlStatus(AudioStatus stat)
{
	run_status = stat;
	if (stat == AUDIO_IDLE)
		current_fx = nullptr;
}

bool AudioSlot::fadeoutFxClip(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("AudioSlot::fadeoutFxClip Fade out time = %d",time_ms);
		return false;
	}

	if (run_status != VIDEO_RUNNING)
		return  false;

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

void AudioSlot::_setVolume(int vol)
{
	if (run_status == VIDEO_RUNNING) {
		AppCentral::ref().unitVideo->setVideoVolume(slotNumber, vol);
		return;
	}

	if (!audio_player) return;

	int level = vol;
	if (master_volume >= 0) {
		level = level * master_volume / MAX_VOLUME;
	}

	audio_player->setVolume(level,MAX_VOLUME);

	current_volume = vol;

	volset_text = tr("Change Volume for Audio Fx in slot %1: %2 ").arg(slotNumber+1).arg(vol);

	// it is not allowed to start timers from different thread
//	if (QThread::currentThread() == this->thread())
//		volset_timer.start();
	// this works from every thread
	QMetaObject::invokeMethod(&volset_timer, "start");
}

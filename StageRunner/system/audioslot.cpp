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

#include "system/audiooutput/audioformat.h"
#ifdef IS_QT6
#	include "system/audiooutput/audioiodevice6.h"
#	include "system/audiooutput/iodeviceaudiobackend6.h"
#endif

#ifdef IS_QT5 // QMediaPlayer only exists in Qt5
#	include "system/audiooutput/mediaplayeraudiobackend.h"
#	include "system/audiooutput/audioiodevice.h"
#	include "system/audiooutput/iodeviceaudiobackend.h"
#endif
#include "system/audiooutput/audioplayer.h"

#ifdef USE_SDL
#  include "system/audiooutput/sdl2audiobackend.h"
#endif

#include "fxaudioitem.h"
#include "fxclipitem.h"
#include "fxplaylistitem.h"
#include "system/log.h"
#include "config.h"
#include "audiocontrol.h"
#include "videocontrol.h"
#include "appcentral.h"
#include "usersettings.h"
#include "executer.h"
#include "execcenter.h"
#include "fxlist.h"
#include "qtstatictools.h"

#include <QApplication>


AudioSlot::AudioSlot(AudioControl *parent, int pSlotNumber, AudioOutputType audioEngineType, const QString &devName)
	: QObject(/*parent*/)				// audiothread
	, slotNumber(pSlotNumber)
	, m_audioCtrlUnit(parent)
	, m_audioPlayer(nullptr)
	, m_runStatus(AUDIO_IDLE)
	, m_currentFx(nullptr)
	, m_currentExecuter(nullptr)
	, m_fadeModeAudio(AUDIO_FADE_IDLE)
	, m_fadeVolumeInitial(0)
	, m_fadeVolumeTarget(0)
	, m_currentVolume(0)
	, m_masterVolume(MAX_VOLUME)
	, m_isFFTEnabled(false)
	, m_dmxVolLockState(DMX_AUDIO_NOT_LOCKED)
	, m_lastAudioError(AUDIO_ERR_NONE)
{
//	qDebug() << "init AudioSlot" << pSlotNumber;

	// Vol Set Logging
	m_volumeSetHelpTimer.setSingleShot(true);
	m_volumeSetHelpTimer.setInterval(500);
	connect(&m_volumeSetHelpTimer,SIGNAL(timeout()),this,SLOT(on_volset_timer_finished()));


	if (audioEngineType == OUT_SDL2) {
#ifdef USE_SDL
		m_audioPlayer = new SDL2AudioBackend(*this);
#else
		qWarning("Configured SDL output is not compiled in!");
#endif
	}
	else if (audioEngineType == OUT_DEVICE) {
		m_audioPlayer = new IODeviceAudioBackend(*this, devName);
	}
#ifdef IS_QT5
	else if (audioEngineType == OUT_MEDIAPLAYER) {
		m_audioPlayer = new MediaPlayerAudioBackend(*this);
		m_audioPlayer->setVolume(MAX_VOLUME,MAX_VOLUME);
	}
	else {
		// no audio -> create a mediaplayer object
		m_audioPlayer = new MediaPlayerAudioBackend(*this);
		m_audioPlayer->setVolume(MAX_VOLUME,MAX_VOLUME);
	}
#else
	else {	// default implementation for Qt6
		m_audioPlayer = new IODeviceAudioBackend(*this, devName);
	}
#endif

	if (m_audioPlayer->audioError()) {
		m_lastAudioError = m_audioPlayer->audioError();
		m_lastErrorText = m_audioPlayer->audioErrorString();
	}


	if (m_lastAudioError == AUDIO_ERR_NONE || m_lastAudioError == AUDIO_ERR_DECODER ) {
	}

	if (parent->myApp.userSettings->pAudioBufferSize >= 16384) {
		m_audioPlayer->setAudioBufferSize(parent->myApp.userSettings->pAudioBufferSize);
	}

	//Fadeout Timeline
	connect(&m_fadeHelpTimeLine,SIGNAL(valueChanged(qreal)),this,SLOT(on_fade_frame_changed(qreal)));
	connect(&m_fadeHelpTimeLine,SIGNAL(finished()),this,SLOT(on_fade_finished()));

	// General audio interface connects
	if (m_audioPlayer) {
		connect(m_audioPlayer,SIGNAL(statusChanged(AUDIO::AudioStatus)),this,SLOT(onPlayerStatusChanged(AUDIO::AudioStatus)));
		connect(m_audioPlayer,SIGNAL(mediaDurationChanged(qint64)),this,SLOT(setAudioDurationMs(qint64)));
		connect(m_audioPlayer,SIGNAL(vuLevelChanged(qreal,qreal)),this,SLOT(on_vulevel_changed(qreal,qreal)),Qt::QueuedConnection);
		connect(m_audioPlayer,SIGNAL(frqSpectrumChanged(FrqSpectrum*)),this,SLOT(on_frqSpectrum_changed(FrqSpectrum*)));
	}
}

AudioSlot::~AudioSlot()
{
	if (m_runStatus != AUDIO_IDLE) {
		if (m_runStatus < VIDEO_INIT)
			stopFxAudio();
	}

	delete m_audioPlayer;
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
	if (m_runStatus <= AUDIO_IDLE) {
		m_runStatus = AUDIO_INIT;
		return true;
	}
	return false;
}

void AudioSlot::unselect()
{
	m_runStatus = AUDIO_IDLE;
}

bool AudioSlot::startFxAudio(const AudioCtrlMsg &msg)
{
	int startPosMs = 0;
	if (msg.ctrlCmd == CMD_AUDIO_START_AT)
		startPosMs = -1;

	bool ok = startFxAudio(msg.fxAudio, msg.executer, startPosMs, msg.volume, msg.fadetime, msg.pan);

	if (msg.isDmxVolumeLocked)
		setDmxVolLockState(DMX_AUDIO_LOCKED);

	return ok;
}

bool AudioSlot::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 startPosMs, int initVol, int fadeInMs, int pan)
{
	if (!m_audioPlayer) {
		m_runStatus = AUDIO_ERROR;
		return -1;
	}

	m_currentFx = fxa;
	m_currentExecuter = exec;
	m_runStatus = AUDIO_BUFFERING;
	m_audioRunTime.start();

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
	if (!m_audioPlayer->setSourceFilename(fxa->filePath(), fxa->name())) {
		QString msg = tr("FX '%1': Could not start FX audio with file '%2'")
				.arg(fxa->fxNamePath(), fxa->filePath());
		POPUPERRORMSG(__func__, msg);
		return false;
	}

	// Set Audio Buffer Size
	if (m_audioCtrlUnit->myApp.userSettings->pAudioBufferSize  >= 16384) {
		m_audioPlayer->setAudioBufferSize(m_audioCtrlUnit->myApp.userSettings->pAudioBufferSize);
		LOGTEXT(tr("Set audio buffer size to: %1").arg(m_audioCtrlUnit->myApp.userSettings->pAudioBufferSize));
	}

	if (startPosMs < 0) {
		// We get the start play position in ms from the last played seek position in the FxAudio instance
		if (fxa->seekPosition() == 0) {
			target_pos_ms = 0;
			m_audioPlayer->seekPlayPosMs(0);
		}
		else if (fxa->seekPosition() > 0) {
			target_pos_ms = fxa->seekPosition();
			m_audioPlayer->seekPlayPosMs(fxa->seekPosition());
		}
		// Reset the seek position to zero
		fxa->setSeekPosition(0);
	} else {
		m_audioPlayer->seekPlayPosMs(startPosMs);
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
	// set panning
	setPanning(pan);

	DEVELTEXT("FxAudio prepare start time [ms] %.1f", float(m_audioRunTime.nsecsElapsed() / 1e6));

	// Start playing
	m_audioPlayer->setStartDelay(fxa->preDelay());
	m_audioPlayer->start(fxa->loopTimes);


	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = m_currentVolume;
	msg.pan = pan;
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
	while (0 && m_audioRunTime.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (m_runStatus == AUDIO_RUNNING) {
			m_currentFx->startInProgress = false;
			m_currentFx->startSlot = -1;
			qreal vol = m_audioPlayer->volume();
			/*if (debug > 1) */DEBUGTEXT("FxAudio started: '%s'' (%dms delayed, vol: %f)"
									 ,fxa->name().toLocal8Bit().data(),m_audioRunTime.elapsed(),vol);
			ok = true;
			m_audioRunTime.start();
			m_currentFx = fxa;
		}
		else if (m_runStatus == AUDIO_ERROR) {
			DEBUGERROR("FxAudio Error while starting: '%s'",fxa->name().toLocal8Bit().data());
			m_currentFx = nullptr;
			break;
		}
		else if (m_runStatus == AUDIO_PAUSED) {
			ok = true;
			m_currentFx = fxa;
			qDebug() << Q_FUNC_INFO << "Audio paused on start";
		}
	}

	if (exec)
		AppCentral::instance()->execCenter->freeExecuter(exec);

	return ok;
}

bool AudioSlot::stopFxAudio()
{
	if (m_fadeHelpTimeLine.state() == QTimeLine::Running) {
		m_fadeHelpTimeLine.stop();
	}

	if (m_currentFx)
		m_currentFx->startInProgress = false;

	emit vuLevelChanged(slotNumber,0.0,0.0);

	if (m_runStatus >= VIDEO_RUNNING) {
		// do not stop video here. Uncomment next 2 lines, if you want to stop video too
		// AudioCtrlMsg msg(slotNumber, CMD_AUDIO_STATUS_CHANGED, AUDIO_IDLE);
		// emit audioCtrlMsgEmitted(msg);
		return true;
	}
	else if (m_runStatus > AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		m_audioPlayer->stop();
		return true;
	}
	else {
		AudioStatus s = m_audioPlayer->state();
		if (s == AUDIO_PAUSED || s == AUDIO_NO_STATE) {
			return true;
		}
		else if (s != AUDIO_IDLE) {
			LOGERROR(tr("Stop Audio playing in slot %1: But audio channel is idle").arg(slotNumber+1));
			m_audioPlayer->stop();
		}
		return false;
	}
}

bool AudioSlot::pauseFxAudio(bool state)
{
	if (state) {
		if (m_runStatus != AUDIO_RUNNING)
			return false;

		m_audioPlayer->pause(true);
		m_runStatus = AUDIO_PAUSED;
	} else {
		if (m_runStatus != AUDIO_PAUSED)
			return false;

		m_audioPlayer->pause(false);
	}

	return true;
}

bool AudioSlot::fadeoutFxAudio(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("AudioSlot::fadeoutFxAudio Fade out time = %d",time_ms);
		return false;
	}

	if (m_audioPlayer->state() != AUDIO_RUNNING)
		return false;

	if (!FxItem::exists(m_currentFx))
		return false;

	m_fadeModeAudio = AUDIO_FADE_OUT;

	// Set Fadeout time
	m_fadeVolumeInitial = m_currentVolume;
	m_fadeVolumeTarget = targetVolume;
	m_fadeHelpTimeLine.setDuration(time_ms);

	// and start the time line ticker
	// timers can must be started from thread they belongs to so we use the invoke trick,
	// which will send a signal if we are in another thread instead of calling m_fadeHelpTimeLine.start() directly
	QMetaObject::invokeMethod(&m_fadeHelpTimeLine, "start");
	LOGTEXT(tr("Fade out for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(m_currentFx->name()).arg(time_ms));

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

	if (!FxItem::exists(m_currentFx)) return false;

	m_fadeModeAudio = AUDIO_FADE_IN;

	// Set Fadein time
	m_fadeVolumeInitial = m_currentVolume;
	m_fadeVolumeTarget = targetVolume;
	m_fadeHelpTimeLine.setDuration(time_ms);

	// and start the time line ticker
	m_fadeHelpTimeLine.start();
	LOGTEXT(tr("Fade in for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(m_currentFx->name()).arg(time_ms));

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

/**
 * @brief This is the volume function that is only used by the timeline (envelopes)
 * @param vol
 */
void AudioSlot::setVolumeFromTimeLine(int vol)
{
	m_dmxVolLockState = DMX_AUDIO_NOT_LOCKED;
	if (AppCentral::ref().isLogarithmicVolume()) {
		_setVolume(vol);
	} else {
		qreal linearVolume = QAudio::convertVolume(vol / qreal(MAX_VOLUME),
												   QAudio::LogarithmicVolumeScale,
												   QAudio::LinearVolumeScale);
		_setVolume(qRound(linearVolume * MAX_VOLUME));
	}


	AudioCtrlMsg msg(slotNumber, CMD_STATUS_REPORT, m_runStatus, m_currentExecuter);
	msg.volume = int(vol);
	emit audioCtrlMsgEmitted(msg);
}

bool AudioSlot::setVolumeFromDMX(int dmxvol)
{
//	qDebug() << "set dmx vol state" << m_dmxVolLockState << dmxvol;

	switch (m_dmxVolLockState) {
	case DMX_AUDIO_LOCKED:
		_setVolume(dmxvol);
		break;

	case DMX_AUDIO_INC_SEARCH:
		if (dmxvol >= m_currentVolume) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
			_setVolume(dmxvol);
		}
		break;

	case DMX_AUDIO_DEC_SEARCH:
		if (dmxvol <= m_currentVolume) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
			_setVolume(dmxvol);
		}
		break;

	case DMX_AUDIO_NOT_LOCKED:
		if (qAbs(dmxvol - m_currentVolume) < 2) {
			m_dmxVolLockState = DMX_AUDIO_LOCKED;
		}
		else if (dmxvol < m_currentVolume) {
			m_dmxVolLockState = DMX_AUDIO_INC_SEARCH;
		}
		else if (dmxvol > m_currentVolume) {
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
	m_masterVolume = vol;
	if (m_runStatus > AUDIO_IDLE && m_runStatus < VIDEO_INIT) {
		setVolume(m_currentVolume);
	}
}

void AudioSlot::setPanning(int pan)
{
	_setPanning(pan);
}

FxAudioItem *AudioSlot::currentFxAudio()
{
	if (!FxItem::exists(m_currentFx)) {
		return nullptr;
	}
	return m_currentFx;
}

Executer *AudioSlot::currentExecuter()
{
	if (m_currentFx) {
		return m_currentExecuter;
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
	if (m_runStatus <= AUDIO_IDLE || !m_currentFx || !FxItem::exists(m_currentFx)) {
		return -1;
	}
	return m_audioRunTime.elapsed();
}

void AudioSlot::emit_audio_play_progress()
{
	if (!m_currentFx) return;

	if (!m_currentFx->audioDuration || m_runStatus != AUDIO_RUNNING) return;

	qint64 soundlen = m_currentFx->audioDuration;

	int loop;
	qint64 per_mille;
	qint64 time_ms;

	loop = m_audioPlayer->currentLoop();
	time_ms = m_audioPlayer->currentPlayPosMs();
	per_mille = time_ms * 1000 / soundlen;

	emit audioProgressChanged(slotNumber, m_currentFx, int(per_mille));

	AudioCtrlMsg msg(m_currentFx, slotNumber);
	msg.currentAudioStatus = m_runStatus;
	msg.progress = int(per_mille);
	msg.progressTime = int(time_ms);
	msg.loop = loop;
	msg.executer = m_currentExecuter;
	if (m_currentFx->loopTimes > 1) {
		msg.maxloop = m_currentFx->loopTimes;
	}
	emit audioCtrlMsgEmitted(msg);
}


void AudioSlot::onPlayerStatusChanged(AudioStatus status)
{
	AudioStatus cur_status = m_runStatus;
	if (status != cur_status) {
		// qDebug() << "onPlayerStatusChanged" << status << cur_status << audio_player->currentAudioCmd();

		switch (status) {
		case AUDIO_MEDIA_STALLED:
			DEBUGERROR("Audio has stalled on channel %1. Buffer underrun?",slotNumber+1);
			m_runStatus = AUDIO_ERROR;
			break;
		case AUDIO_RUNNING:
			if (m_currentFx) {
				m_currentFx->startInProgress = false;
				m_currentFx->startSlot = -1;
			}
			DEVELTEXT("Audio running after [ms] %.1f", float(m_audioRunTime.nsecsElapsed()/1000)/1000);
			LOGTEXT(tr("Slot %1: Audio buffer size used: %2").arg(slotNumber+1).arg(m_audioPlayer->audioBufferSize()));
			//fall through
		default:
			m_runStatus = status;
		}
	}

	if (cur_status != m_runStatus) {
		AudioCtrlMsg msg(slotNumber,CMD_AUDIO_STATUS_CHANGED,m_runStatus,m_currentExecuter);
		msg.fxAudio = m_currentFx;

		if (m_runStatus == AUDIO_IDLE) {
			emit vuLevelChanged(slotNumber,0.0,0.0);
			emit audioProgressChanged(slotNumber,m_currentFx,0);
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
	if (m_currentFx) {
		if (!FxItem::exists(m_currentFx)) {
			m_currentFx = nullptr;
			DEBUGERROR("%s: Fx pointer invalid",Q_FUNC_INFO);
			return;
		}

		bool stopaudio = false;
		if (m_currentFx->stopAtSeekPos > 0 && currentPlayPosMs() >= m_currentFx->stopAtSeekPos)
				stopaudio = true;
		if (m_currentFx->holdTime() > 0 && m_audioRunTime.elapsed() >= m_currentFx->fadeInTime() + m_currentFx->holdTime())
				stopaudio = true;

		if (stopaudio) {
			if (m_currentFx->fadeOutTime() > 0) {
				if (m_fadeHelpTimeLine.state() != QTimeLine::Running)
					fadeoutFxAudio(0,m_currentFx->fadeOutTime());
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
	if (m_fadeModeAudio == AUDIO_FADE_OUT) {
		new_volume = m_fadeVolumeInitial;
		new_volume -= value * qAbs(m_fadeVolumeInitial - m_fadeVolumeTarget);
		// some rounding before cast to integer
		new_volume += 0.5;
	}
	else if (m_fadeModeAudio == AUDIO_FADE_IN) {
		new_volume = m_fadeVolumeInitial;
		new_volume += value * qAbs(m_fadeVolumeTarget - m_fadeVolumeInitial);
		// some rounding before cast to integer
		new_volume += 0.5;
	}
	else {
		return;
	}

	// set volume in audio output
	setVolume(new_volume);

	// send message in order to update GUI
	AudioCtrlMsg msg(slotNumber,CMD_STATUS_REPORT,m_runStatus,m_currentExecuter);
	msg.volume = int(new_volume);
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::on_fade_finished()
{
	if (m_fadeModeAudio == AUDIO_FADE_OUT) {
		stopFxAudio();

		LOGTEXT(tr("Audio fade out finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(currentFxName()));
	}
	else if (m_fadeModeAudio == AUDIO_FADE_IN) {
		LOGTEXT(tr("Audio fade in finished for slot %1: '%2'")
			.arg(slotNumber+1).arg(currentFxName()));

	}
}

void AudioSlot::on_volset_timer_finished()
{
	LOGTEXT(m_volumeSetMsg);
}

void AudioSlot::audioCtrlReceiver(const AUDIO::AudioCtrlMsg &msg)
{
	// Test if Message is for me
	if (msg.slotNumber != slotNumber)
		return;

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
		pauseFxAudio(!(m_runStatus == AUDIO_PAUSED));
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
	if (!FxItem::exists(m_currentFx)) return;

	m_currentFx->audioDuration = ms;
}

bool AudioSlot::seekPosMs(qint64 ms)
{
	bool seek = false;
	if (m_currentFx && m_runStatus == AUDIO_RUNNING) {
		seek = m_audioPlayer->seekPlayPosMs(ms);
		m_volumeSetMsg = tr("Audio: '%1' seek to %2ms").arg(m_currentFx->name()).arg(ms);
		if (!m_volumeSetHelpTimer.isActive())
			m_volumeSetHelpTimer.start();
	}
	return seek;
}

bool AudioSlot::seekPosPerMille(int perMille)
{
	bool seek = false;
	if (m_currentFx && m_runStatus == AUDIO_RUNNING) {
		qint64 seekpos = m_currentFx->audioDuration * perMille / 1000;
		seek = m_audioPlayer->seekPlayPosMs(seekpos);
		m_volumeSetMsg = tr("Audio: '%1' seek to %2ms (%3pMille)")
				.arg(m_currentFx->name()).arg(seekpos).arg(perMille);
		if (!m_volumeSetHelpTimer.isActive())
			m_volumeSetHelpTimer.start();
	}
	return seek;
}

/**
 * @brief Get the current play position of audio stream
 * @return play position (ms) or -1 if audio channel is idle
 */
qint64 AudioSlot::currentPlayPosMs() const
{
	if (m_runStatus != AUDIO_RUNNING && m_runStatus != AUDIO_PAUSED)
		return -1;
	return m_audioPlayer->currentPlayPosMs();
}

void AudioSlot::storeCurrentSeekPos()
{
	if (m_runStatus == AUDIO_RUNNING && FxItem::exists(m_currentFx)) {
		m_currentFx->setSeekPosition(m_audioPlayer->currentPlayPosMs());
	}
}


int AudioSlot::audioOutputBufferSize() const
{
	if (m_audioPlayer)
		return m_audioPlayer->audioBufferSize();

	return 0;
}

void AudioSlot::setFFTEnabled(bool state)
{
	if (state != m_isFFTEnabled) {
		m_isFFTEnabled = state;
		if (m_audioPlayer)
			m_audioPlayer->setFFTEnabled(state);
	}
}

QString AudioSlot::currentFxName() const
{
	if (FxItem::exists(m_currentFx)) {
		return m_currentFx->name();
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
	if (m_runStatus <= AUDIO_IDLE) {
		m_runStatus = VIDEO_INIT;
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
	m_currentFx = fx;			// this is an FxClipItem !!
	m_runStatus = VIDEO_RUNNING;
	m_currentVolume = fx->initialVolume;

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fx,slotNumber, CMD_VIDEO_STATUS_CHANGED, exec);
	msg.volume = m_currentVolume;
	msg.executer = exec;
	msg.currentAudioStatus = VIDEO_RUNNING;
	emit audioCtrlMsgEmitted(msg);
}

void AudioSlot::setFxClipVideoCtrlStatus(AudioStatus stat)
{
	m_runStatus = stat;
	if (stat == AUDIO_IDLE)
		m_currentFx = nullptr;
}

bool AudioSlot::fadeoutFxClip(int targetVolume, int time_ms)
{
	if (time_ms <= 0) {
		qDebug("AudioSlot::fadeoutFxClip Fade out time = %d",time_ms);
		return false;
	}

	if (m_runStatus != VIDEO_RUNNING)
		return  false;

	if (!FxItem::exists(m_currentFx)) return false;

	m_fadeModeAudio = AUDIO_FADE_OUT;

	// Set Fadeout time
	m_fadeVolumeInitial = m_currentVolume;
	m_fadeVolumeTarget = targetVolume;
	m_fadeHelpTimeLine.setDuration(time_ms);

	// and start the time line ticker
	m_fadeHelpTimeLine.start();
	LOGTEXT(tr("Fade out for slot %1: '%2' started with duration %3ms")
			.arg(slotNumber+1).arg(m_currentFx->name()).arg(time_ms));

	return true;
}

void AudioSlot::_setVolume(int vol)
{
	if (m_runStatus == VIDEO_RUNNING) {
		// for video we always used logarithmic volume
		qreal linearVolume = QAudio::convertVolume(vol / qreal(MAX_VOLUME),
												   QAudio::LogarithmicVolumeScale,
												   QAudio::LinearVolumeScale);
		AppCentral::ref().unitVideo->setVideoVolume(slotNumber, qRound(linearVolume * MAX_VOLUME));
		return;
	}
	else if (!m_audioPlayer) {
		return;
	}

	int level = vol;
	if (m_masterVolume >= 0) {
		level = level * m_masterVolume / MAX_VOLUME;
	}

	bool lg = AppCentral::ref().isLogarithmicVolume();
	if (lg) {
		qreal linearVolume = QAudio::convertVolume(level / qreal(MAX_VOLUME),
												   QAudio::LogarithmicVolumeScale,
												   QAudio::LinearVolumeScale);
		level = qRound(linearVolume * MAX_VOLUME);
		m_volumeSetMsg = tr("Set volume for Audio Fx in slot %1: %2 (level: %3)")
				.arg(slotNumber+1).arg(vol).arg(level);
	}
	else {
		m_volumeSetMsg = tr("Change Volume for Audio Fx in slot %1: %2 ")
				.arg(slotNumber+1).arg(vol);
	}

	m_audioPlayer->setVolume(level,MAX_VOLUME);
	m_currentVolume = vol;


	// it is not allowed to start timers from different thread
	// this works from every thread
	QMetaObject::invokeMethod(&m_volumeSetHelpTimer, "start");
}

void AudioSlot::_setPanning(int pan)
{

	m_audioPlayer->setPanning(pan, MAX_PAN);
}

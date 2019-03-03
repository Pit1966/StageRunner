#include "audioslot.h"
#include "audioformat.h"

#include "system/audiooutput/mediaplayeraudiobackend.h"
#include "system/audiooutput/iodeviceaudiobackend.h"

#include "fxaudioitem.h"
#include "fxplaylistitem.h"
#include "system/log.h"
#include "config.h"
#include "audioiodevice.h"
#include "audiocontrol.h"
#include "appcentral.h"
#include "usersettings.h"
#include "executer.h"
#include "execcenter.h"
#include "fxlist.h"
#include "audioplayer.h"
#include "qtstatictools.h"

#include <QTime>
#include <QApplication>
#ifdef IS_QT5
#include <QMediaContent>
#include <QMediaPlayer>
#endif


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
	, m_isQMediaPlayerAudio(false)
	, m_isSDLAudio(false)
	, m_isFFTEnabled(false)
	, m_lastAudioError(AUDIO_ERR_NONE)
#ifdef USE_SDL
	, m_sdlChunk(nullptr)
#endif
{
//	qDebug() << "init AudioSlot" << pSlotNumber;

	// Vol Set Logging
	volset_timer.setSingleShot(true);
	volset_timer.setInterval(500);
	connect(&volset_timer,SIGNAL(timeout()),this,SLOT(on_volset_timer_finished()));


	if (audioEngineType == OUT_SDL2) {

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

bool AudioSlot::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 startPosMs, int initVol)
{
#ifdef USE_SDL
	if (AppCentral::instance()->userSettings->pUseSDLAudio)
		return sdlStartFxAudio(fxa, exec, startPosMs, initVol);

	m_isSDLAudio = false;
#endif

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
	audio_player->setSourceFilename(fxa->filePath());


	// Set Audio Buffer Size
	if (audio_ctrl->myApp.userSettings->pAudioBufferSize  >= 16384) {
		audio_player->setAudioBufferSize(audio_ctrl->myApp.userSettings->pAudioBufferSize);
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
	if (fxa->fadeInTime() > 0) {
		setVolume(0);
		fadeinFxAudio(targetVolume,fxa->fadeInTime());
	}
	else if (startPosMs > 0 && initVol == -1) {
		setVolume(0);
		fadeinFxAudio(targetVolume,200);
	}
	else {
		setVolume(targetVolume);
	}

	// Feed audio device with decoded data
	audio_player->start(fxa->loopTimes);

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = current_volume;
	msg.executer = exec;
	emit audioCtrlMsgEmitted(msg);

	if (exec && AppCentral::instance()->execCenter->useExecuter(exec)) {
		LOGTEXT(tr("<font color=green>Start %1 in audio slot %2</font> with Executer: %3 at position %4 with volume %5")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(exec->getIdString())
				.arg(QtStaticTools::msToTimeString(target_pos_ms))
				.arg(targetVolume));
	} else {
		exec = nullptr;
		LOGTEXT(tr("<font color=green>Start %1 in audio slot %2</font> at time %3 with volume %4")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(QtStaticTools::msToTimeString(target_pos_ms))
				.arg(targetVolume));
	}

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			current_fx->startInProgress = false;
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

#ifdef USE_SDL
	if (m_isSDLAudio)
		return sdlStopFxAudio();
#endif

	if (run_status > AUDIO_IDLE) {
		LOGTEXT(tr("Stop Audio playing in slot %1").arg(slotNumber+1));
		audio_player->stop();
		return true;
	} else {
		if (audio_player->state() != AUDIO_IDLE) {
			LOGERROR(tr("Stop Audio playing in slot %1: But audio channel is idle").arg(slotNumber+1));
			audio_player->stop();
		}
		return false;
	}
}

bool AudioSlot::pauseFxAudio(bool state)
{
	qDebug() << "pause audio" << state;

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
		qDebug("Fade out time = %d",time_ms);
		return false;
	}
	// Fadeout only possible if audio is running
	if (m_isSDLAudio) {
#ifdef USE_SDL
		if (Mix_Playing(slotNumber) != 1)
			return false;
#endif
	}
	else {
		if (audio_player->state() != AUDIO_PLAYING)
			return false;
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

void AudioSlot::setVolume(qreal vol)
{
	setVolume(int(vol));
}

void AudioSlot::setVolume(int vol)
{
	qreal level;
	if (m_isSDLAudio) {
		level = qreal(vol) * SDL_MAX_VOLUME / MAX_VOLUME;
		if (master_volume >= 0) {
			level *= qreal(master_volume) / MAX_VOLUME;
		}
	}
	else {
		level = qreal(vol);
		if (master_volume >= 0) {
			level *= qreal(master_volume) / MAX_VOLUME;
		}
	}

#ifdef IS_QT5
	if (m_isSDLAudio) {
#ifdef USE_SDL
		Mix_Volume(slotNumber, int(level));
#endif
	}
	else {
		if (!audio_player) return;
		audio_player->setVolume(int(level),MAX_VOLUME);
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

	if (m_isSDLAudio) {
		loop = 1;
		time_ms = run_time.elapsed();
		per_mille = time_ms * 1000 / soundlen;
	}
	else {
		loop = audio_player->currentLoop();
		time_ms = audio_player->currentPlayPosMs();
		per_mille = time_ms * 1000 / soundlen;

	}

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
		qDebug() << "onPlayerStatusChanged" << status << cur_status << audio_player->currentAudioCmd();

		switch (status) {
		case AUDIO_MEDIA_STALLED:
			DEBUGERROR("Audio has stalled on channel %1. Buffer underrun?",slotNumber+1);
			run_status = AUDIO_ERROR;
			break;
		case AUDIO_RUNNING:
			if (current_fx)
				current_fx->startInProgress = false;
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
	case CMD_AUDIO_PAUSE:
		pauseFxAudio(!(run_status == AUDIO_PAUSED));
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
		return trUtf8("No FX was set");
	}
}

#ifdef USE_SDL
bool AudioSlot::sdlStartFxAudio(FxAudioItem *fxa, Executer *exec, qint64 startPosMs, int initVol)
{
	m_isSDLAudio = true;
	m_sdlAudioFormat.setChannelCount(2);
	m_sdlAudioFormat.setSampleSize(16);
	m_sdlAudioFormat.setSampleType(QAudioFormat::SignedInt);
	m_sdlAudioFormat.setSampleRate(44100);

	current_fx = fxa;
	current_executer = exec;
	sdlSetRunStatus(AUDIO_BUFFERING);
	run_time.start();

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

	if (m_sdlChunk) {
		*m_sdlChunk = m_sdlChunkCopy;
		Mix_FreeChunk(m_sdlChunk);
	}

	// Load audio data
	m_sdlChunk = Mix_LoadWAV(fxa->filePath().toLocal8Bit().data());	// 1 means, automatically free the source
	if (m_sdlChunk) {
		m_sdlChunkCopy = *m_sdlChunk;
		qDebug() << "Loaded SDL sound:" << fxa->filePath();
	} else {
		LOGERROR(tr("Could not load SDL file: ").arg(fxa->fileName()));
		sdlSetRunStatus(AUDIO_ERROR);
		return false;
	}

	Mix_RegisterEffect(slotNumber, AudioSlot::sdlChannelProcessor, NULL, NULL);

	qint64 durationMs = qint64(m_sdlChunk->alen) * 1000 / (44100 * 4);
	setAudioDurationMs(durationMs);

	if (startPosMs < 0) {
		// We get the start play position in ms from the last played seek position in the FxAudio instance
		if (fxa->seekPosition() == 0) {
			startPosMs = 0;
		}
		else if (fxa->seekPosition() > 0) {
			startPosMs = fxa->seekPosition();
		}
		// Reset the seek position to zero
		fxa->setSeekPosition(0);
	}
	uint seekpos = uint( (44100 * startPosMs / 1000) * 4 );
	if (seekpos >= m_sdlChunk->alen)
		return false;

	m_sdlChunk->abuf = m_sdlChunk->abuf + seekpos;
	m_sdlChunk->alen = m_sdlChunk->alen - seekpos;

	// set initial Volume
	if (fxa->fadeInTime() > 0) {
		setVolume(0);
		fadeinFxAudio(targetVolume,fxa->fadeInTime());
	}
	else if (startPosMs > 0 && initVol == -1) {
		setVolume(0);
		fadeinFxAudio(targetVolume,200);
	}
	else {
		setVolume(targetVolume);
	}

	// Play audio data
	int res = Mix_PlayChannel(slotNumber, m_sdlChunk, fxa->loopValue());
	if (res == -1) {
		LOGERROR(tr("SDL error: %1").arg(Mix_GetError()));
		sdlSetRunStatus(AUDIO_ERROR);
		return false;
	}

	sdlSetRunStatus(AUDIO_RUNNING);

	// Emit Control Msg to send Status of Volume and Name
	AudioCtrlMsg msg(fxa,slotNumber);
	msg.volume = current_volume;
	msg.executer = exec;
	emit audioCtrlMsgEmitted(msg);

	if (exec && AppCentral::instance()->execCenter->useExecuter(exec)) {
		LOGTEXT(tr("<font color=green>Start %1 in audio slot %2</font> with Executer: %3 at position %4 with volume %5")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(exec->getIdString())
				.arg(QtStaticTools::msToTimeString(startPosMs))
				.arg(targetVolume));
	} else {
		exec = nullptr;
		LOGTEXT(tr("<font color=green>Start %1 in audio slot %2</font> at time %3 with volume %4")
				.arg(fxa->name())
				.arg(slotNumber+1)
				.arg(QtStaticTools::msToTimeString(startPosMs))
				.arg(targetVolume));
	}

	bool ok = false;
	while (run_time.elapsed() < FX_AUDIO_START_WAIT_DELAY && !ok) {
		QApplication::processEvents();
		if (run_status == AUDIO_RUNNING) {
			current_fx->startInProgress = false;
			qreal vol = 0;
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

	}

	if (exec)
		AppCentral::instance()->execCenter->freeExecuter(exec);

	return true;
}

bool AudioSlot::sdlStopFxAudio()
{
	Mix_HaltChannel(slotNumber);

	return true;
}

void AudioSlot::sdlSetFinished()
{
	*m_sdlChunk = m_sdlChunkCopy;

	sdlSetRunStatus(AUDIO_IDLE);
}

void AudioSlot::sdlSetRunStatus(AudioStatus state)
{
	if (state != run_status) {
		run_status = state;
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

void AudioSlot::sdlChannelProcessStream(void *stream, int len, void *udata)
{
	Q_UNUSED(udata)
	audio_io->calcVuLevel(reinterpret_cast<const char*>(stream), len, m_sdlAudioFormat);
	emit_audio_play_progress();
}

void AudioSlot::sdlChannelProcessor(int chan, void *stream, int len, void *udata)
{
	const AppCentral &app = AppCentral::ref();
	if (chan < 0 || chan >= app.unitAudio->usedSlots())
		return;

	AudioSlot *slot = app.unitAudio->audioSlot(chan);
	slot->sdlChannelProcessStream(stream, len, udata);
}

void AudioSlot::sdlChannelProcessorFxDone(int chan, void *udata)
{
	Q_UNUSED(chan)
	Q_UNUSED(udata)
}
#endif


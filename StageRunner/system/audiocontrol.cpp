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

#include "audiocontrol.h"
#include "config.h"
#include "log.h"
#include "audioslot.h"
#include "audioiodevice.h"
#include "appcentral.h"
#include "usersettings.h"
#include "fxaudioitem.h"
#include "fxplaylistitem.h"
#include "executer.h"
#include "execcenter.h"
#include "fxlistwidget.h"
#include "fxcontrol.h"
#include "audioformat.h"
#include "fxlist.h"
#include "fxclipitem.h"
#include "customwidget/psvideowidget.h"
#include "videoplayer.h"
#include "videocontrol.h"
#include "system/fxtimer.h"

#include <QStringList>
#include <QDebug>

#ifdef IS_QT5
#include <QAudioDeviceInfo>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QUrl>
#include <QVideoWidget>
#include <QThread>
#endif

#ifdef USE_SDL
#include "audiooutput/sdl2audiobackend.h"
#endif

AudioControl::AudioControl(AppCentral &app_central, bool initInThread)
	: QThread()
	,myApp(app_central)
	,m_initInThread(initInThread)
	,used_slots(MAX_AUDIO_SLOTS)
	,slotMutex(new QRecursiveMutex())
{
	init();
	getAudioDevices();

	if (!m_initInThread)
		createMediaPlayInstances();

	// Audio Thread starten
	start();
	if (!isRunning()) {
		DEBUGERROR("%s: Could not start audio control thread",Q_FUNC_INFO);
		m_isValid = false;
	} else {
		m_isValid = true;
	}
}

AudioControl::~AudioControl()
{
	if (isRunning()) {
		quit();
		wait(500);
	}

	if (!m_initInThread)
		destroyMediaPlayInstances();

	delete slotMutex;
}

void AudioControl::reCreateMediaPlayerInstances()
{
	stopAllFxAudio();

	if (isRunning()) {
		m_isValid = false;
		quit();
		wait(500);
	}

	if (!m_initInThread) {
		destroyMediaPlayInstances();
		createMediaPlayInstances();
	}

	if (!isRunning()) {
		start();
		if (isRunning()) {
			m_isValid = true;
		}
	} else {
		DEBUGERROR("Restart of audio control failed!");
	}

}

void AudioControl::getAudioDevices()
{
//	qDebug() << "getAudioDevices";

	m_audioDeviceNames.clear();

	AudioFormat default_format = AudioFormat::defaultFormat();

	QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int t=0; t<devList.size(); t++) {
		QAudioDeviceInfo &dev = devList[t];
		bool default_format_supported = dev.isFormatSupported(default_format);
		QString dev_name = dev.deviceName();
		if (default_format_supported) {
			m_audioDeviceNames.append(dev_name);
			LOGTEXT(tr("Audio Device: %1").arg(dev_name));
			/// @todo this takes a hardcoded device name and should be adjustable
			if (dev_name.contains("UA-25EX")) {
				m_extraDevice = dev;
			}
		}
	}
	LOGTEXT(tr("<font color=info>Default Audio: %1</font>").arg(QAudioDeviceInfo::defaultOutputDevice().deviceName()));
	if (!m_extraDevice.isNull())
		LOGTEXT(tr("<font color=info>Extra Audio: %1</font>").arg(m_extraDevice.deviceName()));

	QAudioDeviceInfo def_dev(QAudioDeviceInfo::defaultOutputDevice());
	if (def_dev.isFormatSupported(AudioFormat::defaultFormat())) {
		LOGTEXT(tr("<font color=ok>Default format supported</font>"));
	} else {
		LOGTEXT(tr("<font color=error>Default audio format not supported by audio device"));
	}

	if (audioSlots.size())
		LOGTEXT(tr("Audio buffer size is: %1").arg(audioSlots[0]->audioOutputBufferSize()));
}

/**
 * @brief Check if FxAudioItem is active (means Playing or Paused)
 * @param fxa Pointer to FxAudioItem instance
 * @return true, if audio is in RUNNING state or PAUSED state
 */
bool AudioControl::isFxAudioActive(FxAudioItem *fxa)
{
	if (!fxa) return false;

	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa)
			return audioSlots[t]->isActive() || fxa->startInProgress;
	}

	return false;
}

bool AudioControl::isFxAudioActive(int slotnum)
{
	if (slotnum < 0 || slotnum >= used_slots)
		return false;

	return audioSlots[slotnum]->isActive();
}

int AudioControl::findAudioSlot(FxAudioItem *fxa)
{
	if (!fxa) return false;

	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			return t;
		}
	}
	return -1;
}

/**
* @brief Find and select a free audio slot.
* @param slotnum [default -1] use dedicated channel if number is >= 0
* @return slot number that was selected or -1 if none has found
*
*/
int AudioControl::selectFreeAudioSlot(int slotnum)
{
	if (slotnum > used_slots)
		return -1;

	int slot = -1;
	slotMutex->lock();
	if (slotnum < 0) {
		int i = 0;
		while (slot < 0 && i < used_slots) {
			if (audioSlots[i]->status() <= AUDIO_IDLE) {
				audioSlots[i]->select();
				slot = i;
			}

			i++;
		}
	} else {
		if (audioSlots[slotnum]->select())
			slot = slotnum;
	}
	slotMutex->unlock();
	return slot;
}

int AudioControl::selectFreeVideoSlot(bool *isVideoRunning)
{
	int slot = -1;
	slotMutex->lock();

	int i = -1;
	while (slot < 0 && ++i < usedSlots()) {
		if (audioSlots[i]->status() >= VIDEO_INIT && audioSlots[i]->status() <= VIDEO_RUNNING) {
			slot = i;
			*isVideoRunning = true;
		}
	}

	i = -1;
	while (slot < 0 && ++i < usedSlots()) {
		if (audioSlots[i]->status() <= AUDIO_IDLE) {
			audioSlots[i]->selectFxClipVideo();
			slot = i;
			*isVideoRunning = false;
		}
	}

	slotMutex->unlock();
	return slot;
}

/**
 * @brief Select video slot and prepare it for video start
 * @param slotnum
 * @return true on succuss. If false, no further video actions are allowed
 */
bool AudioControl::selectVideoSlot(int slotnum)
{
	bool ok = false;
	slotMutex->lock();
	if (audioSlots[slotnum]->status() <= AUDIO_IDLE) {
		ok = audioSlots[slotnum]->selectFxClipVideo();
	}
	slotMutex->unlock();
	return ok;
}

void AudioControl::setFFTAudioChannelFromMask(qint32 mask)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (mask & (1<<t)) {
			audioSlots[t]->setFFTEnabled(true);
		} else {
			audioSlots[t]->setFFTEnabled(false);
		}
	}
}

AudioPlayer *AudioControl::audioPlayer(int i) const
{
	if (i < 0 || i >= usedSlots())
		return nullptr;

	return audioSlots.at(i)->audioPlayer();
}

/**
 * @brief Search audio fx in any audio slot and return current play volume if found
 * @param fxa
 * @return current play volume or -1, if audio was not found
 */
int AudioControl::evaluateCurrentVolumeForFxAudio(FxAudioItem *fxa)
{
	int curvol = -1;
	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			AudioSlot *slot = audioSlots[t];
			curvol = slot->volume();
			break;
		}
	}

	return curvol;
}

void AudioControl::closeVideoWidget()
{
	if (m_videoWid) {
		m_videoWid->setPicClipOverlaysActive(false);
		m_videoWid->close();
	}
}

bool AudioControl::isVideoWidgetVisible(QWidget ** videoWid) const
{
	if (m_videoWid) {
		if (videoWid)
			*videoWid = m_videoWid;
		return m_videoWid->isVisible();
	}
	if (videoWid)
		*videoWid = nullptr;

	return false;
}

void AudioControl::run()
{
	if (m_initInThread)
		createMediaPlayInstances();

	LOGTEXT(tr("Audio control is running"));

	exec();

	if (m_initInThread)
		destroyMediaPlayInstances();

	LOGTEXT(tr("Audio control finished"));
}

void AudioControl::vu_level_changed_receiver(int slotnum, qreal left, qreal right)
{
	if (slotnum < 0 || slotnum >= audioSlots.size())
		return;

	int vol = audioSlots.at(slotnum)->volume();
	// emit vuLevelChanged(slotnum, left, right);
	double volf = double(vol) / MAX_VOLUME;

	emit vuLevelChanged(slotnum, left * (pow(10,volf)-1) * 10 / 90, right * (pow(10,volf)-1) * 10 / 90);
}

void AudioControl::fft_spectrum_changed_receiver(int slotnum, FrqSpectrum *spec)
{
	if (slotnum < 0 || slotnum >= used_slots) return;
	emit fftSpectrumChanged(slotnum,spec);
}

bool AudioControl::startFxAudioStage2(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol)
{
	QMutexLocker lock(slotMutex);

	// Let us test if Audio is already running in a slot (if double start prohibition is enabled)
	if (!exec && myApp.userSettings->pProhibitAudioDoubleStart) {
		for (int t=0; t<used_slots; t++) {
			if (audioSlots[t]->currentFxAudio() == fxa) {
				// Ok, audio is already running -> Check the time
				int cur_run_time = audioSlots[t]->currentRunTime();
				if ( cur_run_time >= 0 && cur_run_time < myApp.userSettings->pAudioAllowReactivateTime) {
					LOGERROR(tr("Audio Fx '%1' not started since it is already running on channel %2")
							 .arg(fxa->name()).arg(t+1));
					return false;
				}
			}
		}
	}


	int slot;
	if (fxa->playBackSlot == 0) {
		slot = selectFreeAudioSlot();
		if (slot < 0)
			return false;
	} else {
		slot = fxa->playBackSlot-1;
		if (slot >= used_slots)
			return false;
		if (audioSlots[slot]->status() > AUDIO_IDLE)
			return false;

		audioSlots[slot]->select();
	}

	// attention if audio fx is started more than once. This is the last instance.
	fxa->startSlot = slot;

	if (atMs < 0)
		atMs = 0;

	return start_fxaudio_in_slot(fxa, slot, exec, atMs, initVol);
}

void AudioControl::startFxAudioFromTimer(FxItem *fx)
{
	FxAudioItem *fxa = dynamic_cast<FxAudioItem*>(fx);
	if (!fxa) return;

	startFxAudioStage2(fxa, nullptr);
}


bool AudioControl::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol)
{
	QMutexLocker lock(slotMutex);
	executeAttachedAudioStartCmd(fxa);

//	if (fxa->preDelay() > 0) {
//		FxTimer *fxt = new FxTimer(fxa);
//		connect(fxt, SIGNAL(fxTimeout(FxItem*)),this,SLOT(startFxAudioFromTimer(FxItem*)));
//		fxt->start(fxa->preDelay());
//		return true;
//	}

	return startFxAudioStage2(fxa, exec, atMs, initVol);
}

bool AudioControl::startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs, int initVol)
{
	if (slotnum < 0 || slotnum >= used_slots)
		return false;

	if (audioSlots[slotnum]->select()) {
		if (start_fxaudio_in_slot(fxa,slotnum,exec,atMs,initVol))
			return true;

		audioSlots[slotnum]->unselect();
	}
	return false;
}

/**
 * @brief Start FxAudioItem in dedicated slot
 * @param fxa Pointer to FxAudioItem instance
 * @param slotnum The slotnumber
 * @param exec [default 0] Poiner to Executer (if started from an executer)
 * @param atMs [default -1] Start at this seek position in ms (if -1: the last value stored in FxAudioItem will be used later)
 * @param initVol [default -1] The initial volume for audio playback (-1: the value stored in FxAudioItem will be used)
 *
 * @note the audio slot that is used must be in AudioStatus AUDIO_INIT. This means the slot has to
 * be selected before (use audioSlots[slotnum]->select() to do that
 *
 * @return true means ok
 */
bool AudioControl::start_fxaudio_in_slot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs, int initVol)
{
	QMutexLocker lock(slotMutex);

	if (audioSlots[slotnum]->status() == AUDIO_INIT) {

		if (atMs >= 0)
			fxa->setSeekPosition(atMs);

		fxa->startInProgress = true;

		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		AudioCtrlMsg msg(fxa, slotnum, CMD_AUDIO_START_AT, exec);
		msg.volume = initVol;
		if (fxa->isDmxStarted && initVol < 0 && fxa->initialVolume == 0) {
			msg.isDmxVolumeLocked = true;
		}
		emit audioThreadCtrlMsgEmitted(msg);

		return true;

	} else {
		DEBUGERROR("Slot %d is not selected when trying to start '%s'"
				   ,fxa->name().toLocal8Bit().data());
		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_STATUS_CHANGED, exec);
		msg.currentAudioStatus = AUDIO_NO_FREE_SLOT;
		emit audioCtrlMsgEmitted(msg);
		return false;
	}
}


/**
 * @brief AudioControl::startFxClipItemInSlot
 * @param fxc
 * @param slotnum
 * @param exec
 * @param atMs
 * @param initVol
 * @return
 *
 * This is used for audio control of video media files, which are player in VideoPlayer handled by VideoControl
 */
bool AudioControl::startFxClipItemInSlot(FxClipItem *fxc, int slotnum, Executer *exec, qint64 atMs, int initVol)
{
	Q_UNUSED(atMs)

	if (audioSlots[slotnum]->status() == VIDEO_INIT) {
		AudioCtrlMsg msg(fxc, slotnum, CMD_VIDEO_START, exec);
		msg.volume = initVol >=0 ? initVol : fxc->initialVolume;
		emit audioThreadCtrlMsgEmitted(msg);

		return true;
	} else {
		return false;
	}
}

bool AudioControl::restartFxAudioInSlot(int slotnum)
{
	if (slotnum < 0 || slotnum >= used_slots) return false;

	QMutexLocker lock(slotMutex);

	if (audioSlots[slotnum]->isPaused()) {
		// AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_PAUSE);
		// msg.volume = audioSlots[slotnum]->volume();
		AudioCtrlMsg msg(slotnum, CMD_AUDIO_PAUSE);
		emit audioThreadCtrlMsgEmitted(msg);
		return true;
	}
	else if (audioSlots[slotnum]->select()) {
		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		FxAudioItem *fxa = audioSlots[slotnum]->currentFxAudio();
		if (!fxa) {
			audioSlots[slotnum]->unselect();
			return false;
		}

		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_START);
		msg.volume = audioSlots[slotnum]->volume();
		emit audioThreadCtrlMsgEmitted(msg);
		return true;
	}

	return false;
}

int AudioControl::stopAllFxAudio()
{
	QMutexLocker lock(slotMutex);

	int stopcnt = 0;
	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->status() > AUDIO_IDLE)
			stopcnt++;
		stopFxAudio(t);
	}
	return stopcnt;
}

bool AudioControl::stopFxAudioWithID(int fxID)
{
	QMutexLocker lock(slotMutex);
	bool found = false;
	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->status() > AUDIO_IDLE && audioSlots[t]->currentFxAudio()->id() == fxID) {
			found = true;
			stopFxAudio(t);
		}
	}
	return found;
}

void AudioControl::pauseFxAudio(int slot)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);
	if (audioSlots[slot]->status() <= AUDIO_IDLE) {
		audioSlots[slot]->unselect();
	}
	// We send this via Message to communicate with the thread
	AudioCtrlMsg msg(slot,CMD_AUDIO_PAUSE);
	msg.executer = audioSlots[slot]->currentExecuter();
	emit audioThreadCtrlMsgEmitted(msg);
}

void AudioControl::stopFxAudio(int slot)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);
	if (audioSlots[slot]->status() <= AUDIO_IDLE || audioSlots[slot]->status() == VIDEO_INIT) {
		audioSlots[slot]->unselect();
	}
	else if (audioSlots[slot]->status() == VIDEO_RUNNING) {
		return;
	}

	// We send this via Message to communicate with the thread
	AudioCtrlMsg msg(slot,CMD_AUDIO_STOP);
	msg.executer = audioSlots[slot]->currentExecuter();
	emit audioThreadCtrlMsgEmitted(msg);
}

void AudioControl::stopFxAudio(FxAudioItem *fxa)
{
	if (!fxa) return;
	QMutexLocker lock(slotMutex);

	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
			stopFxAudio(t);
		}
	}
}

void AudioControl::storeCurrentSeekPositions()
{
	QMutexLocker lock(slotMutex);

	for (int t=0; t<used_slots; t++) {
		audioSlots[t]->storeCurrentSeekPos();
	}
}

void AudioControl::storeCurrentSeekPos(int slot)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);
	audioSlots[slot]->storeCurrentSeekPos();
}

void AudioControl::fadeVolTo(int slot, int targetVolume, int time_ms)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);

	FxAudioItem *fxaudio = audioSlots[slot]->currentFxAudio();
	if (fxaudio == nullptr) {
		LOGTEXT(tr("FADEVOL canceled since there is no audio running in slot #%1").arg(slot+1));
		return;
	}

	int currentVol = audioSlots[slot]->volume();
	if (targetVolume == currentVol) {
		LOGTEXT(tr("FADEVOL canceled since tareget volume is already reached in slot #%1").arg(slot+1));
		return;
	}

	// This is a private message to my audio thread
	AudioCtrlMsg msg(slot,CMD_AUDIO_FADEIN);
	if (currentVol > targetVolume)
		msg.ctrlCmd = CMD_AUDIO_FADEOUT;
	msg.volume = targetVolume;
	msg.fadetime = time_ms;
	msg.executer = audioSlots[slot]->currentExecuter();
	emit audioThreadCtrlMsgEmitted(msg);

	// audioSlots[slot]->fadeinFxAudio(targetVolume, time_ms);
}

int AudioControl::fadeoutAllFxAudio(int time_ms)
{
	QMutexLocker lock(slotMutex);

	int fadecnt = 0;
	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->status() > AUDIO_IDLE)
			fadecnt++;
		fadeoutFxAudio(t,time_ms);
	}
	return fadecnt;
}

void AudioControl::fadeoutFxAudio(int slot, int time_ms)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);
	dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;

	if (Log::isMainThread()) {
		audioSlots[slot]->fadeoutFxAudio(0,time_ms);
	}
	else {
		// This is a private message to my audio thread
		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
		msg.fadetime = time_ms;
		msg.executer = audioSlots[slot]->currentExecuter();
		emit audioThreadCtrlMsgEmitted(msg);
	}
}

void AudioControl::fadeoutFxAudio(FxAudioItem *fxa, int time_ms)
{
	QMutexLocker lock(slotMutex);

	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			fadeoutFxAudio(t,time_ms);
		}
	}
}

void AudioControl::fadeoutFxAudio(Executer *exec, int time_ms)
{
	QMutexLocker lock(slotMutex);

	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentExecuter() == exec) {
			fadeoutFxAudio(t,time_ms);
		}
	}
}

/**
 * @brief AudioControl::fadeinFxAudio
 * @param slot
 * @param targetVolume
 * @param time_ms
 *
 * @attention this function must be called from main thread!! Otherwise you will get a timer not in thread error
 */
void AudioControl::fadeinFxAudio(int slot, int targetVolume, int time_ms)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);

	FxAudioItem *fxaudio = audioSlots[slot]->currentFxAudio();
	if (fxaudio == nullptr) {
		LOGERROR(tr("FADEVOL canceled since there is no audio running in slot #%1").arg(slot+1));
		return;
	}

	if (Log::isMainThread()) {
		audioSlots[slot]->fadeinFxAudio(targetVolume, time_ms);
	}
	else {
		// This is a private message to my audio thread
		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEIN);
		msg.fadetime = time_ms;
		msg.executer = audioSlots[slot]->currentExecuter();
		emit audioThreadCtrlMsgEmitted(msg);
	}
}

bool AudioControl::seekPosPerMilleFxAudio(int slot, int perMille)
{
	if (slot < 0 || slot >= used_slots)
		return false;

	QMutexLocker lock(slotMutex);

	// dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
	// This is a private message to my audio thread
	//		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
	//		msg.fadetime = time_ms;
	//		msg.executer = audioChannels[slot]->currentExecuter();
	// emit audioThreadCtrlMsgEmitted(msg);

	return audioSlots[slot]->seekPosPerMille(perMille);
}

bool AudioControl::seekPosPerMilleFxAudio(FxAudioItem *fxa, int perMille)
{
	QMutexLocker lock(slotMutex);

	bool seek = false;
	for (int t=0; t<used_slots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			seek = audioSlots[t]->seekPosPerMille(perMille);
		}
	}
	return seek;
}

bool AudioControl::executeAttachedAudioStartCmd(FxAudioItem *fxa)
{
	switch (fxa->attachedStartCmd) {
	case FxAudioItem::ATTACHED_CMD_FADEOUT_ALL:
		myApp.fadeoutAllFxAudio(fxa->attachedStartPara2 > 0 ? fxa->attachedStartPara2 : myApp.userSettings->pDefaultAudioFadeoutTimeMs);
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_ALL:
		myApp.stopAllFxAudio();
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_VIDEO:
		myApp.videoBlack(0);
		break;
	case FxAudioItem::ATTACHED_CMD_START_FX:
		myApp.executeFxCmd(fxa->attachedStartPara1, CMD_FX_START, nullptr);
//		myApp.unitVideo->startFxClipById(fxa->attachedStartPara1);
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_ALL_SEQ_AND_SCRIPTS:
		myApp.stopAllSequencesAndPlaylists();
		break;
	case FxAudioItem::ATTACHED_CMD_SET_MASTER_VOL:
		if (myApp.unitAudio->masterVolume() != fxa->attachedStartPara1)
			fxa->tmpMasterVolAtStart = myApp.unitAudio->masterVolume();
		myApp.unitAudio->setMasterVolume(fxa->attachedStartPara1);
		break;
	default:
		break;
	}
	return true;
}

bool AudioControl::executeAttachedAudioStopCmd(FxAudioItem *fxa)
{
	if (!FxItem::exists(fxa))
		return true;

	switch (fxa->attachedStopCmd) {
	case FxAudioItem::ATTACHED_CMD_FADEOUT_ALL:
		myApp.fadeoutAllFxAudio(fxa->attachedStopPara2 > 0 ? fxa->attachedStopPara2 : myApp.userSettings->pDefaultAudioFadeoutTimeMs);
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_ALL:
		myApp.stopAllFxAudio();
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_VIDEO:
		myApp.videoBlack(0);
		break;
	case FxAudioItem::ATTACHED_CMD_START_FX:
		myApp.executeFxCmd(fxa->attachedStopPara1, CMD_FX_START, nullptr);
//		myApp.unitVideo->startFxClipById(fxa->attachedStopPara1);
		break;
	case FxAudioItem::ATTACHED_CMD_STOP_ALL_SEQ_AND_SCRIPTS:
		myApp.stopAllSequencesAndPlaylists();
		break;
	case FxAudioItem::ATTACHED_CMD_SET_MASTER_VOL:
		fxa->tmpMasterVolAtStart = 0;
		myApp.unitAudio->setMasterVolume(fxa->attachedStopPara1);
		break;
	default:
		break;
	}

	if (fxa->tmpMasterVolAtStart > 0) {
		myApp.unitAudio->setMasterVolume(fxa->tmpMasterVolAtStart);
		fxa->tmpMasterVolAtStart = 0;
	}
	return true;
}

/**
 * @brief [SLOT] The signals from all audio slots will come in here.
 * @param msg
 *
 * This slot is used to repeat the audio control messages from all existing audio slots (channels)
 * So you do not have to connect on each audio channel
 */
void AudioControl::audioCtrlRepeater(AUDIO::AudioCtrlMsg msg)
{
//	qDebug() << "AudioControl::audioCtrlRepeater Ctrl Msg received and forwarded"
//			 << msg.ctrlCmd
//			 << msg.currentAudioStatus
//			 << msg.isActive
//			 << msg.executer;

	if (msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED && msg.currentAudioStatus == AUDIO_IDLE)
		executeAttachedAudioStopCmd(msg.fxAudio);

	if (msg.ctrlCmd == CMD_VIDEO_STATUS_CHANGED) {
		if (msg.slotNumber >= 0 && msg.slotNumber < usedSlots())
			audioSlots[msg.slotNumber]->setFxClipVideoCtrlStatus(msg.currentAudioStatus);
	}

	emit audioCtrlMsgEmitted(msg);
}

void AudioControl::audioCtrlReceiver(AUDIO::AudioCtrlMsg msg)
{
	// qDebug("AudioControl::audioCtrlReceiver Ctrl Msg received: %d",msg.ctrlCmd);

	switch (msg.ctrlCmd) {
	case CMD_AUDIO_STOP:
		stopFxAudio(msg.slotNumber);
		break;
	case CMD_AUDIO_START:
		LOGTEXT(tr("Restart Audio Fx in slot %1").arg(msg.slotNumber+1));
		restartFxAudioInSlot(msg.slotNumber);
		break;
	case CMD_AUDIO_FADEIN:
		LOGTEXT(tr("Fade in Audio Fx in slot %1").arg(msg.slotNumber+1));
		fadeinFxAudio(msg.slotNumber,msg.volume,msg.fadetime);
		break;
	case CMD_AUDIO_FADEOUT:
		LOGTEXT(tr("Fade out Audio Fx in slot %1").arg(msg.slotNumber+1));
		fadeoutFxAudio(msg.slotNumber,msg.fadetime);
		break;
	case CMD_AUDIO_CHANGE_VOL:
		setVolume(msg.slotNumber,msg.volume);
		setVolumeInFx(msg.slotNumber,msg.volume, msg.isActive);
		break;
	case CMD_AUDIO_PAUSE:
		pauseFxAudio(msg.slotNumber);
		break;

	default:
		DEBUGERROR("AudioControl::audioCtrlReceiver: Unsupported command received: %d",msg.ctrlCmd);
		break;
	}
}

void AudioControl::setMasterVolume(int vol)
{
	QMutexLocker lock(slotMutex);

	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}

	if (m_masterVolume != vol) {
		m_masterVolume = vol;
		emit masterVolumeChanged(vol);
	}

	// Set master volume for video players too
	myApp.unitVideo->setVideoMasterVolume(vol);

	// And now update the volume settings in all active audioslots
	for (int t=0; t<used_slots; t++) {
		audioSlots[t]->setMasterVolume(vol);
	}
}

void AudioControl::setVolume(int slot, int vol)
{
	QMutexLocker lock(slotMutex);

	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < used_slots) {
		audioSlots[slot]->setVolume(vol);
		dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
		dmx_audio_ctrl_last_vol[slot] = vol;
	}
}

void AudioControl::setVolumeInFx(int slot, int vol, bool setAsInitVolume)
{
	QMutexLocker lock(slotMutex);
	// qDebug() << Q_FUNC_INFO << slot << vol;
	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < used_slots) {
		FxAudioItem *fx = audioSlots[slot]->currentFxAudio();
		if (fx)
			fx->currentVolume = vol;

		if (fx && fx->parentFxList() && fx->parentFxList()->parentFx()) {
			FxItem *parentFx = fx->parentFxList()->parentFx();
			if (parentFx->fxType() == FX_AUDIO_PLAYLIST) {
				if (setAsInitVolume) {
					reinterpret_cast<FxPlayListItem*>(parentFx)->initialVolume = vol;
				} else {
					reinterpret_cast<FxPlayListItem*>(parentFx)->currentVolume = vol;
				}
			}
		}
	}
}

int AudioControl::getVolume(int slot) const
{
	if (slot < 0 || slot >= used_slots)
		return 0;

	return audioSlots[slot]->volume();
}

void AudioControl::setVolumeByDmxInput(int slot, int vol)
{
	QMutexLocker lock(slotMutex);

	if (slot < 0 || slot >= MAX_AUDIO_SLOTS) return;

	if (debug) qDebug("Set Volume by DMX input in slot %d, %d",slot,vol);
	vol = MAX_VOLUME * vol / 255;
	int cvol = getVolume(slot);

	int & ctrl = dmx_audio_ctrl_status[slot];
	int & lvol = dmx_audio_ctrl_last_vol[slot];

	if (vol > lvol) {
		lvol = vol;
		if (vol <= cvol || ctrl == DMX_SLOT_DOWN_CATCH) {
			ctrl = DMX_SLOT_IS_LOWER;
			return;
		}
		if (ctrl != DMX_SLOT_IS_LOWER && ctrl != DMX_SLOT_UP_CATCH) return;

		ctrl = DMX_SLOT_UP_CATCH;
	}
	else if (vol < lvol) {
		lvol = vol;

		if (vol >= cvol || ctrl == DMX_SLOT_UP_CATCH) {
			ctrl = DMX_SLOT_IS_HIGHER;
			return;
		}
		if (ctrl != DMX_SLOT_IS_HIGHER && ctrl != DMX_SLOT_DOWN_CATCH) return;

		ctrl = DMX_SLOT_DOWN_CATCH;
	}
	else {
		return;
	}


	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}

	audioSlots[slot]->setVolume(vol);

	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_STATUS_REPORT;
	msg.slotNumber = slot;
	msg.volume = vol;
	emit audioCtrlMsgEmitted(msg);
}

void AudioControl::setVolumeFromDmxLevel(int slot, int vol)
{
	if (slot < 0 || slot >= MAX_AUDIO_SLOTS) return;
//	if (debug)
//		qDebug("Set Volume slot %d, %d",slot,vol);

	vol = MAX_VOLUME * vol / 255;
	bool locked = audioSlots[slot]->setVolumeFromDMX(vol);

	if (locked) {
		AudioCtrlMsg msg;
		msg.ctrlCmd = CMD_STATUS_REPORT;
		msg.slotNumber = slot;
		msg.volume = vol;
		emit audioCtrlMsgEmitted(msg);
	}
}

void AudioControl::setDmxVolumeToLocked(int slot)
{
	if (slot < 0 || slot >= MAX_AUDIO_SLOTS) return;
	audioSlots[slot]->setDmxVolLockState(DMX_AUDIO_LOCKED);
}

bool AudioControl::handleDmxInputAudioEvent(FxAudioItem *fxa, uchar value)
{
	if (myApp.isApplicationStart())
		return false;

	QMutexLocker lock(slotMutex);

	// qDebug() << "dmx audio" << fxa << value;

	bool ok = true;
	if (value > 3) {
		if (!fxa->isDmxStarted && !isFxAudioActive(fxa)) {
			fxa->isDmxStarted = true;
			if (fxa->initialVolume == 0) {
				ok = startFxAudio(fxa, nullptr, fxa->seekPosition());
			} else {
				ok = startFxAudio(fxa, nullptr);
			}
			if (!ok)
				fxa->isDmxStarted = false;
		}
		else if (fxa->isDmxStarted) {
			int slot = findAudioSlot(fxa);
			if (slot >= 0)
				setVolumeFromDmxLevel(slot,(value-3) * 255 / 252);
		}
	}
	else {
		fxa->isDmxStarted = false;
		int slot = findAudioSlot(fxa);
		if (slot >= 0) {
			if (fxa->initialVolume == 0)
				storeCurrentSeekPos(slot);
			stopFxAudio(slot);
		}
	}
	return ok;
}


void AudioControl::init()
{
	m_playlist = nullptr;
	m_videoPlayer = nullptr;
	m_videoWid = nullptr;

	setObjectName("Audio Control");
	m_masterVolume = MAX_VOLUME;
	for (int t=0; t<used_slots; t++) {
		dmx_audio_ctrl_last_vol[t] = 0;
		dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
	}
}

void AudioControl::createMediaPlayInstances()
{
	UserSettings *set = myApp.userSettings;
	AudioOutputType outputType = myApp.usedAudioOutputType();
	LOGTEXT(tr("Create media player instances %1").arg(m_initInThread?"in thread":"from main"));

	// This is for audio use
	bool errmsg = false;
	for (int t=0; t<used_slots; t++) {
		QString audiodev = set->pSlotAudioDevice[t];
		if (!audiodev.isEmpty() && audiodev != "system default") {
			if (outputType != OUT_DEVICE) {
				if (!errmsg)
					POPUPERRORMSG("Init audio", tr("There is a dedicated audio device specified for audio slot %1,\n"
												  "but this is only supported for CLASSIC audio mode.\n"
												  "Multi device output not possible! Default audio from system will be used.").arg(t+1));
				errmsg = true;
				myApp.setModuleError(AppCentral::E_AUDIO_MULTI_OUT_FAIL);
			} else {
				bool ok;
				QAudioDeviceInfo dev = AudioIODevice::getAudioDeviceInfo(audiodev, &ok);
				if (!ok) {
					if (!errmsg)
						POPUPERRORMSG("Init audio", tr("Audio device '%1' not found!\n"
													  "Configuration for audio slot %2 failed.\n"
													  "Multi device output not possible! Default audio from system will be used.")
									  .arg(audiodev).arg(t+1));
					errmsg = true;
					myApp.setModuleError(AppCentral::E_AUDIO_DEVICE_NOT_FOUND);
				}
			}
		}
		AudioSlot *slot = new AudioSlot(this, t, outputType, audiodev);
		audioSlots.append(slot);
		AudioErrorType aerror = slot->lastAudioError();
		if (aerror == AUDIO_ERR_DECODER) {
			myApp.setModuleError(AppCentral::E_NO_AUDIO_DECODER);
		}
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AUDIO::AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,qreal,qreal)),this,SLOT(vu_level_changed_receiver(int,qreal,qreal)));
		connect(slot,SIGNAL(frqSpectrumChanged(int,FrqSpectrum*)),this,SLOT(fft_spectrum_changed_receiver(int,FrqSpectrum*)));
		connect(this,SIGNAL(audioThreadCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),slot,SLOT(audioCtrlReceiver(AUDIO::AudioCtrlMsg)));
	}
	// Enable FFT
	setFFTAudioChannelFromMask(myApp.userSettings->pFFTAudioMask);


	// This is for video playback
	m_videoWid = new PsVideoWidget;
	m_videoPlayer = new VideoPlayer(myApp.unitVideo, m_videoWid);
	// m_videoWid->setVideoPlayer(m_videoPlayer);

#ifdef USE_SDL
	Mix_AllocateChannels(used_slots);
	Mix_ChannelFinished(SDL2AudioBackend::sdlChannelDone);
	Mix_SetPostMix(SDL2AudioBackend::sdlPostMix, nullptr);
#endif
}

void AudioControl::destroyMediaPlayInstances()
{
	while (!audioSlots.isEmpty()) {
		if (audioSlots.first()->isActive()) {
			audioSlots.first()->stopFxAudio();
		}
		delete audioSlots.takeFirst();
	}

	delete m_videoWid;
	m_videoWid = nullptr;
	delete m_videoPlayer;
	m_videoPlayer = nullptr;
	delete m_playlist;
	m_playlist = nullptr;
}

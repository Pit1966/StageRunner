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
#include "appcentral.h"
#include "usersettings.h"
#include "fxplaylistitem.h"
#include "fxlistwidget.h"
#include "fx/fxaudioitem.h"
#include "fx/executer.h"
#include "fx/execcenter.h"
#include "fx/fxlist.h"
#include "fx/fxclipitem.h"
#include "system/fxcontrol.h"
#include "system/videocontrol.h"
#include "system/audioslot.h"
#include "system/audioworker.h"
#include "system/fxtimer.h"

#include "system/audiooutput/audioformat.h"

#include <QStringList>
#include <QDebug>
#include <QUrl>
#include <QThread>
#include <QVideoWidget>
#include <QMediaPlayer>

#ifdef IS_QT5
#	include "system/videoplayer.h"
#	include "customwidget/psvideowidget.h"
#	include "system/audiooutput/audioiodevice.h"
#endif

#ifdef IS_QT6
#	include <QMediaDevices>
#	include "system/videoplayer6.h"
#	include "customwidget/psvideowidget6.h"
#	include "system/audiooutput/audioiodevice6.h"
#endif

#ifdef USE_SDL
#include "audiooutput/sdl2audiobackend.h"
#endif

AudioControl::AudioControl(AppCentral &app_central, bool initInThread)
	: QObject()
	, myApp(app_central)
	, m_initInThread(initInThread)
	, slotMutex(new QRecursiveMutex())
{
	setObjectName("AudioControl");
	initDefaults();

	// initAudio(initInThread);
}

AudioControl::~AudioControl()
{

	if (!m_initInThread)
		destroyMediaPlayInstances();

	destroyVideoWidget();

	if (m_audioWorker) {
		m_audioThread.quit();
		m_audioThread.wait(2000);
	}

	delete slotMutex;
}

bool AudioControl::initAudio(bool initInThread)
{
	m_initInThread = initInThread;

	getAudioDevices();

	// Audio Thread starten
	createAudioWorker();

	checkAudioLimits();

	if (!m_initInThread) {
		createMediaPlayInstances();
	}
	else {
		QMetaObject::invokeMethod(m_audioWorker, "initMediaPlayerInstances");
		ExtElapsedTimer wait;
		while (wait.elapsed() < 500) {
			if (m_audioWorker->isValid)
				break;
		}
	}

	// create video widget and player always in main thread
	createVideoWidget();

	m_isInThread = m_initInThread;
	m_isValid = true;
	m_isSmallAudioBufFix = myApp.userSettings->pIsSmallAudioBufferFix;

	return m_isValid;
}

bool AudioControl::createAudioWorker()
{
	if (m_audioWorker)
		return false;

	m_audioThread.setObjectName("AudioThread");
	m_audioWorker = new AudioWorker(this);
	m_audioWorker->setObjectName("AudioWorker");
	m_audioWorker->moveToThread(&m_audioThread);

	connect(&m_audioThread, SIGNAL(finished()), this, SLOT(_audioWorkerFinished()));

	m_audioThread.start();
	return true;
}

AudioWorker *AudioControl::audioWorker()
{
	return m_audioWorker;
}

QThread *AudioControl::audioThread()
{
	return &m_audioThread;
}

void AudioControl::reCreateMediaPlayerInstances()
{
	LOGTEXT(tr("<font color=orange>Reinit audio slots</font> %1")
			.arg(m_initInThread ? tr("for background playing") : tr("for main thread playing")));

	checkAudioLimits();

	if (videoPlayer())
		videoPlayer()->stop();

	stopAllFxAudio(true);
	destroyVideoWidget();

	destroyMediaPlayInstances();
	if (!m_initInThread) {
		createMediaPlayInstances();
	}
	else {
		QMetaObject::invokeMethod(m_audioWorker, "initMediaPlayerInstances");
		ExtElapsedTimer wait;
		while (wait.elapsed() < 500) {
			if (m_audioWorker->isValid)
				break;
		}
	}

	createVideoWidget();

	if (m_usedSlots != audioSlots.size()) {
		POPUPERRORMSG("Reinit mediaplayer",
					  tr("Reinit of audio slots failed!"));
	}
}

void AudioControl::setAudioInThreadEnabled(bool state)
{
	if (state != m_initInThread) {
		m_initInThread = state;
		if (m_initInThread != m_isInThread) {
			LOGTEXT(tr("Audio in background task was %1. Audio must be restarted!")
					.arg(m_initInThread ? tr("activated") : tr("deactivated")));
		}
	}
}

void AudioControl::setSmallAudioBufFix(bool state)
{
	m_isSmallAudioBufFix = state;
}

bool AudioControl::checkAudioLimits()
{
	AudioOutputType outputType = myApp.usedAudioOutputType();
	if (outputType == AUDIO::OUT_NONE) {
		outputType = defaultAudioOut();
	}
	else if (!isAudioOutAvailable(outputType)) {
		outputType = defaultAudioOut();
	}

	if (outputType == AUDIO::OUT_MEDIAPLAYER && myApp.isAudioInThread()) {
		POPUPERRORMSG(tr("Audio control"),
					  tr("Audio output mode Mediaplayer and background audio play is not compatible!\n"
						 "Please deactivate background audio or use I/O audio or SDL!\n\n"
						 "By chance all works correct, but video player may cause problems"));
		return false;
	}

	return true;
}

void AudioControl::getAudioDevices()
{
	//	qDebug() << "getAudioDevices";

	m_audioDeviceNames.clear();
	AudioFormat default_format = AudioFormat::defaultFormat();

#ifdef IS_QT6

	QList<QAudioDevice> devList = QMediaDevices::audioOutputs();
	for (int t=0; t<devList.size(); t++) {
		QAudioDevice &dev = devList[t];
		bool default_format_supported = dev.isFormatSupported(default_format);
		QString dev_name = dev.description();
		if (default_format_supported) {
			m_audioDeviceNames.append(dev_name);
			LOGTEXT(tr("Audio device available: %1").arg(dev_name));
		}
	}

	QAudioDevice def_dev = QMediaDevices::defaultAudioOutput();
	LOGTEXT(tr("<font color=info>Default Audio: %1</font>").arg(def_dev.description()));
	if (def_dev.isFormatSupported(AudioFormat::defaultFormat())) {
		LOGTEXT(tr("<font color=ok>Default format supported</font>"));
	} else {
		LOGTEXT(tr("<font color=error>Default audio format not supported by audio device"));
	}

	if (audioSlots.size())
		LOGTEXT(tr("Audio buffer size is: %1").arg(audioSlots[0]->audioOutputBufferSize()));

#else

	QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int t=0; t<devList.size(); t++) {
		QAudioDeviceInfo &dev = devList[t];
		bool default_format_supported = dev.isFormatSupported(default_format);
		QString dev_name = dev.deviceName();
		if (default_format_supported) {
			m_audioDeviceNames.append(dev_name);
			LOGTEXT(tr("Audio device available: %1").arg(dev_name));
		}
	}
	LOGTEXT(tr("<font color=info>Default Audio: %1</font>").arg(QAudioDeviceInfo::defaultOutputDevice().deviceName()));

	QAudioDeviceInfo def_dev(QAudioDeviceInfo::defaultOutputDevice());
	if (def_dev.isFormatSupported(AudioFormat::defaultFormat())) {
		LOGTEXT(tr("<font color=ok>Default format supported</font>"));
	} else {
		LOGTEXT(tr("<font color=error>Default audio format not supported by audio device"));
	}

	if (audioSlots.size())
		LOGTEXT(tr("Audio buffer size is: %1").arg(audioSlots[0]->audioOutputBufferSize()));

#endif
}

/**
 * @brief Check if FxAudioItem is active (means Playing or Paused)
 * @param fxa Pointer to FxAudioItem instance
 * @return true, if audio is in RUNNING state or PAUSED state
 */
bool AudioControl::isFxAudioActive(FxAudioItem *fxa)
{
	if (!fxa) return false;

	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa)
			return audioSlots[t]->isActive() || fxa->startInProgress;
	}

	return false;
}

bool AudioControl::isFxAudioActive(int slotnum)
{
	if (slotnum < 0 || slotnum >= m_usedSlots)
		return false;

	return audioSlots[slotnum]->isActive();
}

/**
 * @brief Check if any audio slot is used for playback (also paused)
 * @return true, if there is at least one audio slot used
 */
bool AudioControl::isAnyFxAudioActive() const
{
	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->isActive())
			return true;
	}
	return false;
}

int AudioControl::findAudioSlot(FxAudioItem *fxa)
{
	if (!fxa)
		return -1;

	for (int t=0; t<m_usedSlots; t++) {
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
	if (slotnum > m_usedSlots)
		return -1;

	int slot = -1;
	slotMutex->lock();
	if (slotnum < 0) {
		int i = 0;
		while (slot < 0 && i < audioSlots.size()) {
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
		if (t >= audioSlots.size()) {
			LOGERROR("Slots not initialized!");
			break;
		}
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
 * @brief Search audio fx in any audio slots and return current play volume if found
 * @param fxa
 * @return current play volume or -1, if audio was not found
 */
int AudioControl::evaluateCurrentVolumeForFxAudio(FxAudioItem *fxa)
{
	int curvol = -1;
	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			AudioSlot *slot = audioSlots[t];
			curvol = slot->volume();
			break;
		}
	}

	return curvol;
}

/**
 * @brief Search audio fx in audio slots and return the slot number if found
 * @param fxa
 * @return current slot number the audio fx is playing in or -1, if not found
 */
int AudioControl::getSlotForFxAudio(FxAudioItem *fxa)
{
	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa)
			return t;
	}

	return -1;
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

QString AudioControl::audioOutTypeToString(AUDIO::AudioOutputType type)
{
	switch (type) {
	case OUT_NONE:
		return "Default";
	case OUT_DEVICE:
		return "I/O Audio Device";
	case OUT_MEDIAPLAYER:
		return "Mediaplayer";
	case OUT_SDL2:
		return "SDL2";
	default:
		return "Unknown audio output";
	}
}

bool AudioControl::isAudioOutAvailable(AUDIO::AudioOutputType type)
{
#ifdef IS_QT6
	switch (type) {
	case OUT_NONE:
		return true;
	case OUT_DEVICE:
		return true;
	case OUT_MEDIAPLAYER:
		return false;
	case OUT_SDL2:
#	ifdef USE_SDL
		return true;
#	else
		return false;
#	endif
	default:
		return false;
	}
#else
	switch (type) {
	case OUT_NONE:
		return true;
	case OUT_DEVICE:
		return true;
	case OUT_MEDIAPLAYER:
		return true;
	case OUT_SDL2:
#	ifdef USE_SDL
		return true;
#	else
		return false;
#	endif
	default:
		return false;
	}
#endif
}

AudioOutputType AudioControl::defaultAudioOut()
{
#ifdef IS_QT6
	// not optimal for Linux, cause of stop audio and swithch paused to stopped issue
	// but works well on mac.
	return AUDIO::OUT_DEVICE;

#else

#	ifdef IS_MAC
#		ifdef USE_SDL
	return AUDIO::OUT_SDL2
		#		else
	return AUDIO::OUT_MEDIAPLAYER
		#		endif
		#	else
	return AUDIO::OUT_DEVICE;
#	endif

#endif
}


// void AudioControl::run()
// {
// 	if (m_initInThread) {
// 		createMediaPlayInstances();

// 		// test test test
// 		// qDebug() << "-------- test ---- mediaplayer -----";
// 		// qDebug() << "application" << QApplication::instance();
// 		// QMediaPlayer *mp = new QMediaPlayer();
// 	}

// 	LOGTEXT(tr("Audio control is running"));

// 	m_isValid = true;

// 	exec();

// 	m_isValid = false;
// 	if (m_initInThread)
// 		destroyMediaPlayInstances();


// 	LOGTEXT(tr("Audio control finished"));
// }

void AudioControl::_vuLevelChangedReceiver(int slotnum, qreal left, qreal right)
{
	if (slotnum < 0 || slotnum >= audioSlots.size())
		return;

	int vol = audioSlots.at(slotnum)->volume();
	// emit vuLevelChanged(slotnum, left, right);
	double volf = double(vol) / MAX_VOLUME;

	emit vuLevelChanged(slotnum, left * (pow(10,volf)-1) * 10 / 90, right * (pow(10,volf)-1) * 10 / 90);
}

void AudioControl::_fftSpectrumChangedReceiver(int slotnum, FrqSpectrum *spec)
{
	if (slotnum < 0 || slotnum >= m_usedSlots) return;
	emit fftSpectrumChanged(slotnum,spec);
}

void AudioControl::_audioWorkerFinished()
{
	if (m_audioWorker) {
		m_audioWorker->deleteLater();
		m_audioWorker = nullptr;
		LOGTEXT(tr("Audio background task finished"));
	}
}

bool AudioControl::startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	QMutexLocker lock(slotMutex);
	// Execute other configured commands, before audio is started
	executeAttachedAudioStartCmd(fxa);

	return _startFxAudioStage2(fxa, exec, atMs, initVol, fadeInMs);
}

bool AudioControl::startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	if (slotnum < 0 || slotnum >= m_usedSlots)
		return false;

	if (audioSlots[slotnum]->select()) {
		if (_startFxAudioInSlot(fxa, slotnum, exec, atMs, initVol, fadeInMs))
			return true;

		audioSlots[slotnum]->unselect();
	}
	return false;
}

bool AudioControl::_startFxAudioStage2(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	if (debug) {
		qDebug() << "_startFxAudioStage2" << fxa->name()
				 << (exec ? QString("executer:%1").arg(exec->getIdString()) : QString("no executer"))
				 << QString("atMs: %1").arg(atMs)
				 << "initVol" << initVol
				 << QString("fadeInMs: %1").arg(fadeInMs);
	}

	// redirect command, if executed  from main thread, but we are in background task mode
	if (m_isInThread && Log::isMainThread()) {
		bool ok = QMetaObject::invokeMethod(m_audioWorker, "acStartFxAudioStage2", Qt::QueuedConnection,
									   // Q_RETURN_ARG(bool, retok),
									   Q_ARG(FxAudioItem*, fxa),
									   Q_ARG(Executer*, exec),
									   Q_ARG(qint64, atMs),
									   Q_ARG(int, initVol),
									   Q_ARG(int, fadeInMs));

		return ok;
	}

	QMutexLocker lock(slotMutex);

	// Let us test if Audio is already running in a slot (if double start prohibition is enabled)
	if (!exec && myApp.userSettings->pProhibitAudioDoubleStart) {
		for (int t=0; t<audioSlots.size(); t++) {
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
		if (slot < 0) {
			LOGERROR(tr("No free audio slot available!"));
			return false;
		}
	} else {
		slot = fxa->playBackSlot-1;
		if (slot >= m_usedSlots)
			return false;
		if (audioSlots[slot]->status() > AUDIO_IDLE) {
			LOGERROR(tr("Could not start audio FX '%1' in slot %2, since the slot is already occupied!")
					 .arg(fxa->name()).arg(fxa->playBackSlot));
			return false;
		}

		audioSlots[slot]->select();
	}

	// attention if audio fx is started more than once. This is the last instance.
	/// @todo
	fxa->startSlot = slot;

	if (atMs < 0)
		atMs = 0;


	// // redirect command, if executed  from main thread, but we are in background task mode
	// if (m_isInThread && Log::isMainThread()) {
	// 	bool ok = QMetaObject::invokeMethod(m_audioWorker, "acStartFxAudioInSlot", Qt::QueuedConnection,
	// 										// Q_RETURN_ARG(bool, retok),
	// 										Q_ARG(FxAudioItem*, fxa),
	// 										Q_ARG(int, slot),
	// 										Q_ARG(Executer*, exec),
	// 										Q_ARG(qint64, atMs),
	// 										Q_ARG(int, initVol),
	// 										Q_ARG(int, fadeInMs));

	// 	qDebug() << "redirect _startFxAudioInSlot" << ok;

	// 	return ok;
	// }

	return _startFxAudioInSlot(fxa, slot, exec, atMs, initVol, fadeInMs);
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
bool AudioControl::_startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	if (debug) {
		qDebug() << "_startFxAudioInSlot" << fxa->name()
				 << "slotnum" << slotnum
				 << (exec ? QString("executer:%1").arg(exec->getIdString()) : QString("no executer"))
				 << QString("atMs: %1").arg(atMs)
				 << "initVol" << initVol
				 << QString("fadeInMs: %1").arg(fadeInMs);
	}

	QMutexLocker lock(slotMutex);

	if (audioSlots[slotnum]->status() == AUDIO_INIT) {

		if (atMs >= 0)
			fxa->setSeekPosition(atMs);

		fxa->startInProgress = true;

		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		AudioCtrlMsg msg(fxa, slotnum, CMD_AUDIO_START_AT, exec);
		msg.volume = initVol;
		msg.pan = fxa->panning;
		msg.fadetime = fadeInMs;
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
	if (slotnum < 0 || slotnum >= m_usedSlots) return false;

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

int AudioControl::stopAllFxAudio(bool waitForEnd)
{
	QMutexLocker lock(slotMutex);

	int stopcnt = 0;
	for (int t=0; t<audioSlots.size(); t++) {
		if (audioSlots.at(t)->status() > AUDIO_IDLE)
			stopcnt++;
		stopFxAudio(t);
	}

	if (stopcnt > 0 && waitForEnd) {
		bool audioactive;
		do {
			audioactive = false;
			for (int t=0; t<audioSlots.size(); t++) {
				if (audioSlots.at(t)->status() > AUDIO_IDLE)
					audioactive = true;
			}
		} while (audioactive == true);
	}

	return stopcnt;
}

bool AudioControl::stopFxAudioWithID(int fxID)
{
	QMutexLocker lock(slotMutex);
	bool found = false;
	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->status() > AUDIO_IDLE && audioSlots[t]->currentFxAudio()->id() == fxID) {
			found = true;
			stopFxAudio(t);
		}
	}
	return found;
}

void AudioControl::pauseFxAudio(int slot)
{
	if (slot < 0 || slot >= m_usedSlots)
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
	if (slot < 0 || slot >= m_usedSlots)
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

	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
			stopFxAudio(t);
		}
	}
}

void AudioControl::storeCurrentSeekPositions()
{
	QMutexLocker lock(slotMutex);

	for (int t=0; t<m_usedSlots; t++) {
		audioSlots[t]->storeCurrentSeekPos();
	}
}

void AudioControl::storeCurrentSeekPos(int slot)
{
	if (slot < 0 || slot >= m_usedSlots)
		return;

	QMutexLocker lock(slotMutex);
	audioSlots[slot]->storeCurrentSeekPos();
}

void AudioControl::fadeVolTo(int slot, int targetVolume, int time_ms)
{
	if (slot < 0 || slot >= m_usedSlots)
		return;

	QMutexLocker lock(slotMutex);

	FxAudioItem *fxaudio = audioSlots[slot]->currentFxAudio();
	if (fxaudio == nullptr) {
		LOGTEXT(tr("FADEVOL canceled since there is no audio running in slot #%1").arg(slot+1));
		return;
	}

	int currentVol = audioSlots[slot]->volume();
	if (targetVolume == currentVol) {
		LOGTEXT(tr("FADEVOL canceled since target volume is already reached in slot #%1").arg(slot+1));
		return;
	}

	// This is a private message to my audio thread
	AudioCtrlMsg msg(slot,CMD_AUDIO_FADEIN);
	if (currentVol > targetVolume) {
		if (targetVolume > 0) {
			msg.ctrlCmd = CMD_AUDIO_FADETO;
		} else {
			msg.ctrlCmd = CMD_AUDIO_FADEOUT;
		}
	}
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
	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->status() > AUDIO_IDLE)
			fadecnt++;
		fadeoutFxAudio(t,time_ms);
	}
	return fadecnt;
}

void AudioControl::fadeoutFxAudio(int slot, int time_ms)
{
	if (slot < 0 || slot >= m_usedSlots)
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

	for (int t=0; t<m_usedSlots; t++) {
		if (audioSlots[t]->currentFxAudio() == fxa) {
			fadeoutFxAudio(t,time_ms);
		}
	}
}

void AudioControl::fadeoutFxAudio(Executer *exec, int time_ms)
{
	QMutexLocker lock(slotMutex);

	for (int t=0; t<m_usedSlots; t++) {
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
	if (slot < 0 || slot >= m_usedSlots)
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
		msg.volume = targetVolume;
		msg.executer = audioSlots[slot]->currentExecuter();
		emit audioThreadCtrlMsgEmitted(msg);
	}
}

bool AudioControl::seekPosPerMilleFxAudio(int slot, int perMille)
{
	if (slot < 0 || slot >= m_usedSlots)
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
	for (int t=0; t<m_usedSlots; t++) {
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
	// qDebug() << "AudioControl::audioCtrlRepeater Ctrl Msg received and forwarded"
	// 		 << msg.ctrlCmd
	// 		 << msg.currentAudioStatus
	// 		 << msg.isActive
	// 		 << msg.executer;

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
	case CMD_AUDIO_CHANGE_PAN:
		setPanning(msg.slotNumber,msg.pan);
		// setPanningInFx(msg.slotNumber,msg.pan, msg.isActive);
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
	for (int t=0; t<m_usedSlots; t++) {
		if (t >= audioSlots.size()) {
			LOGERROR(QString("Only %1 of %2 audio slots available!").arg(audioSlots.size()).arg(m_usedSlots));
			break;
		}
		audioSlots[t]->setMasterVolume(vol);
	}
}


/**
 * @brief Set Volume in slot
 * @param slot
 * @param vol
 *
 * This function is called by audioCtrlReceiver slot.
 * Usualy when the software calls it internaly not directly by a slider move
 */
void AudioControl::setVolume(int slot, int vol)
{
	QMutexLocker lock(slotMutex);

	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < m_usedSlots) {
		audioSlots[slot]->setVolume(vol);
		dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
		dmx_audio_ctrl_last_vol[slot] = vol;
	}
}


/**
 * @brief Set Volume in slot. Only call this from TimeLine Executers
 * @param slot
 * @param vol
 */
void AudioControl::setVolumeFromTimeLine(int slot, int vol)
{
	QMutexLocker lock(slotMutex);

	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < m_usedSlots) {
		audioSlots[slot]->setVolumeFromTimeLine(vol);
		// dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
		// dmx_audio_ctrl_last_vol[slot] = vol;
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
	if (slot >= 0 && slot < m_usedSlots) {
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
	if (slot < 0 || slot >= m_usedSlots)
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

/**
 * @brief Set panning for audio in slot
 * @param slot
 * @param pan
 *
 * This function is called by audioCtrlReceiver slot.
 * Usualy when the software calls it internaly not directly by a slider move
 */
void AudioControl::setPanning(int slot, int pan)
{
	QMutexLocker lock(slotMutex);

	if (pan < 1) {
		pan = 1;
	}
	else if (pan > MAX_PAN) {
		pan = MAX_PAN;
	}
	if (slot >= 0 && slot < m_usedSlots) {
		audioSlots[slot]->setPanning(pan);
	}
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

VideoPlayer * AudioControl::createVideoPlayer()
{
	if (m_videoPlayer)
		return m_videoPlayer;

	if (!m_videoWid) {
		LOGERROR(tr("No video widget available for videoplayer!"));
		return nullptr;
	}

	m_videoPlayer = new VideoPlayer(myApp.unitVideo, m_videoWid);

	return m_videoPlayer;
}


void AudioControl::initDefaults()
{
	m_usedSlots = MAX_AUDIO_SLOTS;
	m_masterVolume = MAX_VOLUME;

	for (int t=0; t<m_usedSlots; t++) {
		dmx_audio_ctrl_last_vol[t] = 0;
		dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
	}
}

void AudioControl::createMediaPlayInstances()
{
	UserSettings *set = myApp.userSettings;
	AudioOutputType outputType = myApp.usedAudioOutputType();
	if (outputType == AUDIO::OUT_NONE) {
		outputType = defaultAudioOut();
	}
	else if (!isAudioOutAvailable(outputType)) {
		LOGTEXT(tr("<font color=red>Warning!</font> Audio output type %1 not available!").arg(audioOutTypeToString(outputType)));
		outputType = defaultAudioOut();
		LOGTEXT(tr("<font color=red>Warning!</font> Switch to default audio type %1").arg(audioOutTypeToString(outputType)));
	}

	LOGTEXT(tr("Create media player instances of type %1 %2 %3")
			.arg(audioOutTypeToString(outputType),
				 m_initInThread ? "in thread" : "from main",
				 QThread::currentThread()->objectName()));

	// This is for audio use
	bool errmsg = false;
	for (int t=0; t<m_usedSlots; t++) {
		QString audioDevName = set->pSlotAudioDevice[t];
		QString alternativeAudioDevName = set->pSlotAltAudioDevice[t];
		if (!audioDevName.isEmpty() && audioDevName != "system default") {
			if (outputType != OUT_DEVICE) {
				if (!errmsg)
					POPUPERRORMSG("Init audio", tr("There is a dedicated audio device specified for audio slot %1,\n"
												   "but this is only supported for CLASSIC audio mode.\n"
												   "Multi device output not possible! Default audio from system will be used.").arg(t+1));
				errmsg = true;
				myApp.setModuleError(AppCentral::E_AUDIO_MULTI_OUT_FAIL);
			} else {
				bool ok;
#ifdef IS_QT6
				QAudioDevice dev = AudioIODevice::getAudioDevice(audioDevName, &ok);
				if (!ok && !alternativeAudioDevName.isEmpty()) {
					dev = AudioIODevice::getAudioDevice(alternativeAudioDevName, &ok);
					if (ok) {
						LOGTEXT(tr("Audio slot %1: Use alternative audio device: %2").arg(t+1).arg(alternativeAudioDevName));
						audioDevName = alternativeAudioDevName;
					}
				}
#else
				QAudioDeviceInfo dev = AudioIODevice::getAudioDeviceInfo(audioDevName, &ok);
				if (!ok && !alternativeAudioDevName.isEmpty()) {
					dev = AudioIODevice::getAudioDeviceInfo(alternativeAudioDevName, &ok);
					if (ok) {
						audioDevName = alternativeAudioDevName;
						LOGTEXT(tr("Audio slot %1: Use alternative audio device: %2").arg(t+1).arg(alternativeAudioDevName));
					}
				}
#endif
				if (!ok) {
					if (!errmsg)
						POPUPERRORMSG("Init audio", tr("Audio device '<font color=grey>%1</font>' not found!\n"
													   "Configuration for audio slot %2 failed.\n"
													   "Multi device output not possible!\n"
													   "Default audio from system will be used.")
									  .arg(audioDevName).arg(t+1));
					errmsg = true;
					myApp.setModuleError(AppCentral::E_AUDIO_DEVICE_NOT_FOUND);
				}
				Q_UNUSED(dev)
			}
		}

		AudioSlot *slot = new AudioSlot(this, t, outputType, audioDevName);
		audioSlots.append(slot);
		AudioErrorType aerror = slot->lastAudioError();
		if (aerror == AUDIO_ERR_DECODER) {
			myApp.setModuleError(AppCentral::E_NO_AUDIO_DECODER);
		}
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AUDIO::AudioCtrlMsg)));
		connect(this,SIGNAL(audioThreadCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),slot,SLOT(audioCtrlReceiver(AUDIO::AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,qreal,qreal)),this,SLOT(_vuLevelChangedReceiver(int,qreal,qreal)));
		connect(slot,SIGNAL(frqSpectrumChanged(int,FrqSpectrum*)),this,SLOT(_fftSpectrumChangedReceiver(int,FrqSpectrum*)));

		// buffer underrun detection
		connect(slot->audioPlayer(), SIGNAL(audioIdleEventsChanged(int)), this, SIGNAL(audioIdleEventCountChanged(int)));
	}
	// Enable FFT
	setFFTAudioChannelFromMask(myApp.userSettings->pFFTAudioMask);

#ifdef USE_SDL
	Mix_AllocateChannels(m_usedSlots);
	Mix_ChannelFinished(SDL2AudioBackend::sdlChannelDone);
	Mix_SetPostMix(SDL2AudioBackend::sdlPostMix, nullptr);
#endif

	QString statMsg = tr("Audio mode: %1 (%2)")
			.arg(audioOutTypeToString(outputType),
				 m_initInThread ? "background" : "main");

	if (errmsg)
		statMsg += tr(" : Init failed!");
	emit mediaPlayerInstancesCreated(statMsg);
}

void AudioControl::createVideoWidget()
{
	if (!m_videoWid) {
		m_videoWid = new PsVideoWidget;
		m_videoPlayer = new VideoPlayer(myApp.unitVideo, m_videoWid);
		// m_videoPlayer->setVideoOutput(m_videoWid);
	}
}

void AudioControl::destroyMediaPlayInstances()
{
	while (!audioSlots.isEmpty()) {
		if (audioSlots.first()->isActive()) {
			audioSlots.first()->stopFxAudio();
		}
		delete audioSlots.takeFirst();
	}

	m_audioWorker->isValid = false;
}

void AudioControl::destroyVideoWidget()
{
	delete m_videoWid;
	m_videoWid = nullptr;
	delete m_videoPlayer;
	m_videoPlayer = nullptr;
}

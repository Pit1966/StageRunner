#include "audiocontrol.h"
#include "config.h"
#include "log.h"
#include "audiochannel.h"
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



AudioControl::AudioControl(AppCentral &app_central, bool initInThread)
	: QThread()
	,myApp(app_central)
	,m_initInThread(initInThread)
	,used_slots(MAX_AUDIO_SLOTS)
	,slotMutex(new QMutex(QMutex::Recursive))
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

QAudioDeviceInfo AudioControl::getAudioDeviceInfo(const QString &devName, bool *found)
{
	QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int t=0; t<devList.size(); t++) {
		if (devList.at(t).deviceName() == devName) {
			if (found)
				*found = true;
			return devList.at(t);
		}
	}

	if (found)
		*found = false;

	return QAudioDeviceInfo();
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

/**
 * @brief Start Clip (Video)
 * @param fxc
 * @return
 */
bool AudioControl::startFxClip(FxClipItem *fxc)
{
	if (!m_videoPlayer) return false;

	QMultimedia::AvailabilityStatus astat = m_videoPlayer->availability();
	Q_UNUSED(astat)

	qDebug() << "Start FxClip"<< fxc->name();

	setVideoPlayerVolume(fxc->initialVolume);
	m_videoPlayer->playFxClip(fxc);

	return true;
}

void AudioControl::setVideoPlayerVolume(int vol)
{
	float audiolevel = float(vol) * 100 / MAX_VOLUME;
	if (masterVolume >= 0)
		audiolevel *= float(masterVolume) / MAX_VOLUME;

	m_videoPlayer->setVolume(int(audiolevel));
	m_videoPlayerCurrentVolume = vol;
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


bool AudioControl::startFxAudio(FxAudioItem *fxa, Executer *exec)
{
	QMutexLocker lock(slotMutex);

	executeAttachedAudioStartCmd(fxa);

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

	return start_fxaudio_in_slot(fxa, slot, exec, 0);
}

bool AudioControl::startFxAudioAt(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol)
{
	QMutexLocker lock(slotMutex);

	executeAttachedAudioStartCmd(fxa);

	int slot = selectFreeAudioSlot();
	if (slot < 0) {
		return false;
	} else {
		return start_fxaudio_in_slot(fxa, slot, exec, atMs, initVol);
	}
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

		if (atMs >= 0) {
			fxa->setSeekPosition(atMs);
		}
		fxa->startInProgress = true;

		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_START_AT,exec);
		msg.volume = initVol;
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

bool AudioControl::restartFxAudioInSlot(int slotnum)
{
	if (slotnum < 0 || slotnum >= used_slots) return false;

	QMutexLocker lock(slotMutex);

	if (audioSlots[slotnum]->select()) {
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

void AudioControl::stopFxAudio(int slot)
{
	if (slot < 0 || slot >= used_slots)
		return;

	QMutexLocker lock(slotMutex);
	if (audioSlots[slot]->status() <= AUDIO_IDLE) {
		audioSlots[slot]->unselect();
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

	// This is a private message to my audio thread
	//		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
	//		msg.fadetime = time_ms;
	//		msg.executer = audioChannels[slot]->currentExecuter();
	//		emit audioThreadCtrlMsgEmitted(msg);

	audioSlots[slot]->fadeoutFxAudio(0,time_ms);
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
		myApp.fadeoutAllFxAudio();
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
	default:
		break;
	}
	return true;
}

bool AudioControl::executeAttachedAudioStopCmd(FxAudioItem *fxa)
{
	switch (fxa->attachedStopCmd) {
	case FxAudioItem::ATTACHED_CMD_FADEOUT_ALL:
		myApp.fadeoutAllFxAudio();
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
	default:
		break;
	}
	return true;
}

/**
 * @brief [SLOT] The signals from all audio slots will come in here.
 * @param msg
 *
 * This slot is used to repeat the audio control messages from all existing audio slots (channels)
 * So you do not habe to connect on each audio channel
 */
void AudioControl::audioCtrlRepeater(AudioCtrlMsg msg)
{
//	qDebug() << "AudioControl::audioCtrlRepeater Ctrl Msg received and forwarded"
//			 << msg.ctrlCmd
//			 << msg.currentAudioStatus
//			 << msg.isActive
//			 << msg.executer;

	if (msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED && msg.currentAudioStatus == AUDIO_IDLE)
		executeAttachedAudioStopCmd(msg.fxAudio);

	emit audioCtrlMsgEmitted(msg);
}

void AudioControl::audioCtrlReceiver(AudioCtrlMsg msg)
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
	case CMD_AUDIO_FADEOUT:
		LOGTEXT(tr("Fade out Audio Fx in slot %1").arg(msg.slotNumber+1));
		fadeoutFxAudio(msg.slotNumber,msg.fadetime);
		break;
	case CMD_AUDIO_CHANGE_VOL:
		setVolume(msg.slotNumber,msg.volume);
		setVolumeInFx(msg.slotNumber,msg.volume, msg.isActive);
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
	masterVolume = vol;
	for (int t=0; t<used_slots; t++) {
		audioSlots[t]->setMasterVolume(vol);
	}
	if (m_videoPlayer)
		setVideoPlayerVolume(m_videoPlayerCurrentVolume);
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


	if (debug) qDebug("Set Volume slot %d, %d",slot,vol);

	vol = MAX_VOLUME * vol / 255;
	audioSlots[slot]->setVolume(vol);

	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_STATUS_REPORT;
	msg.slotNumber = slot;
	msg.volume = vol;
	emit audioCtrlMsgEmitted(msg);
}

bool AudioControl::handleDmxInputAudioEvent(FxAudioItem *fxa, uchar value)
{
	QMutexLocker lock(slotMutex);

	bool ok = true;
	if (value > 5) {
		if (!fxa->isDmxStarted && !isFxAudioActive(fxa)) {
			ok = startFxAudioAt(fxa, nullptr, -1, 5);
			fxa->isDmxStarted = true;
		}
		else if (fxa->isDmxStarted) {
			int slot = findAudioSlot(fxa);
			if (slot >= 0) {
				setVolumeFromDmxLevel(slot,value);
			}
		}
	}
	else {
		fxa->isDmxStarted = false;
		int slot = findAudioSlot(fxa);
		if (slot >= 0) {
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
	m_videoPlayerCurrentVolume = 0;

	setObjectName("Audio Control");
	masterVolume = MAX_VOLUME;
	for (int t=0; t<used_slots; t++) {
		dmx_audio_ctrl_last_vol[t] = 0;
		dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
	}
}

void AudioControl::createMediaPlayInstances()
{
	UserSettings *set = myApp.userSettings;

	// This is for audio use
	for (int t=0; t<used_slots; t++) {
		AudioSlot *slot = new AudioSlot(this,t,set->pSlotAudioDevice[t]);
		audioSlots.append(slot);
		AudioErrorType aerror = slot->lastAudioError();
		if (aerror == AUDIO_ERR_DECODER) {
			myApp.setModuleError(AppCentral::E_NO_AUDIO_DECODER);
		}
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,qreal,qreal)),this,SLOT(vu_level_changed_receiver(int,qreal,qreal)));
		connect(slot,SIGNAL(frqSpectrumChanged(int,FrqSpectrum*)),this,SLOT(fft_spectrum_changed_receiver(int,FrqSpectrum*)));
		connect(this,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),slot,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	}

	// This is for video playback
	m_videoWid = new PsVideoWidget;
	m_videoPlayer = new VideoPlayer(m_videoWid);
	m_videoWid->setVideoPlayer(m_videoPlayer);
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

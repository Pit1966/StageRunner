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

#include <QStringList>
#include <QDebug>

#ifdef IS_QT5
#include <QAudioDeviceInfo>
#endif



AudioControl::AudioControl(AppCentral &app_central)
	: QThread()
	,myApp(app_central)
{
	init();
	getAudioDevices();

	// Audio Thread starten
	start();
	if (!isRunning()) {
		DEBUGERROR("%s: Could not start audio control thread",Q_FUNC_INFO);
	}
}

AudioControl::~AudioControl()
{
	if (isRunning()) {
		quit();
		wait(500);
	}
}

void AudioControl::getAudioDevices()
{
	QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (int t=0; t<devList.size(); t++) {
		QAudioDeviceInfo &dev = devList[t];
		LOGTEXT(tr("Audio Device: %1").arg(dev.deviceName()));
	}
	LOGTEXT(tr("<font color=darkgreen>Default Audio: %1</font>").arg(QAudioDeviceInfo::defaultOutputDevice().deviceName()));

	QAudioDeviceInfo def_dev(QAudioDeviceInfo::defaultOutputDevice());
	if (def_dev.isFormatSupported(AudioFormat::defaultFormat())) {
		LOGTEXT(tr("<font color=green>Default format supported</font>"));
	} else {
		LOGTEXT(tr("<font color=red>Default audio format not supported by audio device"));
	}
}

void AudioControl::run()
{
	initFromThread();
	LOGTEXT(tr("Audio control is running"));

	exec();

	while (!audioChannels.isEmpty()) {
		delete audioChannels.takeFirst();
	}

	LOGTEXT(tr("Audio control finished"));
}

void AudioControl::vu_level_changed_receiver(int slotnum, int left, int right)
{
	if (slotnum < 0 || slotnum >= audioChannels.size()) return;

	int vol = audioChannels.at(slotnum)->volume();
	emit vuLevelChanged(slotnum, left * vol / MAX_VOLUME, right * vol / MAX_VOLUME);
}

bool AudioControl::startFxAudio(FxAudioItem *fxa, Executer *exec)
{
	bool ok = false;
	// Let us test if Audio is already running in a slot (if double start prohibition is enabled)
	if (!exec && myApp.userSettings->pProhibitAudioDoubleStart) {
		for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
			if (audioChannels[t]->currentFxAudio() == fxa) {
				// Ok, audio is already running -> Check the time
				int cur_run_time = audioChannels[t]->currentRunTime();
				if ( cur_run_time >= 0 && cur_run_time < myApp.userSettings->pAudioAllowReactivateTime) {
					LOGERROR(tr("Audio Fx '%1' not started since it is already running on channel %2")
							 .arg(fxa->name()).arg(t+1));
					return false;
				}
			}
		}
	}

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() < AUDIO_INIT) {
			ok = startFxAudioInSlot(fxa, t, exec, 0);
			break;
		}
	}

	return ok;
}

bool AudioControl::startFxAudioAt(FxAudioItem *fxa, Executer *exec, qint64 atMs)
{
	bool ok = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() < AUDIO_INIT) {
			ok = startFxAudioInSlot(fxa, t, exec, atMs);
			break;
		}
	}
	return ok;
}

bool AudioControl::startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs)
{
	if (audioChannels[slotnum]->status() < AUDIO_INIT) {

		if (atMs >= 0) {
			fxa->setSeekPosition(atMs);
		}
		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_START_AT,exec);
		emit audioThreadCtrlMsgEmitted(msg);

		return true;
	} else {
		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_STATUS_CHANGED, exec);
		msg.currentAudioStatus = AUDIO_NO_FREE_SLOT;
		emit audioCtrlMsgEmitted(msg);
		return false;
	}
}

bool AudioControl::restartFxAudioInSlot(int slotnum)
{
	if (slotnum < 0 || slotnum >= audioChannels.size()) return false;

	if (audioChannels[slotnum]->status() < AUDIO_INIT) {
		dmx_audio_ctrl_status[slotnum] = DMX_SLOT_UNDEF;
		FxAudioItem *fxa = audioChannels[slotnum]->currentFxAudio();
		AudioCtrlMsg msg(fxa,slotnum, CMD_AUDIO_START);
		emit audioThreadCtrlMsgEmitted(msg);
		return true;
	}
	return false;
}

bool AudioControl::stopAllFxAudio()
{
	bool was_running = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() != AUDIO_IDLE) was_running = true;
		stopFxAudio(t);
	}
	return was_running;
}

void AudioControl::stopFxAudio(int slot)
{
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		// We send this via Message to communicate with the thread
		AudioCtrlMsg msg(slot,CMD_AUDIO_STOP);
		msg.executer = audioChannels[slot]->currentExecuter();
		emit audioThreadCtrlMsgEmitted(msg);
	}
}

void AudioControl::stopFxAudio(FxAudioItem *fxa)
{
	if (!fxa) return;

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentFxAudio() == fxa) {
			dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
			stopFxAudio(t);
		}
	}
}

void AudioControl::storeCurrentSeekPositions()
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		audioChannels[t]->storeCurrentSeekPos();
	}
}

bool AudioControl::fadeoutAllFxAudio(int time_ms)
{
	bool was_running = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() != AUDIO_IDLE) was_running = true;
		fadeoutFxAudio(t,time_ms);
	}
	return was_running;
}

void AudioControl::fadeoutFxAudio(int slot, int time_ms)
{
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;

		// This is a private message to my audio thread
		//		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
		//		msg.fadetime = time_ms;
		//		msg.executer = audioChannels[slot]->currentExecuter();
		//		emit audioThreadCtrlMsgEmitted(msg);

		audioChannels[slot]->fadeoutFxAudio(0,time_ms);
	}
}

void AudioControl::fadeoutFxAudio(FxAudioItem *fxa, int time_ms)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentFxAudio() == fxa) {
			fadeoutFxAudio(t,time_ms);
		}
	}
}

void AudioControl::fadeoutFxAudio(Executer *exec, int time_ms)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentExecuter() == exec) {
			fadeoutFxAudio(t,time_ms);
		}
	}
}

bool AudioControl::seekPosPerMilleFxAudio(int slot, int perMille)
{
	bool seek = false;
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {

		// dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
		// This is a private message to my audio thread
		//		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
		//		msg.fadetime = time_ms;
		//		msg.executer = audioChannels[slot]->currentExecuter();
		// emit audioThreadCtrlMsgEmitted(msg);

		seek = audioChannels[slot]->seekPosPerMille(perMille);
	}
	return seek;
}

bool AudioControl::seekPosPerMilleFxAudio(FxAudioItem *fxa, int perMille)
{
	bool seek = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentFxAudio() == fxa) {
			seek = audioChannels[t]->seekPosPerMille(perMille);
		}
	}
	return seek;
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
	// qDebug("AudioControl::audioCtrlRepeater Ctrl Msg received and forwarded");
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
		audioChannels[msg.slotNumber]->setVolume(msg.volume);
		setVolumeInFx(msg.slotNumber,msg.volume);
		break;
	default:
		DEBUGERROR("AudioControl::audioCtrlReceiver: Unsupported command received: %d",msg.ctrlCmd);
		break;
	}
}

void AudioControl::setMasterVolume(int vol)
{
	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	masterVolume = vol;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		audioChannels[t]->setMasterVolume(vol);
	}
}

void AudioControl::setVolume(int slot, int vol)
{
	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		audioChannels[slot]->setVolume(vol);
		dmx_audio_ctrl_status[slot] = DMX_SLOT_UNDEF;
		dmx_audio_ctrl_last_vol[slot] = vol;
	}
}

void AudioControl::setVolumeInFx(int slot, int vol)
{
	if (vol < 0) {
		vol = 0;
	}
	else if (vol > MAX_VOLUME) {
		vol = MAX_VOLUME;
	}
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		FxItem *fx = audioChannels[slot]->currentFxAudio();
		if (fx && fx->parentFxList() && fx->parentFxList()->parentFx()) {
			FxItem *parentFx = fx->parentFxList()->parentFx();
			if (parentFx->fxType() == FX_AUDIO_PLAYLIST) {
				reinterpret_cast<FxPlayListItem*>(parentFx)->initialVolume = vol;
				qDebug("Set volume in parentFx %s to %d",parentFx->name().toLocal8Bit().data(),vol);
			}
		}
	}
}

int AudioControl::getVolume(int slot) const
{
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		return audioChannels[slot]->volume();
	} else {
		return 0;
	}
}

void AudioControl::setVolumeFromDmxLevel(int slot, int vol)
{
	if (slot < 0 || slot >= MAX_AUDIO_SLOTS) return;

	qDebug("Set Volume slot %d, %d",slot,vol);
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
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		audioChannels[slot]->setVolume(vol);
	}

	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_STATUS_REPORT;
	msg.slotNumber = slot;
	msg.volume = vol;
	emit audioCtrlMsgEmitted(msg);
}


void AudioControl::init()
{
	setObjectName("Audio Control");
	masterVolume = MAX_VOLUME;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		dmx_audio_ctrl_last_vol[t] = 0;
		dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
	}
}

void AudioControl::initFromThread()
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		AudioSlot *slot = new AudioSlot(this,t);
		audioChannels.append(slot);
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,int,int)),this,SLOT(vu_level_changed_receiver(int,int,int)));
		connect(this,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),slot,SLOT(audioCtrlReceiver(AudioCtrlMsg)));

	}
}

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

#include <QStringList>
#include <QDebug>



AudioControl::AudioControl(AppCentral *app_central)
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
	if (!exec && myApp->userSettings->pProhibitAudioDoubleStart) {
		for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
			if (audioChannels[t]->currentFxAudio() == fxa) {
				// Ok, audio is already running -> Check the time
				int cur_run_time = audioChannels[t]->currentRunTime();
				if ( cur_run_time >= 0 && cur_run_time < myApp->userSettings->pAudioAllowReactivateTime) {
					LOGERROR(tr("Audio Fx '%1' not started since it is already running on channel %2")
							 .arg(fxa->name()).arg(t+1));
					return false;
				}
			}
		}
	}

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() < AUDIO_INIT) {
			dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
			AudioCtrlMsg msg(fxa,t, CMD_AUDIO_START,exec);
			emit audioThreadCtrlMsgEmitted(msg);
			ok = true;
			break;
		}
	}

	return ok;
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
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentFxAudio() == fxa) {
			dmx_audio_ctrl_status[t] = DMX_SLOT_UNDEF;
			stopFxAudio(t);
		}
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
		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
		msg.fadetime = time_ms;
		msg.executer = audioChannels[slot]->currentExecuter();
		// emit audioThreadCtrlMsgEmitted(msg);
		audioChannels[slot]->fadeoutFxAudio(time_ms);
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

bool AudioControl::startFxAudioPlayList(FxPlayListItem *fxplay)
{
	if (!FxItem::exists(fxplay)) {
		qDebug("FxPlayListItem not found in pool");
		return false;
	}

	// First create an executer to process the playlist
	FxListExecuter *fxexec = myApp->execCenter->newFxListExecuter(fxplay->fxPlayList);
	fxexec->setOriginFx(fxplay);
	fxexec->setPlayListInitialVolume(fxplay->initialVolume);

	// Maybe there is a FxListWidget window opened. Than we can do some monitoring
	FxListWidget *wid = FxListWidget::findFxListWidget(fxplay->fxPlayList);
	if (wid) {
		connect(fxexec,SIGNAL(fxItemExecuted(FxItem*,Executer*)),wid,SLOT(markFx(FxItem*)));
		connect(fxexec,SIGNAL(nextFxChanged(FxItem*)),wid,SLOT(selectFx(FxItem*)));
		if (wid->currentSelectedFxItem())
			return fxexec->runFxItem(wid->currentSelectedFxItem());
	}

	return fxexec->runExecuter();
}

/**
 * @brief Starts or continues the playback of a FxAudioPlayList
 * @param fxplay A Pointer to the FxPlayListItem instance that contains the playlist
 * @param fxaudio A valid Pointer to the FxAudioItem that should be started
 * @return true, if successful
 *
 */
bool AudioControl::continueFxAudioPlayList(FxPlayListItem *fxplay, FxAudioItem *fxaudio)
{
	// First try to find an existing Executer
	FxListExecuter *cur_exe = myApp->execCenter->findFxListExecuter(fxplay);
	if (cur_exe) {
		cur_exe->fadeEndExecuter();
	}

	FxListExecuter *fxexec = myApp->execCenter->newFxListExecuter(fxplay->fxPlayList);
	fxexec->setOriginFx(fxplay);
	fxexec->setPlayListInitialVolume(fxplay->initialVolume);

	// Maybe there is a FxListWidget window opened. Than we can do some monitoring
	FxListWidget *wid = FxListWidget::findFxListWidget(fxplay->fxPlayList);
	if (wid) {
		connect(fxexec,SIGNAL(fxItemExecuted(FxItem*,Executer*)),wid,SLOT(markFx(FxItem*)));
		connect(fxexec,SIGNAL(nextFxChanged(FxItem*)),wid,SLOT(selectFx(FxItem*)));
	}

	return fxexec->runFxItem(fxaudio);
}

bool AudioControl::stopFxAudioPlayList(FxPlayListItem *fxplay)
{
	/// @implement me


	return true;
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
		AudioSlot *slot = new AudioSlot(this);
		audioChannels.append(slot);
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,int,int)),this,SLOT(vu_level_changed_receiver(int,int,int)));
		connect(this,SIGNAL(audioThreadCtrlMsgEmitted(AudioCtrlMsg)),slot,SLOT(audioCtrlReceiver(AudioCtrlMsg)));

	}
}

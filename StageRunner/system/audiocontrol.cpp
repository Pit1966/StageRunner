#include "audiocontrol.h"
#include "config.h"
#include "log.h"
#include "audiochannel.h"
#include "appcentral.h"

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

bool AudioControl::startFxAudio(FxAudioItem *fxa)
{
	bool ok = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->status() < AUDIO_INIT) {
			AudioCtrlMsg msg(fxa,t, CMD_AUDIO_START);
			emit audioThreadCtrlMsgEmitted(msg);
			ok = true;
			// ok = audioChannels[t]->startFxAudio(fxa);
			break;
		}
	}

	return ok;
}

bool AudioControl::restartFxAudioInSlot(int slotnum)
{
	if (slotnum < 0 || slotnum >= audioChannels.size()) return false;

	if (audioChannels[slotnum]->status() < AUDIO_INIT) {
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
		emit audioThreadCtrlMsgEmitted(msg);
		// audioChannels[slot]->stopFxAudio();
	}
}

void AudioControl::stopFxAudio(FxAudioItem *fxa)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audioChannels[t]->currentFxAudio() == fxa) {
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
		// This is a private message to my audio thread
		AudioCtrlMsg msg(slot,CMD_AUDIO_FADEOUT);
		msg.fadetime = time_ms;
		// emit audioThreadCtrlMsgEmitted(msg);
		audioChannels[slot]->fadeoutFxAudio(time_ms);
	}
}

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
	}
}

void AudioControl::init()
{
	setObjectName("Audio Control");
	masterVolume = MAX_VOLUME;
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

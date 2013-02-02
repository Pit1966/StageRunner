#include "audiocontrol.h"
#include "../config.h"
#include "audiochannel.h"
#include "../system/log.h"


#include <QStringList>
#include <QDebug>



AudioControl::AudioControl()
	: QObject()
{
	init();
	getAudioDevices();
}

AudioControl::~AudioControl()
{
	while (!audio_channels.isEmpty()) {
		delete audio_channels.takeFirst();
	}
}

void AudioControl::getAudioDevices()
{
}

bool AudioControl::startFxAudio(FxAudioItem *fxa)
{
	bool ok = false;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (audio_channels[t]->status() < AUDIO_INIT) {
			ok = audio_channels[t]->startFxAudio(fxa);
			break;
		}
	}

	return ok;
}

void AudioControl::stopAllFxAudio()
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		audio_channels[t]->stopFxAudio();
	}
}

void AudioControl::stopFxAudio(int slot)
{
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		audio_channels[slot]->stopFxAudio();
	}
}

void AudioControl::fadeoutAllFxAudio(int time_ms)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		audio_channels[t]->fadeoutFxAudio(time_ms);
	}
}

void AudioControl::fadeoutFxAudio(int slot, int time_ms)
{
	if (slot >= 0 && slot < MAX_AUDIO_SLOTS) {
		audio_channels[slot]->fadeoutFxAudio(time_ms);
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
		break;
	case CMD_AUDIO_CHANGE_VOL:
		audio_channels[msg.slotNumber]->setVolume(msg.volume);
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
	master_volume = vol;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		audio_channels[t]->setMasterVolume(vol);
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
		audio_channels[slot]->setVolume(vol);
	}
}

void AudioControl::init()
{
	master_volume = MAX_VOLUME;
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		AudioSlot *slot = new AudioSlot(this);
		audio_channels.append(slot);
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,int,int)),this,SIGNAL(vuLevelChanged(int,int,int)));
	}
}

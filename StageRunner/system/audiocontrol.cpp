#include "audiocontrol.h"
#include "config.h"
#include "audiochannel.h"
#include "system/log.h"


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
#ifndef IS_QT5

	QList<Phonon::AudioOutputDevice>audiodevs = Phonon::BackendCapabilities::availableAudioOutputDevices();

	qDebug() << "Audio Devices";
	for (int t=0; t<audiodevs.size(); t++) {
		Phonon::AudioOutputDevice dev = audiodevs[t];

		qDebug() << "valid:" << dev.isValid() << "name:" << dev.name();
		// qDebug() << "props" << dev.propertyNames();
	}


	QList<Phonon::EffectDescription>effects = Phonon::BackendCapabilities::availableAudioEffects();

	qDebug() << "Audio Fx";
	for (int t=0; t<effects.size(); t++) {
		Phonon::EffectDescription fx = effects[t];

		qDebug() << fx.name();
		qDebug() << fx.propertyNames();
		qDebug() << "--------------------------------";
	}

	// QStringList mime = Phonon::BackendCapabilities::availableMimeTypes();
	// qDebug() << mime;

#endif
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

void AudioControl::init()
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		AudioSlot *slot = new AudioSlot;
		audio_channels.append(slot);
		slot->slotNumber = t;
		connect(slot,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));
		connect(slot,SIGNAL(vuLevelChanged(int,int,int)),this,SIGNAL(vuLevelChanged(int,int,int)));
	}
}

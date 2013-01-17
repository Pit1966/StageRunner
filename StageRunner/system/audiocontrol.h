#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"

#include <phonon/BackendCapabilities>
#include <phonon/AudioOutputDevice>
#include <QObject>
#include <QList>

using namespace AUDIO;

class AudioSlot;
class FxAudioItem;

class AudioControl : public QObject
{
	Q_OBJECT
private:
	QList<AudioSlot*> audio_channels;
	QList<Phonon::EffectDescription> available_effects;

public:
	AudioControl();
	~AudioControl();

	void getAudioDevices();

public slots:
	bool startFxAudio(FxAudioItem *fxa);
	void stopAllFxAudio();
	void stopFxAudio(int slot);

	void audioCtrlRepeater(AudioCtrlMsg msg);
	void audioCtrlReceiver(AudioCtrlMsg msg);

private:
	void init();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);

};

#endif // AUDIOCONTROL_H

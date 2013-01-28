#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"

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
	int master_volume;

public:
	AudioControl();
	~AudioControl();

	void getAudioDevices();

public slots:
	bool startFxAudio(FxAudioItem *fxa);
	void stopAllFxAudio();
	void stopFxAudio(int slot);
	void fadeoutAllFxAudio(int time_ms = 5000);
	void fadeoutFxAudio(int slot, int time_ms);

	void audioCtrlRepeater(AudioCtrlMsg msg);
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void setMasterVolume(int vol);
	void setVolume(int slot, int vol);

private:
	void init();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, int left, int right);
};

#endif // AUDIOCONTROL_H

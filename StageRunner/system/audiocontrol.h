#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"

#include <QThread>
#include <QList>

using namespace AUDIO;

class AudioSlot;
class FxAudioItem;
class AppCentral;

class AudioControl : public QThread
{
	Q_OBJECT

public:
	AppCentral *myApp;

protected:
	QList<AudioSlot*> audioChannels;
	int masterVolume;

public:
	AudioControl(AppCentral *app_central);
	~AudioControl();

	void getAudioDevices();

private:
	void run();

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
	void initFromThread();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, int left, int right);
};

#endif // AUDIOCONTROL_H

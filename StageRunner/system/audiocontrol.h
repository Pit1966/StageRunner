#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"
#include "config.h"

#include <QThread>
#include <QList>

using namespace AUDIO;

class AudioSlot;
class FxAudioItem;
class AppCentral;
class FxPlayListItem;

class AudioControl : public QThread
{
	Q_OBJECT

public:
	AppCentral *myApp;

protected:
	QList<AudioSlot*> audioChannels;
	int masterVolume;

private:
	enum {
		DMX_SLOT_UNDEF,
		DMX_SLOT_IS_LOWER,
		DMX_SLOT_IS_HIGHER,
		DMX_SLOT_UP_CATCH,
		DMX_SLOT_DOWN_CATCH
	};
	int dmx_audio_ctrl_status[MAX_AUDIO_SLOTS];
	int dmx_audio_ctrl_last_vol[MAX_AUDIO_SLOTS];

public:
	AudioControl(AppCentral *app_central);
	~AudioControl();

	void getAudioDevices();

private:
	void run();

private slots:
	void vu_level_changed_receiver(int slotnum, int left, int right);

public slots:
	bool startFxAudio(FxAudioItem *fxa);
	bool restartFxAudioInSlot(int slotnum);
	bool stopAllFxAudio();
	void stopFxAudio(int slot);
	void stopFxAudio(FxAudioItem *fxa);
	bool fadeoutAllFxAudio(int time_ms = 5000);
	void fadeoutFxAudio(int slot, int time_ms);
	void fadeoutFxAudio(FxAudioItem *fxa, int time_ms = 5000);

	void audioCtrlRepeater(AudioCtrlMsg msg);
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void setMasterVolume(int vol);
	void setVolume(int slot, int vol);
	int getVolume(int slot) const;
	void setVolumeFromDmxLevel(int slot, int vol);
	bool startFxAudioPlayList(FxPlayListItem *fxplay);
	bool stopFxAudioPlayList(FxPlayListItem *fxplay);


private:
	void init();
	void initFromThread();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void audioThreadCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, int left, int right);
};

#endif // AUDIOCONTROL_H

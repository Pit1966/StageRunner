#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"
#include "config.h"

#include <QThread>
#include <QList>
#include <QMutex>

using namespace AUDIO;

class AudioSlot;
class FxAudioItem;
class AppCentral;
class FxPlayListItem;
class Executer;
class FxControl;
class FrqSpectrum;

class AudioControl : public QThread
{
	Q_OBJECT

public:
	AppCentral &myApp;

protected:
	QList<AudioSlot*> audioSlots;
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
	int used_slots;

	QMutex *slotMutex;


public:
	AudioControl(AppCentral &app_central);
	~AudioControl();

	void getAudioDevices();
	bool isFxAudioActive(FxAudioItem *fxa);
	bool isFxAudioActive(int slotnum);
	int findAudioSlot(FxAudioItem *fxa);
	int selectFreeAudioSlot(int slotnum = -1);
	void setFFTAudioChannelFromMask(qint32 mask);

private:
	void run();
	bool start_fxaudio_in_slot(FxAudioItem *fxa, int slotnum, Executer *exec = 0, qint64 atMs = -1, int initVol = -1);

private slots:
	void vu_level_changed_receiver(int slotnum, qreal left, qreal right);
	void fft_spectrum_changed_receiver(int slotnum, FrqSpectrum *spec);

public slots:
	bool startFxAudio(FxAudioItem *fxa, Executer *exec);
	bool startFxAudioAt(FxAudioItem *fxa, Executer *exec = 0, qint64 atMs = -1, int initVol = -1);
	bool startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec = 0, qint64 atMs = -1, int initVol = -1);
	bool restartFxAudioInSlot(int slotnum);
	int stopAllFxAudio();
	void stopFxAudio(int slot);
	void stopFxAudio(FxAudioItem *fxa);
	void storeCurrentSeekPositions();
	void storeCurrentSeekPos(int slot);
	int fadeoutAllFxAudio(int time_ms = 5000);
	void fadeoutFxAudio(int slot, int time_ms);
	void fadeoutFxAudio(FxAudioItem *fxa, int time_ms = 5000);
	void fadeoutFxAudio(Executer *exec, int time_ms = 5000);
	bool seekPosPerMilleFxAudio(int slot, int perMille);
	bool seekPosPerMilleFxAudio(FxAudioItem *fxa, int perMille);

	void audioCtrlRepeater(AudioCtrlMsg msg);
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void setMasterVolume(int vol);
	void setVolume(int slot, int vol);
	void setVolumeInFx(int slot, int vol, bool setAsInitVolume);
	int getVolume(int slot) const;
	void setVolumeByDmxInput(int slot, int vol);
	void setVolumeFromDmxLevel(int slot, int vol);

	bool handleDmxInputAudioEvent(FxAudioItem *fxa, uchar value);

private:
	void init();
	void initFromThread();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void audioThreadCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void fftSpectrumChanged(int slotnum, FrqSpectrum * spec);
};

#endif // AUDIOCONTROL_H

#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"
#include "config.h"

#include <QThread>
#include <QList>
#include <QMutex>
#include <QAudioDeviceInfo>

using namespace AUDIO;

class AudioSlot;
class AudioPlayer;
class FxAudioItem;
class AppCentral;
class FxPlayListItem;
class Executer;
class FxControl;
class FrqSpectrum;
class QMediaPlaylist;
class VideoPlayer;
class PsVideoWidget;
class FxClipItem;

class AudioControl : public QThread
{
	Q_OBJECT

public:
	AppCentral &myApp;

protected:
	QList<AudioSlot*> audioSlots;
	int masterVolume;
	bool m_isValid;
	bool m_initInThread;
	QAudioDeviceInfo m_extraDevice;
	QAudioDeviceInfo m_audioDevInfos[MAX_AUDIO_SLOTS];
	QStringList m_audioDeviceNames;						///< A list of audio devices by names

	// Video player stuff (as hyper extension)
	VideoPlayer *m_videoPlayer;
	QMediaPlaylist *m_playlist;
	PsVideoWidget *m_videoWid;
	int m_videoPlayerCurrentVolume;

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
	AudioControl(AppCentral &app_central, bool initInThread);
	~AudioControl();

	void reCreateMediaPlayerInstances();

	void getAudioDevices();
	QAudioDeviceInfo getAudioDeviceInfo(const QString &devName, bool *found = nullptr);
	bool isFxAudioActive(FxAudioItem *fxa);
	bool isFxAudioActive(int slotnum);
	int findAudioSlot(FxAudioItem *fxa);
	int selectFreeAudioSlot(int slotnum = -1);
	void setFFTAudioChannelFromMask(qint32 mask);
	inline bool isValid() const {return m_isValid;}
	int usedSlots() const {return audioSlots.size();}
	AudioSlot* audioSlot(int i) const {return audioSlots.at(i);}
	AudioPlayer* audioPlayer(int i) const;

	bool startFxClip(FxClipItem *fxc);
	inline PsVideoWidget * videoWidget() const {return m_videoWid;}
	inline VideoPlayer * videoPlayer() const {return m_videoPlayer;}
	void setVideoPlayerVolume(int vol);
	int evaluateCurrentVolumeForFxAudio(FxAudioItem *fxa);

	inline const QAudioDeviceInfo & extraAudioDevice() const {return m_extraDevice;}
	inline const QStringList & audioDeviceNames() const {return m_audioDeviceNames;}

private:
	void run();
	bool start_fxaudio_in_slot(FxAudioItem *fxa, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1);
#ifdef USE_SDL
	static void sdlChannelDone(int chan);
	static void sdlPostMix(void *udata, quint8 *stream, int len);
#endif

private slots:
	void vu_level_changed_receiver(int slotnum, qreal left, qreal right);
	void fft_spectrum_changed_receiver(int slotnum, FrqSpectrum *spec);

public slots:
	bool startFxAudio(FxAudioItem *fxa, Executer *exec);
	bool startFxAudioAt(FxAudioItem *fxa, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1);
	bool startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1);
	bool restartFxAudioInSlot(int slotnum);
	int stopAllFxAudio();
	void pauseFxAudio(int slot);
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
	bool executeAttachedAudioStartCmd(FxAudioItem *fxa);
	bool executeAttachedAudioStopCmd(FxAudioItem *fxa);


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
	void createMediaPlayInstances();
	void destroyMediaPlayInstances();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void audioThreadCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void fftSpectrumChanged(int slotnum, FrqSpectrum * spec);
};

#endif // AUDIOCONTROL_H

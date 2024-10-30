//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "commandsystem.h"
#include "config.h"
#include "qt_versions.h"

#include <QThread>
#include <QList>
#include <QMutex>
#ifdef IS_QT6
#  include <QAudioDevice>
#else
#  include <QAudioDeviceInfo>
#endif

//using namespace AUDIO;

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
class FxItem;
class AudioWorker;

class AudioControl : public QObject
{
	Q_OBJECT

public:
	AppCentral &myApp;

protected:
	QList<AudioSlot*> audioSlots;
	int m_masterVolume			= 0;
	volatile bool m_isValid		= false;
	bool m_initInThread			= false;
	bool m_isInThread			= false;
	bool m_isSmallAudioBufFix	= false;
#ifdef IS_QT6
	QAudioDevice m_extraDevice;
	QAudioDevice m_audioDevInfos[MAX_AUDIO_SLOTS];
#else
	QAudioDeviceInfo m_extraDevice;
	QAudioDeviceInfo m_audioDevInfos[MAX_AUDIO_SLOTS];
#endif
	QStringList m_audioDeviceNames;							///< A list of audio devices by names

	// Video player stuff (as hyper extension)
	VideoPlayer *m_videoPlayer		= nullptr;
	PsVideoWidget *m_videoWid		= nullptr;

	// audio control (threaded)
	AudioWorker *m_audioWorker		= nullptr;
	QThread m_audioThread;

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
	int m_usedSlots		= 0;

	QRecursiveMutex *slotMutex;

public:
	AudioControl(AppCentral &app_central, bool initInThread);
	~AudioControl();

	bool initAudio(bool initInThread);
	bool createAudioWorker();
	AudioWorker *audioWorker();
	QThread *audioThread();

	void reCreateMediaPlayerInstances();
	void setAudioInThreadEnabled(bool state);
	void setSmallAudioBufFix(bool state);
	bool isSmallAudioBufFix() const {return m_isSmallAudioBufFix;}
	bool checkAudioLimits();

	void getAudioDevices();
	bool isFxAudioActive(FxAudioItem *fxa);
	bool isFxAudioActive(int slotnum);
	bool isAnyFxAudioActive() const;
	int findAudioSlot(FxAudioItem *fxa);
	int selectFreeAudioSlot(int slotnum = -1);
	int selectFreeVideoSlot(bool *isVideoRunning);
	bool selectVideoSlot(int slotnum);
	void setFFTAudioChannelFromMask(qint32 mask);
	inline bool isValid() const {return m_isValid;}
	int usedSlots() const {return audioSlots.size();}
	AudioSlot* audioSlot(int i) const {return audioSlots.at(i);}
	AudioPlayer* audioPlayer(int i) const;
	int getVolume(int slot) const;
	inline int masterVolume() const {return m_masterVolume;}
	inline int maxVolume() const {return MAX_VOLUME;}

	inline PsVideoWidget * videoWidget() const {return m_videoWid;}
	inline VideoPlayer * videoPlayer() const {return m_videoPlayer;}
	int evaluateCurrentVolumeForFxAudio(FxAudioItem *fxa);
	int getSlotForFxAudio(FxAudioItem *fxa);
	void closeVideoWidget();
	bool isVideoWidgetVisible(QWidget **videoWid = nullptr) const;

	inline const QStringList & audioDeviceNames() const {return m_audioDeviceNames;}
	static QString audioOutTypeToString(AUDIO::AudioOutputType type);
	static bool isAudioOutAvailable(AUDIO::AudioOutputType type);
	static AUDIO::AudioOutputType defaultAudioOut();

private:
	// void run();
	bool _startFxAudioStage2(FxAudioItem *fxa, Executer *exec, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);
	bool _startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);
#ifdef USE_SDL
	static void sdlChannelDone(int chan);
	static void sdlPostMix(void *udata, quint8 *stream, int len);
#endif

private slots:
	void _vuLevelChangedReceiver(int slotnum, qreal left, qreal right);
	void _fftSpectrumChangedReceiver(int slotnum, FrqSpectrum *spec);
	void _audioWorkerFinished();

public slots:
	bool startFxAudio(FxAudioItem *fxa, Executer *exec, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);
	bool startFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1, int fadeInMs = -1);
	bool startFxClipItemInSlot(FxClipItem *fxc, int slotnum, Executer *exec = nullptr, qint64 atMs = -1, int initVol = -1);
	bool restartFxAudioInSlot(int slotnum);
	int stopAllFxAudio(bool waitForEnd = false);
	bool stopFxAudioWithID(int fxID);

	void pauseFxAudio(int slot);
	void stopFxAudio(int slot);
	void stopFxAudio(FxAudioItem *fxa);
	void storeCurrentSeekPositions();
	void storeCurrentSeekPos(int slot);
	void fadeVolTo(int slot, int targetVolume, int time_ms);
	int fadeoutAllFxAudio(int time_ms = 5000);
	void fadeoutFxAudio(int slot, int time_ms);
	void fadeoutFxAudio(FxAudioItem *fxa, int time_ms = 5000);
	void fadeoutFxAudio(Executer *exec, int time_ms = 5000);
	void fadeinFxAudio(int slot, int targetVolume, int time_ms = 5000);
	bool seekPosPerMilleFxAudio(int slot, int perMille);
	bool seekPosPerMilleFxAudio(FxAudioItem *fxa, int perMille);
	bool executeAttachedAudioStartCmd(FxAudioItem *fxa);
	bool executeAttachedAudioStopCmd(FxAudioItem *fxa);

	void audioCtrlRepeater(AUDIO::AudioCtrlMsg msg);
	void audioCtrlReceiver(AUDIO::AudioCtrlMsg msg);
	void setMasterVolume(int vol);
	void setVolume(int slot, int vol);
	void setVolumeFromTimeLine(int slot, int vol);
	void setVolumeInFx(int slot, int vol, bool setAsInitVolume);
	void setVolumeByDmxInput(int slot, int vol);
	void setVolumeFromDmxLevel(int slot, int vol);
	void setDmxVolumeToLocked(int slot);
	void setPanning(int slot, int pan);

	bool handleDmxInputAudioEvent(FxAudioItem *fxa, uchar value);

	VideoPlayer *createVideoPlayer();

private:
	void initDefaults();
	void createMediaPlayInstances();
	void createVideoWidget();
	void destroyMediaPlayInstances();
	void destroyVideoWidget();

signals:
	void audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg msg);
	void audioThreadCtrlMsgEmitted(AUDIO::AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void fftSpectrumChanged(int slotnum, FrqSpectrum * spec);
	void masterVolumeChanged(int vol);
	void mediaPlayerInstancesCreated(const QString &status);

	friend class AudioWorker;
};

#endif // AUDIOCONTROL_H

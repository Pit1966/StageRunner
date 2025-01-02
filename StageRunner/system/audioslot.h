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

#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"
#include "psmovingaverage.h"

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QFile>
#include <QTimeLine>
#include <QAudioFormat>
#include <QPointer>

#ifdef USE_SDL
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_mixer.h>
#endif

using namespace AUDIO;

class FxAudioItem;
class AudioIODevice;
class AudioControl;
class Executer;
class AudioPlayer;
class FFTRealWrapper;
class FrqSpectrum;
class FxClipItem;


class AudioSlot : public QObject
{
	friend class AudioPlayer;
	friend class MediaPlayerAudioBackend;

	Q_OBJECT
public:
	int slotNumber;

private:
	AudioControl *m_audioCtrlUnit;
	AudioPlayer *m_audioPlayer;				///< base class interface for audio implementation

	AudioStatus m_runStatus;
	QPointer<FxAudioItem> m_currentFx;				///< The current/last Fx loaded into this audio slot
	Executer *m_currentExecuter;			///< A Pointer to an Executor if the sound was started by one

	QTimer m_volumeSetHelpTimer;			///< help timer. Used to define an interval for the log output, when volume is changed manualy
	QString m_volumeSetMsg;
	QElapsedTimer m_audioRunTime;
	QTimeLine m_fadeHelpTimeLine;
	AudioFadeMode m_fadeModeAudio;
	int m_fadeVolumeInitial;				///< The Volume the fadeout/in starts with
	int m_fadeVolumeTarget;
	int m_currentVolume;					///< Volume the audio slot is set to
	int m_masterVolume;						///< This is Master Volume
	bool m_isFFTEnabled;

	DMXLockingMode m_dmxVolLockState;		///< this is a helper for audio fx started by dmx signal. It tracks if dmx input is synchronized with audio volume

	QString m_lastErrorText;
	AudioErrorType m_lastAudioError;

public:
	AudioSlot(AudioControl *parent, int pSlotNumber, AUDIO::AudioOutputType audioEngineType, const QString &devName);
	~AudioSlot();

	bool select();
	void unselect();
	bool startFxAudio(const AUDIO::AudioCtrlMsg &msg);
	bool startFxAudio(FxAudioItem * fxa, Executer *exec, qint64 startPosMs = 0, int initVol = -1, int fadeInMs = -1, int pan = -1);
	bool stopFxAudio();
	bool pauseFxAudio(bool state);
	bool fadeoutFxAudio(int targetVolume, int time_ms, bool fadeToMode = false);
	bool fadeinFxAudio(int targetVolume, int time_ms, bool fadeToMode = false);
	void setVolume(int vol);
	void setVolumeFromTimeLine(int vol);
	bool setVolumeFromDMX(int dmxvol);
	void setDmxVolLockState(DMXLockingMode state) {m_dmxVolLockState = state;}
	inline int volume() {return m_currentVolume;}
	void setMasterVolume(int vol);
	void setPanning(int pan);
	FxAudioItem *currentFxAudio();
	Executer * currentExecuter();
	int currentRunTime() const;
	bool seekPosMs(qint64 ms);
	bool seekPosPerMille(int perMille);
	qint64 currentPlayPosMs() const;
	void storeCurrentSeekPos();
	inline bool isActive() const {return m_runStatus == AUDIO_RUNNING || m_runStatus == AUDIO_PAUSED;}
	inline bool isPaused() const {return m_runStatus == AUDIO_PAUSED;}
	inline AudioStatus status() {return m_runStatus;}
	int audioOutputBufferSize() const;
	void setFFTEnabled(bool state);
	inline bool isFFTEnabled() const {return m_isFFTEnabled;}
	QString currentFxName() const;

	inline AudioErrorType lastAudioError() const {return m_lastAudioError;}
	AudioPlayer * audioPlayer() const {return m_audioPlayer;}

	void sdlEmitProgress();
	bool selectFxClipVideo();
	void startFxClipVideoControls(FxAudioItem *fx, Executer *exec);
	void setFxClipVideoCtrlStatus(AudioStatus stat);
	bool fadeoutFxClip(int targetVolume, int time_ms);


private:
	void _setVolume(int vol);
	void _setPanning(int pan);
	void emit_audio_play_progress();

private slots:
	// new base class concept slots
	void onPlayerStatusChanged(AUDIO::AudioStatus status);

	void on_vulevel_changed(qreal left, qreal right);
	void on_frqSpectrum_changed(FrqSpectrum *spec);
	void on_fade_frame_changed(qreal value);
	void on_fade_finished();
	void on_volset_timer_finished();

public slots:
	void audioCtrlReceiver(const AUDIO::AudioCtrlMsg &msg);
	void setAudioDurationMs(qint64 ms);

signals:
	void audioCtrlMsgEmitted(const AUDIO::AudioCtrlMsg &msg);
	void audioProgressChanged(int slotnum, FxAudioItem *fxaudio, int perMille);			///< this is not used for now!!
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void frqSpectrumChanged(int slotnum, FrqSpectrum *spec);
};

#endif // AUDIOCHANNEL_H

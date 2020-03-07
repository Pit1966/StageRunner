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
	AudioControl *audio_ctrl;

//	AudioIODevice *audio_io;
//	QAudioOutput *audio_output;

	AudioPlayer *audio_player;				///< base class interface for audio implementation

	AudioStatus run_status;
	FxAudioItem *current_fx;				///< The current/last Fx loaded into this audio slot
	Executer *current_executer;				///< A Pointer to an Executor if the sound was started by one

	QTimer volset_timer;
	QString volset_text;
	QElapsedTimer run_time;
	QTimeLine fade_timeline;
	AudioFadeMode fade_mode;
	int fade_initial_vol;							///< The Volume the fadeout/in starts with
	int fade_target_vol;
	int current_volume;								///< Volume the audio slot is set to
	int master_volume;								///< This is Master Volume

	bool m_isFFTEnabled;

	QString m_lastErrorText;
	AudioErrorType m_lastAudioError;

public:
	AudioSlot(AudioControl *parent, int pSlotNumber, AUDIO::AudioOutputType audioEngineType, const QString &devName);
	~AudioSlot();

	bool select();
	void unselect();
	bool startFxAudio(FxAudioItem * fxa, Executer *exec, qint64 startPosMs = 0, int initVol = -1);
	bool stopFxAudio();
	bool pauseFxAudio(bool state);
	bool fadeoutFxAudio(int targetVolume, int time_ms);
	bool fadeinFxAudio(int targetVolume, int time_ms);
	void setVolume(int vol);
	inline int volume() {return current_volume;}
	void setMasterVolume(int vol);
	FxAudioItem *currentFxAudio();
	Executer * currentExecuter();
	int currentRunTime() const;
	bool seekPosMs(qint64 ms);
	bool seekPosPerMille(int perMille);
	qint64 currentPlayPosMs() const;
	void storeCurrentSeekPos();
	inline bool isActive() const {return run_status == AUDIO_RUNNING || run_status == AUDIO_PAUSED;}
	inline bool isPaused() const {return run_status == AUDIO_PAUSED;}
	inline AudioStatus status() {return run_status;}
	int audioOutputBufferSize() const;
	void setFFTEnabled(bool state);
	inline bool isFFTEnabled() const {return m_isFFTEnabled;}
	QString currentFxName() const;

	inline AudioErrorType lastAudioError() const {return m_lastAudioError;}
	AudioPlayer * audioPlayer() const {return audio_player;}

	void sdlEmitProgress();
	bool selectFxClipVideo();
	void startFxClipVideoControls(FxAudioItem *fx, Executer *exec);
	void setFxClipVideoCtrlStatus(AudioStatus stat);

private:
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
	void audioCtrlReceiver(const AudioCtrlMsg &msg);
	void setAudioDurationMs(qint64 ms);

signals:
	void audioCtrlMsgEmitted(const AudioCtrlMsg &msg);
	void audioProgressChanged(int slotnum, FxAudioItem *fxaudio, int perMille);			///< this is not used for now!!
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void frqSpectrumChanged(int slotnum, FrqSpectrum *spec);
};

#endif // AUDIOCHANNEL_H

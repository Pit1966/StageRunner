#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"
#include "psmovingaverage.h"

#ifdef IS_QT5
#include <QAudioDecoder>
#include <QMediaPlayer>
#endif

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QAudioOutput>
#include <QFile>
#include <QTimeLine>

#ifdef USE_SDL
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_mixer.h>
#  define SDL_MAX_VOLUME MIX_MAX_VOLUME
#else
#  define SDL_MAX_VOLUME 128
#endif

using namespace AUDIO;

class FxAudioItem;
class AudioIODevice;
class AudioControl;
class Executer;
class AudioPlayer;
class FFTRealWrapper;
class FrqSpectrum;


class AudioSlot : public QObject
{
	friend class AudioPlayer;
	friend class MediaPlayerAudioBackend;

	Q_OBJECT
public:
	int slotNumber;

private:
	AudioControl *audio_ctrl;

	AudioIODevice *audio_io;
	QAudioOutput *audio_output;

	AudioPlayer *audio_player;				///< base class interface for audio implementation
	QFile *audio_file;

	AudioStatus run_status;
	FxAudioItem *current_fx;						///< The current/last Fx loaded into this audio slot
	Executer *current_executer;				///< A Pointer to an Executor if the sound was started by one

	QTimer volset_timer;
	QString volset_text;
	QTime run_time;
	QTimeLine fade_timeline;
	AudioFadeMode fade_mode;
	int fade_initial_vol;							///< The Volume the fadeout/in starts with
	int fade_target_vol;
	int current_volume;								///< Volume the audio slot is set to
	int master_volume;								///< This is Master Volume

	bool m_isQMediaPlayerAudio;
	bool m_isSDLAudio;
	bool m_isFFTEnabled;

	QString m_lastErrorText;
	AudioErrorType m_lastAudioError;
#ifdef USE_SDL
	QAudioFormat m_sdlAudioFormat;
	Mix_Chunk *m_sdlChunk;
	Mix_Chunk m_sdlChunkCopy;
#endif

public:
	AudioSlot(AudioControl *parent, int pSlotNumber, const QString &devName);
	~AudioSlot();

	bool select();
	void unselect();
	bool startFxAudio(FxAudioItem * fxa, Executer *exec, qint64 startPosMs = 0, int initVol = -1);
	bool stopFxAudio();
	bool pauseFxAudio(bool state);
	bool fadeoutFxAudio(int targetVolume, int time_ms);
	bool fadeinFxAudio(int targetVolume, int time_ms);
	void setVolume(qreal vol);
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

#ifdef USE_SDL
	bool sdlStartFxAudio(FxAudioItem * fxa, Executer *exec, qint64 startPosMs = 0, int initVol = -1);
	bool sdlStopFxAudio();
	void sdlSetFinished();
	void sdlSetRunStatus(AudioStatus state);
private:
	void sdlChannelProcessStream(void *stream, int len, void *udata);
	static void sdlChannelProcessor(int chan, void *stream, int len, void *udata);
	static void sdlChannelProcessorFxDone(int chan, void *udata);
#endif

private:
	void emit_audio_play_progress();

private slots:
	// legacy audio slots
	void on_audio_output_status_changed(QAudio::State state);
	void on_audio_io_read_ready();
	// qmediaplayer audio slots
//	void on_media_status_changed(QMediaPlayer::MediaStatus status);
//	void on_media_playstate_changed(QMediaPlayer::State state);

	// new base class concept slots
	void onPlayerStatusChanged(AUDIO::AudioStatus status);


	void on_vulevel_changed(qreal left, qreal right);
	void on_frqSpectrum_changed(FrqSpectrum *spec);
	void on_fade_frame_changed(qreal value);
	void on_fade_finished();
	void on_volset_timer_finished();

public slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void setAudioDurationMs(qint64 ms);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void audioProgressChanged(int slotnum, FxAudioItem *fxaudio, int perMille);
	void vuLevelChanged(int slotnum, qreal left, qreal right);
	void frqSpectrumChanged(int slotnum, FrqSpectrum *spec);
};

#endif // AUDIOCHANNEL_H

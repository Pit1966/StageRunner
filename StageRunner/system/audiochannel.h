#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"

#ifdef IS_QT5
#include <QAudioDecoder>
#endif

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QAudioOutput>
#include <QFile>
#include <QTimeLine>
#include <QMediaPlayer>

using namespace AUDIO;

class FxAudioItem;
class AudioIODevice;
class AudioControl;
class Executer;
class AudioPlayer;


class AudioSlot : public QObject
{
	friend class AudioPlayer;

	Q_OBJECT
public:
	int slotNumber;

private:
	AudioControl *audio_ctrl;
	AudioIODevice *audio_io;
	QAudioOutput *audio_output;
	AudioPlayer *audio_player;
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

	bool is_experimental_audio_f;

public:
	AudioSlot(AudioControl *parent, int pSlotNumber);
	~AudioSlot();

	bool startFxAudio(FxAudioItem * fxa, Executer *exec, qint64 startPosMs = 0, int initVol = -1);
	bool stopFxAudio();
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

	inline AudioStatus status() {return run_status;}

private:
	void emit_audio_play_progress();

private slots:
	// legacy audio slots
	void on_audio_output_status_changed(QAudio::State state);
	void on_audio_io_read_ready();
	// qmediaplayer audio slots
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_media_playstate_changed(QMediaPlayer::State state);


	void on_vulevel_changed(int left, int right);
	void on_fade_frame_changed(qreal value);
	void on_fade_finished();
	void on_volset_timer_finished();

public slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void setAudioDurationMs(qint64 ms);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void audioProgressChanged(int slotnum, FxAudioItem *fxaudio, int perMille);
	void vuLevelChanged(int slotnum, int left, int right);
};

#endif // AUDIOCHANNEL_H

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

using namespace AUDIO;

class FxAudioItem;
class AudioIODevice;
class AudioControl;


class AudioSlot : public QObject
{
	Q_OBJECT
public:
	int slotNumber;

private:
	AudioControl *audio_ctrl;
	AudioIODevice *audio_io;
	QAudioOutput *audio_output;
	QFile *audio_file;

	AudioStatus run_status;
	FxAudioItem *current_fx;						///< The current/last Fx loaded into this audio slot


	QTimer volset_timer;
	QString volset_text;
	QTime run_time;
	QTimeLine fadeout_timeline;
	int fade_out_initial_vol;						///< The Volume the fadeout starts with
	int current_volume;								///< Volume the audio slot is set to
	int master_volume;								///< This is Master Volume

public:
	AudioSlot(AudioControl *parent);
	~AudioSlot();

	bool startFxAudio(FxAudioItem * fxa);
	bool stopFxAudio();
	bool fadeoutFxAudio(int time_ms);
	void setVolume(int vol);
	inline int volume() {return current_volume;}
	void setMasterVolume(int vol);
	FxAudioItem *currentFxAudio() {return current_fx;}

	inline AudioStatus status() {return run_status;}

private slots:
	void on_audio_output_status_changed(QAudio::State state);
	void on_audio_io_read_ready();
	void on_vulevel_changed(int left, int right);
	void on_fade_out_frame_changed(qreal value);
	void on_fade_out_finished();
	void on_volset_timer_finished();

public slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, int left, int right);

};

#endif // AUDIOCHANNEL_H

#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"

#ifdef IS_QT5
#include <QAudioDecoder>
#endif

#include <QObject>
#include <QTime>
#include <QAudioOutput>
#include <QFile>

using namespace AUDIO;

class FxAudioItem;
class AudioIODevice;


class AudioSlot : public QObject
{
	Q_OBJECT
public:
	int slotNumber;

private:
	AudioIODevice *audio_io;
	QAudioOutput *audio_output;
	QFile *audio_file;

	volatile AudioStatus run_status;

	QTime run_time;
	bool probe_init_f;

public:
	AudioSlot();
	~AudioSlot();

	bool startFxAudio(FxAudioItem * fxa);
	bool stopFxAudio();
	void setVolume(qint32 vol);

	inline AudioStatus status() {return run_status;}

private slots:
	void on_audio_output_status_changed(QAudio::State state);
	void on_audio_io_read_ready();
	void on_vulevel_changed(int left, int right);


signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void vuLevelChanged(int slotnum, int left, int right);

};

#endif // AUDIOCHANNEL_H

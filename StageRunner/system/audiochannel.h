#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"

#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/AudioOutput>
#include <QObject>
#include <QTime>

using namespace AUDIO;

class FxAudioItem;


class AudioSlot : public QObject
{
	Q_OBJECT
public:
	int slotNumber;

private:
	Phonon::MediaObject *media_obj;
	Phonon::AudioOutput *audio_out;
	Phonon::Path path;
	volatile AudioStatus run_status;

	QTime run_time;

public:
	AudioSlot();
	~AudioSlot();

	bool startFxAudio(FxAudioItem * fxa);
	bool stopFxAudio();
	inline AudioStatus status() {return run_status;}
	inline Phonon::AudioOutput *audioSink() {return audio_out;}

protected slots:
	void setAudioState(Phonon::State newstate, Phonon::State oldstate);
	void setFinished();

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);

};

#endif // AUDIOCHANNEL_H

#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "commandsystem.h"

#ifndef IS_QT5
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/AudioOutput>
#else
#include <QMediaPlayer>
#endif

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
#ifndef IS_QT5
	Phonon::MediaObject *media_obj;
	Phonon::AudioOutput *audio_out;
	Phonon::Path path;
#else
	QMediaPlayer * media_player;

#endif
	volatile AudioStatus run_status;

	QTime run_time;

public:
	AudioSlot();
	~AudioSlot();

	bool startFxAudio(FxAudioItem * fxa);
	bool stopFxAudio();
	inline AudioStatus status() {return run_status;}
#ifndef IS_QT5
	inline Phonon::AudioOutput *audioSink() {return audio_out;}
#endif

protected slots:
#ifndef IS_QT5
	void setPhononAudioState(Phonon::State newstate, Phonon::State oldstate);
	void setPhononFinished();
#else
	void setQtAudioState(QMediaPlayer::State state);
#endif

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);

};

#endif // AUDIOCHANNEL_H

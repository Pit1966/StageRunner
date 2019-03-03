#ifndef IODEVICEAUDIOBACKEND_H
#define IODEVICEAUDIOBACKEND_H

#include "system/audioplayer.h"
#include "commandsystem.h"

#include <QObject>
#include <QAudio>

class AudioIODevice;
class QAudioOutput;

class IODeviceAudioBackend : public AudioPlayer
{
	Q_OBJECT
private:
	AudioIODevice *m_audioIODev;
	QAudioOutput *m_audioOutput;
	QAudio::State m_currentOutputState;
	AUDIO::AudioStatus m_currentAudioStatus;			///< This is AudioIODevice state and output state translated to StageRunner audio state


public:
	IODeviceAudioBackend(AudioSlot &audioChannel, const QString &devName);
	~IODeviceAudioBackend() override;
	AUDIO::AudioOutputType outputType() const override {return AUDIO::OUT_DEVICE;}

	bool setSourceFilename(const QString &path) override;
	void start(int loops) override;
	void stop() override;
	void pause(bool state) override;
	qint64 currentPlayPosUs() const override;
	qint64 currentPlayPosMs() const override;
	bool seekPlayPosMs(qint64 posMs) override;
	void setVolume(int vol, int maxvol) override;
	int volume() const override;
	AUDIO::AudioStatus state() const override;

	void setAudioBufferSize(int bytes) override;
	int audioBufferSize() const override;


private slots:
	void onAudioOutputStatusChanged(QAudio::State state);
	void onAudioIODevReadReady();
	void onMediaDurationDetected(qint64 ms);

};

#endif // IODEVICEAUDIOBACKEND_H

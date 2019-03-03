#ifndef SDL2AUDIOBACKEND_H
#define SDL2AUDIOBACKEND_H

#include "audioplayer.h"
#include "commandsystem.h"

#include <QTime>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


#define SDL_MAX_VOLUME MIX_MAX_VOLUME

class SDL2AudioBackend : public AudioPlayer
{
	Q_OBJECT
private:
	QAudioFormat m_sdlAudioFormat;
	Mix_Chunk *m_sdlChunk;
	Mix_Chunk m_sdlChunkCopy;
	AUDIO::AudioStatus m_currentStatus;

	QTime m_runtime;

public:
	SDL2AudioBackend(AudioSlot &audioSlot);
	~SDL2AudioBackend() override;

	AUDIO::AudioOutputType outputType() const override {return AUDIO::OUT_SDL2;}

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

	// SDL Callbacks
	static void sdlChannelDone(int chan);
	static void sdlPostMix(void *udata, quint8 *stream, int len);

protected:
	void sdlSetRunStatus(AUDIO::AudioStatus state);

private:
	void sdlChannelProcessStream(void *stream, int len, void *udata);
	void sdlSetFinished();

	// SDL callbacks
	static void sdlChannelProcessor(int chan, void *stream, int len, void *udata);
	static void sdlChannelProcessorFxDone(int chan, void *udata);

};

#endif // SDL2AUDIOBACKEND_H

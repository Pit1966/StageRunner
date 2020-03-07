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

#ifndef SDL2AUDIOBACKEND_H
#define SDL2AUDIOBACKEND_H

#include "audioplayer.h"
#include "commandsystem.h"

#include <QElapsedTimer>

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

	QElapsedTimer m_runtime;

public:
	SDL2AudioBackend(AudioSlot &audioSlot);
	~SDL2AudioBackend() override;

	AUDIO::AudioOutputType outputType() const override {return AUDIO::OUT_SDL2;}

	bool setSourceFilename(const QString &path, const QString &fxName) override;
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

	void delayedStartEvent() override;

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

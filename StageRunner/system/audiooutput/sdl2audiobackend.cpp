//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include "sdl2audiobackend.h"
#include "log.h"
#include "audioslot.h"
#include "appcentral.h"
#include "audiocontrol.h"

#include <QDebug>

using namespace AUDIO;

SDL2AudioBackend::SDL2AudioBackend(AudioSlot &audioSlot)
	: AudioPlayer(audioSlot)
	, m_sdlChunk(nullptr)
	, m_currentStatus(AUDIO_NO_STATE)
{
	m_sdlAudioFormat.setChannelCount(2);
	m_sdlAudioFormat.setSampleFormat(AudioFormat::Int16);
	m_sdlAudioFormat.setSampleRate(44100);

	// m_sdlAudioFormat.setSampleSize(16);
	// m_sdlAudioFormat.setSampleType(QAudioFormat::SignedInt);
}

SDL2AudioBackend::~SDL2AudioBackend()
{

}

bool SDL2AudioBackend::setSourceFilename(const QString &path, const QString &fxName)
{
	if (m_sdlChunk) {
		*m_sdlChunk = m_sdlChunkCopy;
		Mix_FreeChunk(m_sdlChunk);
		m_sdlChunkCopy.alen = 0;
	}

	setFxName(fxName);

	// Load audio data
	m_sdlChunk = Mix_LoadWAV(path.toLocal8Bit().data());	// 1 means, automatically free the source
	if (m_sdlChunk) {
		m_sdlChunkCopy = *m_sdlChunk;
		qDebug() << "Loaded SDL sound:" << path;

		qint64 durationMs = qint64(m_sdlChunk->alen) * 1000 / (44100 * 4);
		m_parentAudioSlot.setAudioDurationMs(durationMs);

	} else {
		LOGERROR(tr("Could not load SDL file: %1").arg(path));
		sdlSetRunStatus(AUDIO_ERROR);
		return false;
	}

	return true;
}

void SDL2AudioBackend::start(int loops)
{
	Mix_RegisterEffect(m_parentAudioSlot.slotNumber, SDL2AudioBackend::sdlChannelProcessor, nullptr, nullptr);
	// Play audio data
	int res = Mix_PlayChannel(m_parentAudioSlot.slotNumber, m_sdlChunk, loops);
	if (res == -1) {
		LOGERROR(tr("SDL error: %1").arg(Mix_GetError()));
		sdlSetRunStatus(AUDIO_ERROR);
	} else {
		if (m_startDelayedTimerId == 0) {
			sdlSetRunStatus(AUDIO_RUNNING);
			m_runtime.start();
		} else {
			sdlSetRunStatus(AUDIO_PAUSED);
			pause(true);
		}
	}
}

void SDL2AudioBackend::stop()
{
	Mix_HaltChannel(m_parentAudioSlot.slotNumber);
}

void SDL2AudioBackend::pause(bool state)
{
	int c = m_parentAudioSlot.slotNumber;

	if (state) {
		if (Mix_Playing(c)) {
			Mix_Pause(c);
			sdlSetRunStatus(AUDIO_PAUSED);
		}
	} else {
		if (Mix_Paused(c)) {
			Mix_Resume(c);
			sdlSetRunStatus(AUDIO_RUNNING);
		}
	}
}

qint64 SDL2AudioBackend::currentPlayPosUs() const
{
	return qint64(m_runtime.elapsed()) * 1000;
}

qint64 SDL2AudioBackend::currentPlayPosMs() const
{
	return qint64(m_runtime.elapsed());
}

bool SDL2AudioBackend::seekPlayPosMs(qint64 posMs)
{
	if (!m_sdlChunk)
		return false;

	qDebug() << "seek" << posMs;
	uint seekpos = uint( (44100 * posMs / 1000) * 4 );
	if (seekpos >= m_sdlChunkCopy.alen)
		return false;

	m_sdlChunk->abuf = m_sdlChunkCopy.abuf + seekpos;
	m_sdlChunk->alen = m_sdlChunkCopy.alen - seekpos;
	qDebug() << "   seekps" << seekpos;

	//m_sdlChunk->abuf = m_sdlChunk->abuf + seekpos;
	//m_sdlChunk->alen = m_sdlChunk->alen - seekpos;

	return true;
}

void SDL2AudioBackend::setVolume(int vol, int maxVol)
{
	m_currentVolume = vol;

	int v = vol * SDL_MAX_VOLUME / maxVol;
	Mix_Volume(m_parentAudioSlot.slotNumber, v);
}

void SDL2AudioBackend::setPanning(int pan, int maxPan)
{
	m_currentPan = pan;
	m_maxPan = maxPan;
	if (pan == 0)		// deactivated
		return;

	// we have to calculat amplification factors for both the left and the right channel
	if (pan > maxPan/2) {
		// panorama right
		m_panVolRight = 1.0;
		m_panVolLeft = qreal((maxPan - pan) * 2) / maxPan;
	}
	else {
		// panorama left
		m_panVolLeft = 1.0;
		m_panVolRight = qreal((pan-1) * 2) / maxPan;
	}
	m_panVolLeft = QAudio::convertVolume(m_panVolLeft,
										 QAudio::LogarithmicVolumeScale,
										 QAudio::LinearVolumeScale);
	m_panVolRight = QAudio::convertVolume(m_panVolRight,
										 QAudio::LogarithmicVolumeScale,
										 QAudio::LinearVolumeScale);

	// panning vol calculation option 2
	// m_panVolLeft = qreal(maxPan - pan) / maxPan;
	// m_panVolRight = qreal(pan) / maxPan;

	// qDebug() << "pan values" << m_panVolLeft << m_panVolRight;

}

int SDL2AudioBackend::volume() const
{
	return m_currentVolume;
}

AUDIO::AudioStatus SDL2AudioBackend::state() const
{
	//int playstate = Mix_Playing(m_parentAudioSlot.slotNumber);

	return m_currentStatus;
}

void SDL2AudioBackend::setAudioBufferSize(int bytes)
{
	Q_UNUSED(bytes)
}

int SDL2AudioBackend::audioBufferSize() const
{
	return 0;
}

void SDL2AudioBackend::delayedStartEvent()
{
	pause(false);
	sdlSetRunStatus(AUDIO_RUNNING);
	LOGTEXT(tr("<font color=green> Started delayed audio:</font> %1").arg(m_fxName));
}

void SDL2AudioBackend::sdlSetRunStatus(AudioStatus state)
{
	if (m_currentStatus != state) {
		m_currentStatus = state;
		emit statusChanged(state);
	}
}

/**
 * @brief SDL2AudioBackend::sdlChannelProcessStream
 * @param stream
 * @param len
 * @param udata
 *
 * This is called by static event callback function sdlChannelProcessor(....)
 */
void SDL2AudioBackend::sdlChannelProcessStream(void *stream, int len, void *udata)
{
	Q_UNUSED(udata)
	if (m_currentPan > 0)
		calcPanning(reinterpret_cast<char*>(stream), len, m_sdlAudioFormat);

	calcVuLevel(reinterpret_cast<const char*>(stream), len, m_sdlAudioFormat);
	m_parentAudioSlot.sdlEmitProgress();
}

/**
 * @brief SDL2AudioBackend::sdlSetFinished
 *
 * This is called by static event callback function sdlChannelDone(int chan)
 */
void SDL2AudioBackend::sdlSetFinished()
{
	qDebug() << "set sdl channel finished" << m_parentAudioSlot.slotNumber+1;

	*m_sdlChunk = m_sdlChunkCopy;
	sdlSetRunStatus(AUDIO_IDLE);
}


/**
 * @brief Static wrapper function, that forwards the callbacks to the instance that is associated with the channel number
 * @param chan
 * @param stream
 * @param len
 * @param udata
 */
void SDL2AudioBackend::sdlChannelProcessor(int chan, void *stream, int len, void *udata)
{
	const AppCentral &app = AppCentral::ref();

	SDL2AudioBackend *sdlplayer = dynamic_cast<SDL2AudioBackend*>(app.unitAudio->audioPlayer(chan));
	if (sdlplayer)
		sdlplayer->sdlChannelProcessStream(stream, len, udata);

}

void SDL2AudioBackend::sdlChannelProcessorFxDone(int chan, void *udata)
{
	Q_UNUSED(chan)
	Q_UNUSED(udata)
}

/**
 * @brief SDL2AudioBackend::sdlChannelDone
 * @param chan
 */
void SDL2AudioBackend::sdlChannelDone(int chan)
{
	LOGTEXT(tr("SDL channel %1 ready").arg(chan+1));
	const AppCentral &app = AppCentral::ref();

	SDL2AudioBackend *sdlplayer = dynamic_cast<SDL2AudioBackend*>(app.unitAudio->audioPlayer(chan));
	if (sdlplayer)
		sdlplayer->sdlSetFinished();
}

/**
 * @brief SDL2AudioBackend::sdlPostMix
 * @param udata
 * @param stream
 * @param len
 *
 * Implement this, if you want to monitor or process the final audio stream after all channels were mixed.
 */
void SDL2AudioBackend::sdlPostMix(void *udata, quint8 *stream, int len)
{
	Q_UNUSED(udata)
	Q_UNUSED(stream)
	Q_UNUSED(len)
}

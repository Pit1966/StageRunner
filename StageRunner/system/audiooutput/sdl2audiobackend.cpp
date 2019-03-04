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
	m_sdlAudioFormat.setSampleSize(16);
	m_sdlAudioFormat.setSampleType(QAudioFormat::SignedInt);
	m_sdlAudioFormat.setSampleRate(44100);
}

SDL2AudioBackend::~SDL2AudioBackend()
{

}

bool SDL2AudioBackend::setSourceFilename(const QString &path)
{
	if (m_sdlChunk) {
		*m_sdlChunk = m_sdlChunkCopy;
		Mix_FreeChunk(m_sdlChunk);
	}

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
		sdlSetRunStatus(AUDIO_RUNNING);
		m_runtime.start();
	}
}

void SDL2AudioBackend::stop()
{
	Mix_HaltChannel(m_parentAudioSlot.slotNumber);
}

void SDL2AudioBackend::pause(bool state)
{
	if (state) {
		Mix_Pause(m_parentAudioSlot.slotNumber);
	} else {
		Mix_Resume(m_parentAudioSlot.slotNumber);
	}
}

qint64 SDL2AudioBackend::currentPlayPosUs() const
{
	/// @todo implement me
	return 0;
}

qint64 SDL2AudioBackend::currentPlayPosMs() const
{
	/// @todo implement me
	return m_runtime.elapsed();
}

bool SDL2AudioBackend::seekPlayPosMs(qint64 posMs)
{
	if (!m_sdlChunk)
		return false;

	uint seekpos = uint( (44100 * posMs / 1000) * 4 );
	if (seekpos >= m_sdlChunk->alen)
		return false;

	m_sdlChunk->abuf = m_sdlChunk->abuf + seekpos;
	m_sdlChunk->alen = m_sdlChunk->alen - seekpos;

	return true;
}

void SDL2AudioBackend::setVolume(int vol, int maxvol)
{
	m_currentVolume = vol;

	int v = vol * SDL_MAX_VOLUME / maxvol;
	Mix_Volume(m_parentAudioSlot.slotNumber, v);
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

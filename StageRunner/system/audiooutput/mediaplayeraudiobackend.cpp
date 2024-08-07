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

#include "audioformat.h"
#include "mediaplayeraudiobackend.h"
#include "log.h"
#include "audioslot.h"

#include "appcontrol/appcentral.h"
#include "appcontrol/audiocontrol.h"

#include <QAudioProbe>
#include <QApplication>

using namespace AUDIO;

MediaPlayerAudioBackend::MediaPlayerAudioBackend(AudioSlot &audioChannel)
	: AudioPlayer(audioChannel)
	, m_mediaPlayer(nullptr)
	, m_audioProbe(nullptr)
	, m_currentMediaPlayerState(QMediaPlayer::StoppedState)
	, m_currentAudioStatus(AUDIO_IDLE)
{
#ifndef IS_MAC
	m_mediaPlayer = new QMediaPlayer();
	m_audioProbe = new QAudioProbe();

	if (m_audioProbe->setSource(m_mediaPlayer)) {
		connect(m_audioProbe,SIGNAL(audioBufferProbed(QAudioBuffer)),this,SLOT(calculateVuLevel(QAudioBuffer)));
	} else {
		m_audioError = AUDIO_ERR_PROBE;
		LOGERROR("Could not connect AudioProbe to MediaPlayer");
//		qDebug() << "Could not connect AudioProbe to MediaPlayer";
	}
#endif

	connect(m_mediaPlayer,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(m_mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(onPlayerStateChanged(QMediaPlayer::State)),Qt::DirectConnection);
	connect(m_mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(onMediaDurationChanged(qint64)));
}

MediaPlayerAudioBackend::~MediaPlayerAudioBackend()
{
	delete m_audioProbe;
	delete m_mediaPlayer;
}

bool MediaPlayerAudioBackend::setSourceFilename(const QString &path, const QString &fxName)
{
	setFxName(fxName);

	if (path != m_mediaPath) {
		if (!QFile::exists(path))
			return false;

		m_mediaPlayer->setMedia(QUrl::fromLocalFile(path));
		m_mediaPath = path;
	}

	return true;
}

void MediaPlayerAudioBackend::start(int loops)
{
	// qDebug("Buffer filled: %d",bufferStatus());
	m_loopCnt = 1;
	if (loops > 1) {
		m_loopTarget = loops;
	} else {
		m_loopTarget = 1;
	}

	m_currentCtrlCmd = CMD_AUDIO_START;

	if (m_startDelayedTimerId == 0) {
		m_mediaPlayer->play();

		LOGTEXT(tr("Play QMediaPlayer audio: %1 (%2)")
				.arg(m_mediaPath).arg(m_mediaPlayer->errorString()));
	} else {
		m_mediaPlayer->play();
		pause(true);
		// m_mediaPlayer->pause();

		LOGTEXT(tr("Start MediaPlayerBackend audio in delay mode: %1 (%2)")
				.arg(m_mediaPath).arg(m_mediaPlayer->errorString()));

	}

}

void MediaPlayerAudioBackend::stop()
{
	bool waspaused = m_currentMediaPlayerState == QMediaPlayer::PausedState;

	m_loopCnt = m_loopTarget;
	m_currentCtrlCmd = CMD_AUDIO_STOP;
	m_mediaPlayer->pause();
	// m_mediaPlayer->stop();

	if (waspaused)
		emit statusChanged(AUDIO_IDLE);

	if (m_startDelayedTimerId) {
		killTimer(m_startDelayedTimerId);
		m_startDelayedTimerId = 0;
	}
}

void MediaPlayerAudioBackend::pause(bool state)
{
	// qDebug() << "QMediaPlayer: set paused" << state;
	if (state) {
		m_currentCtrlCmd = CMD_AUDIO_PAUSE;
		m_mediaPlayer->pause();
	} else {
		m_currentCtrlCmd = CMD_AUDIO_START;
		m_mediaPlayer->play();
	}
}

qint64 MediaPlayerAudioBackend::currentPlayPosUs() const
{
	return m_mediaPlayer->position() * 1000;
}

qint64 MediaPlayerAudioBackend::currentPlayPosMs() const
{
	return m_mediaPlayer->position();

}

bool MediaPlayerAudioBackend::seekPlayPosMs(qint64 posMs)
{
	bool seekok = true;

	if (posMs > m_mediaPlayer->duration()) {
		seekok = false;
	}
	if (posMs < 0) {
		seekok = false;
		posMs = 0;
	}

	m_mediaPlayer->setPosition(posMs);
	return seekok;
}

void MediaPlayerAudioBackend::setVolume(int vol, int maxvol)
{
	int v = vol * 100 / maxvol;
	m_currentVolume = vol;
	m_mediaPlayer->setVolume(v);
}

void MediaPlayerAudioBackend::setPanning(int pan, int maxpan)
{
	///@implement me
	/// panning with QMediaPlayer not possible ??
	Q_UNUSED(pan)
	Q_UNUSED(maxpan)
}

int MediaPlayerAudioBackend::volume() const
{
	return m_currentVolume;
}

AudioStatus MediaPlayerAudioBackend::state() const
{
	switch (m_currentMediaPlayerState/*m_mediaPlayer->state()*/) {
	case QMediaPlayer::StoppedState:
		return AUDIO_STOPPED;
	case QMediaPlayer::PlayingState:
		return AUDIO_RUNNING;
	case QMediaPlayer::PausedState:
		return AUDIO_PAUSED;
	}

	return AUDIO_STOPPED;
}

void MediaPlayerAudioBackend::setAudioBufferSize(int bytes)
{
	Q_UNUSED(bytes)
	/// @todo implement me, if possible and useful
}

int MediaPlayerAudioBackend::audioBufferSize() const
{
	return 0;
}

void MediaPlayerAudioBackend::delayedStartEvent()
{
	pause(false);
	LOGTEXT(tr("<font color=green> Started delayed audio:</font> %1").arg(m_fxName));
}

/**
 * @brief Slot that is called, if media status of player changes
 * @param status
 *
 * This slots translates the QMediaPlayer status enums to StageRunner AudioStatus enums
 * and does some internal function handling
 */
void MediaPlayerAudioBackend::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	AUDIO::AudioStatus audiostatus;

	switch (status) {
	case QMediaPlayer::UnknownMediaStatus:
	case QMediaPlayer::NoMedia:
		audiostatus = AUDIO_ERROR;
		break;
	case QMediaPlayer::LoadingMedia:
		audiostatus = AUDIO_INIT;
		m_mediaPlayer->setVolume(m_currentVolume-1);		// why this??
		break;
	case QMediaPlayer::LoadedMedia:
		audiostatus = AUDIO_IDLE;
		break;
	case QMediaPlayer::StalledMedia:
		audiostatus = AUDIO_MEDIA_STALLED;
		break;
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		if (m_startDelayedTimerId == 0) {
			audiostatus = AUDIO_RUNNING;
		} else {
			audiostatus = m_currentAudioStatus;
		}
		m_mediaPlayer->setVolume(m_currentVolume);
		break;
	case QMediaPlayer::EndOfMedia:
		if (m_loopCnt < m_loopTarget)
			return;
		audiostatus = AUDIO_IDLE;
		break;
	case QMediaPlayer::InvalidMedia:
		audiostatus = AUDIO_ERROR;
		break;
	default:
		audiostatus = m_currentAudioStatus;
	}

	// qDebug() << "mediaStatusChanged" << status << "audiostat old" << m_currentAudioStatus << "new" << audiostatus;;
	if (m_currentAudioStatus != audiostatus) {
		m_currentAudioStatus = audiostatus;
		emit statusChanged(audiostatus);
		// qDebug() << "status changed (from media)" << audiostatus;
	}
}

void MediaPlayerAudioBackend::onPlayerStateChanged(QMediaPlayer::State state)
{
	//qDebug() << "playerStateChanged" << m_currentMediaPlayerState << state;
	if (state != m_currentMediaPlayerState) {
		AUDIO::AudioStatus audiostatus;

		if (state == QMediaPlayer::StoppedState) {
			if (m_currentMediaPlayerState == QMediaPlayer::PlayingState) {
				if (m_loopCnt < m_loopTarget) {
					m_loopCnt++;
					m_mediaPlayer->play();
					audiostatus = AUDIO_RUNNING;
				} else {
					audiostatus = AUDIO_IDLE;
				}
			} else {
				audiostatus = AUDIO_IDLE;
			}
		}
		else if (state == QMediaPlayer::PlayingState) {
			// qDebug("Current volume: %d",volume());
			// QMediaPlayer::setVolume(currentVolume);
			audiostatus = AUDIO_RUNNING;
		}
		else if (state == QMediaPlayer::PausedState) {
			if (currentAudioCmd() == CMD_AUDIO_PAUSE) {
				// this distinction is necessary cause we always pause the audio stream even if stop was selected
				audiostatus = AUDIO_PAUSED;
			} else {
				audiostatus = AUDIO_STOPPED;
			}
		}
		else {
			audiostatus = AUDIO_IDLE;
		}
		m_currentMediaPlayerState = state;

		if (m_currentAudioStatus != audiostatus) {
			m_currentAudioStatus = audiostatus;
			// qDebug() << "status changed (from player)" << audiostatus;
			emit statusChanged(audiostatus);
		}
	}
}

void MediaPlayerAudioBackend::onMediaDurationChanged(qint64 ms)
{
	emit mediaDurationChanged(ms);
}

void MediaPlayerAudioBackend::calculateVuLevel(QAudioBuffer buffer)
{
	calcVuLevel(reinterpret_cast<const char *>(buffer.constData())
				,buffer.byteCount()
				,buffer.format());
}

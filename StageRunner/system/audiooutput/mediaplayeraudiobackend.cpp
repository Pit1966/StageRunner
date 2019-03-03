#include "mediaplayeraudiobackend.h"
#include "log.h"
#include "audioslot.h"

#include <QAudioProbe>

using namespace AUDIO;

MediaPlayerAudioBackend::MediaPlayerAudioBackend(AudioSlot &audioChannel)
	: AudioPlayer(audioChannel)
	, m_mediaPlayer(new QMediaPlayer())
	, m_audioProbe(new QAudioProbe())
	, m_currentMediaPlayerState(QMediaPlayer::StoppedState)
	, m_currentAudioStatus(AUDIO_IDLE)
{
#ifndef IS_MAC
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

bool MediaPlayerAudioBackend::setSourceFilename(const QString &path)
{
	if (path != m_mediaPath) {
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
	m_mediaPlayer->play();

	LOGTEXT(tr("Play QMediaPlayer audio: %1 (%2)")
			.arg(m_mediaPath).arg(m_mediaPlayer->errorString()));

}

void MediaPlayerAudioBackend::stop()
{
	m_loopCnt = m_loopTarget;
	m_currentCtrlCmd = CMD_AUDIO_STOP;
	m_mediaPlayer->pause();
	// m_mediaPlayer->stop();
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
	m_mediaPlayer->setVolume(vol);
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
		return AUDIO_PLAYING;
	case QMediaPlayer::PausedState:
		return AUDIO_PAUSED;
	}
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
		audiostatus = AUDIO_RUNNING;
		m_mediaPlayer->setVolume(m_currentVolume);
		break;
	case QMediaPlayer::EndOfMedia:
		if (m_loopCnt < m_loopTarget)
			return;
		audiostatus = AUDIO_IDLE;
		break;
	case QMediaPlayer::InvalidMedia:
		audiostatus = AUDIO_ERROR;
	}

	if (m_currentAudioStatus != audiostatus) {
		m_currentAudioStatus = audiostatus;
		emit statusChanged(audiostatus);
	}
}

void MediaPlayerAudioBackend::onPlayerStateChanged(QMediaPlayer::State state)
{
	if (state != m_currentMediaPlayerState) {
		AUDIO::AudioStatus audiostatus;

		if (state == QMediaPlayer::StoppedState) {
			if (m_currentMediaPlayerState == QMediaPlayer::PlayingState) {
				if (m_loopCnt < m_loopTarget) {
					m_loopCnt++;
					m_mediaPlayer->play();
					audiostatus = AUDIO_PLAYING;
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
			audiostatus = AUDIO_PLAYING;
		}
		else if (state == QMediaPlayer::PausedState) {
			if (currentAudioCmd() == CMD_AUDIO_PAUSE) {
				// this distinction is necessary cause we always pause the audio stream even if stop was selected
				audiostatus = AUDIO_PAUSED;
			} else {
				audiostatus = AUDIO_STOPPED;
			}
		}
		m_currentMediaPlayerState = state;

		if (m_currentAudioStatus != audiostatus) {
			m_currentAudioStatus = audiostatus;
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

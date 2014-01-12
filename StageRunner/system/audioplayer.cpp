#include "audioplayer.h"
#include "log.h"
#include "audiochannel.h"
#include "audioiodevice.h"

AudioPlayer::AudioPlayer(AudioSlot &audioChannel)
	:QMediaPlayer()
	,myChannel(audioChannel)
	,audioProbe(new QAudioProbe(this))
	,loopTarget(1)
	,loopCnt(1)
	,currentState(QMediaPlayer::StoppedState)
	,currentVolume(100)
{
	if (audioProbe->setSource(this)) {
		connect(audioProbe,SIGNAL(audioBufferProbed(QAudioBuffer)),this,SLOT(calculate_vu_level(QAudioBuffer)));
	}

	connect(this,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_media_status_changed(QMediaPlayer::MediaStatus)),Qt::DirectConnection);
	connect(this,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(on_play_state_changed(QMediaPlayer::State)),Qt::DirectConnection);
}

bool AudioPlayer::setSourceFilename(const QString &path)
{
	if (path != mediaPath) {
		QMediaPlayer::setMedia(QUrl::fromLocalFile(path));
		mediaPath = path;
	}
	return true;
}

void AudioPlayer::start(int loops)
{
	// qDebug("Buffer filled: %d",bufferStatus());
	loopCnt = 1;
	if (loops > 1) {
		loopTarget = loops;
	} else {
		loopTarget = 1;
	}

	QMediaPlayer::play();

	LOGTEXT(tr("Play experimental audio: %1 (%2)")
			.arg(mediaPath).arg(errorString()));

}

void AudioPlayer::stop()
{
	loopCnt = loopTarget;
	QMediaPlayer::pause();
	// QMediaPlayer::stop();
}

qint64 AudioPlayer::currentPlayPosUs() const
{
	return position() * 1000;
}

qint64 AudioPlayer::currentPlayPosMs() const
{
	return position();
}

bool AudioPlayer::seekPlayPosMs(qint64 posMs)
{
	bool seekok = true;

	if (posMs > duration()) {
		seekok = false;
	}
	if (posMs < 0) {
		seekok = false;
		posMs = 0;
	}

	setPosition(posMs);
	return seekok;
}

void AudioPlayer::setVolume(int vol)
{
	currentVolume = vol;
	QMediaPlayer::setVolume(vol);
}

void AudioPlayer::on_media_status_changed(QMediaPlayer::MediaStatus status)
{
	switch (status) {
	case QMediaPlayer::UnknownMediaStatus:
	case QMediaPlayer::NoMedia:
	case QMediaPlayer::LoadingMedia:
		QMediaPlayer::setVolume(currentVolume-1);
		break;
	case QMediaPlayer::LoadedMedia:
	case QMediaPlayer::StalledMedia:
		break;
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		QMediaPlayer::setVolume(currentVolume);
		break;
	case QMediaPlayer::EndOfMedia:
		if (loopCnt < loopTarget) return;
		break;
	case QMediaPlayer::InvalidMedia:
		break;
	}

	emit statusChanged(status);
}

void AudioPlayer::on_play_state_changed(QMediaPlayer::State state)
{
	if (state != currentState) {
		if (state == QMediaPlayer::StoppedState) {
			if (currentState == QMediaPlayer::PlayingState) {
				if (loopCnt < loopTarget) {
					loopCnt++;
					QMediaPlayer::play();
				}
			}
		}
		else if (state == QMediaPlayer::PlayingState) {
			// qDebug("Current volume: %d",volume());
			// QMediaPlayer::setVolume(currentVolume);
		}
		currentState = state;
	}
}

void AudioPlayer::calculate_vu_level(QAudioBuffer buffer)
{
	/// @todo: implement this function here in audio player
	myChannel.audio_io->calcVuLevel(reinterpret_cast<const char *>(buffer.constData())
									,buffer.byteCount()
									,buffer.format());

}

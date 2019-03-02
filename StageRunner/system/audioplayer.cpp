#include "audioplayer.h"
#include "log.h"
#include "audiochannel.h"
#include "audioiodevice.h"

using namespace AUDIO;

AudioPlayer::AudioPlayer(AudioSlot &audioChannel)
	:QMediaPlayer()
	,myChannel(audioChannel)
	,m_loopTarget(1)
	,m_loopCnt(1)
	,m_currentVolume(100)
	,m_currentCtrlCmd(CMD_NONE)
	,m_audioError(AUDIO_ERR_NONE)
{
}

bool AudioPlayer::setSourceFilename(const QString &path)
{
	if (path != m_mediaPath) {
		m_mediaPath = path;
	}
	return true;
}

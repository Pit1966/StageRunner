#include "audioworker.h"
#include "audiocontrol.h"

#include <QDebug>

AudioWorker::AudioWorker(AudioControl *unitAudio, QObject *parent)
	: QObject{parent}
	, m_audioControl(unitAudio)
{

}

void AudioWorker::initMediaPlayerInstances()
{
	m_audioControl->createMediaPlayInstances();
	isValid = true;
}

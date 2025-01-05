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

bool AudioWorker::acStartFxAudioStage2(FxAudioItem *fxa, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	return m_audioControl->_startFxAudioStage2(fxa, exec, atMs, initVol, fadeInMs);
}

bool AudioWorker::acStartFxAudioInSlot(FxAudioItem *fxa, int slotnum, Executer *exec, qint64 atMs, int initVol, int fadeInMs)
{
	return m_audioControl->_startFxAudioInSlot(fxa, slotnum, exec, atMs, initVol, fadeInMs);
}

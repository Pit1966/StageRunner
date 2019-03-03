#include "iodeviceaudiobackend.h"
#include "audioslot.h"
#include "log.h"
#include "audioiodevice.h"

#include <QAudioOutput>

using namespace AUDIO;

IODeviceAudioBackend::IODeviceAudioBackend(AudioSlot &audioChannel, const QString &devName)
	: AudioPlayer(audioChannel)
	, m_audioIODev(nullptr)
	, m_audioOutput(nullptr)
	, m_currentOutputState(QAudio::StoppedState)
	, m_currentAudioStatus(AUDIO_NO_STATE)
{
	// No AudioIODevice SoundOutputSupport for MAC OS X
	m_audioIODev = new AudioIODevice(AudioFormat::defaultFormat());
	if (m_audioIODev->audioError()) {
		m_audioError = m_audioIODev->audioError();
		m_audioErrorString = m_audioIODev->lastErrorText();
	}

	if (devName.isEmpty() || devName == "system default") {
		m_audioOutput = new QAudioOutput(AudioFormat::defaultFormat(),this);
	}
	else {
		bool ok;
		QAudioDeviceInfo dev = AudioIODevice::getAudioDeviceInfo(devName, &ok);
		if (ok) {
			m_audioOutput = new QAudioOutput(dev, AudioFormat::defaultFormat(),this);
		} else {
			m_audioOutput = new QAudioOutput(AudioFormat::defaultFormat(),this);
		}
	}

	connect(m_audioOutput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(onAudioOutputStatusChanged(QAudio::State)));
	connect(m_audioIODev,SIGNAL(readReady()),this,SLOT(onAudioIODevReadReady()),Qt::QueuedConnection);
	connect(m_audioIODev,SIGNAL(audioDurationDetected(qint64)),this,SLOT(onMediaDurationDetected(qint64)));
	connect(m_audioIODev,SIGNAL(vuLevelChanged(qreal,qreal)),this,SIGNAL(vuLevelChanged(qreal,qreal)));
	connect(m_audioIODev,SIGNAL(frqSpectrumChanged(FrqSpectrum*)),this,SIGNAL(frqSpectrumChanged(FrqSpectrum*)));
}

IODeviceAudioBackend::~IODeviceAudioBackend()
{
	delete m_audioOutput;
	delete m_audioIODev;
}

bool IODeviceAudioBackend::setSourceFilename(const QString &path)
{
	if (!QFile::exists(path))
		return false;

	// Set filename
	m_audioIODev->setSourceFilename(path);
	// and implicitely start decoding
	m_audioIODev->start(1);			// set loop count to 1 for now. Later in start function this will be set again

	return true;
}

void IODeviceAudioBackend::start(int loops)
{
	m_loopCnt = 1;
	if (loops > 1) {
		m_loopTarget = loops;
	} else {
		m_loopTarget = 1;
	}
	m_audioIODev->setLoopCount(loops);

	m_audioOutput->start(m_audioIODev);
}

void IODeviceAudioBackend::stop()
{
	m_audioOutput->stop();
	m_audioIODev->stop();
}

void IODeviceAudioBackend::pause(bool state)
{
	if (state) {
		m_audioOutput->suspend();
	} else {
		m_audioOutput->resume();
	}
}

qint64 IODeviceAudioBackend::currentPlayPosUs() const
{
	return m_audioIODev->currentPlayPosUs();
}

qint64 IODeviceAudioBackend::currentPlayPosMs() const
{
	return m_audioIODev->currentPlayPosMs();
}

bool IODeviceAudioBackend::seekPlayPosMs(qint64 posMs)
{
	return m_audioIODev->seekPlayPosMs(posMs);
}

void IODeviceAudioBackend::setVolume(int vol, int maxvol)
{
	qreal v = qreal(vol) / maxvol;
	m_currentVolume = vol;
	m_audioOutput->setVolume(v);
}

int IODeviceAudioBackend::volume() const
{
	return m_currentVolume;
}

AudioStatus IODeviceAudioBackend::state() const
{
	switch (m_currentOutputState) {
	case QAudio::ActiveState:
		return AUDIO_PLAYING;
	case QAudio::SuspendedState:
		return AUDIO_PAUSED;
	case QAudio::IdleState:
	case QAudio::StoppedState:
		return AUDIO_STOPPED;
	}

	return AUDIO_STOPPED;
}

void IODeviceAudioBackend::setAudioBufferSize(int bytes)
{
	m_audioOutput->setBufferSize(bytes);
}

int IODeviceAudioBackend::audioBufferSize() const
{
	return m_audioOutput->bufferSize();
}

void IODeviceAudioBackend::onAudioOutputStatusChanged(QAudio::State state)
{
	m_currentOutputState = state;

	AudioStatus audiostatus;

	switch (state) {
	case QAudio::ActiveState:
		audiostatus = AUDIO_RUNNING;
		break;
	case QAudio::IdleState:
		audiostatus = AUDIO_IDLE;
		break;
	case QAudio::SuspendedState:
		audiostatus = AUDIO_PAUSED;
		break;
	case QAudio::StoppedState:
		if (!m_audioIODev->isDecodingFinished()) {
			DEBUGERROR("Audio is in Stopped State while decoding -> This might be a buffer underrun for an audio channel");
			audiostatus = AUDIO_ERROR;
		} else {
			audiostatus = AUDIO_STOPPED;
		}
		break;

#if QT_VERSION >= 0x050b00
	case QAudio::InterruptedState:
		DEBUGERROR("%s: QAudio::Interrupted",Q_FUNC_INFO);
		break;
#endif
	default:
		audiostatus = m_currentAudioStatus;
	}

	if (m_audioIODev->audioError() != AUDIO_ERR_NONE) {
		audiostatus = AUDIO_ERROR;
		m_audioError = m_audioIODev->audioError();
	}

	if (m_currentAudioStatus != audiostatus) {
		m_currentAudioStatus = audiostatus;
		emit statusChanged(audiostatus);
	}
}

void IODeviceAudioBackend::onAudioIODevReadReady()
{
	// qDebug("Audio io ready read");
	m_audioOutput->stop();
	m_audioIODev->stop();
}

void IODeviceAudioBackend::onMediaDurationDetected(qint64 ms)
{
	emit mediaDurationChanged(ms);
}

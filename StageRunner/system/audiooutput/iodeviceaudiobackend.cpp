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

#include "iodeviceaudiobackend.h"
#include "audioslot.h"
#include "log.h"
#include "audioiodevice.h"

#include <QAudioOutput>

using namespace AUDIO;

IODeviceAudioBackend::IODeviceAudioBackend(AudioSlot &audioChannel, const QString &devName)
	: AudioPlayer(audioChannel)
	, m_audioIODev(nullptr)
#ifdef IS_QT6
	, m_audioSink(nullptr)
#else
	, m_audioOutput(nullptr)
#endif
	, m_currentOutputState(QAudio::StoppedState)
	, m_currentAudioStatus(AUDIO_NO_STATE)
	, m_requestedBufferSize(0)
	, m_gotBufferSize(0)
{
	// No AudioIODevice SoundOutputSupport for MAC OS X
	m_audioIODev = new AudioIODevice(AudioFormat::defaultFormat());
	if (m_audioIODev->audioError()) {
		m_audioError = m_audioIODev->audioError();
		m_audioErrorString = m_audioIODev->lastErrorText();
	}

#ifdef IS_QT6
	QAudioFormat format = AudioFormat::defaultFormat();
	if (devName.isEmpty() || devName == "system default") {
		m_audioSink = new QAudioSink(format, this);
	}
	else {
		bool ok;
		QAudioDevice dev = AudioIODevice::getAudioDeviceInfo(devName, &ok);
		if (ok) {
			m_audioSink = new QAudioSink(dev, format, this);
		} else {
			m_audioSink = new QAudioSink(format, this);
		}
	}
	connect(m_audioSink, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioOutputStatusChanged(QAudio::State)));

#else
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
#endif

	connect(m_audioIODev,SIGNAL(readReady()),this,SLOT(onAudioIODevReadReady()),Qt::QueuedConnection);
	connect(m_audioIODev,SIGNAL(audioDurationDetected(qint64)),this,SLOT(onMediaDurationDetected(qint64)));
	connect(m_audioIODev,SIGNAL(vuLevelChanged(qreal,qreal)),this,SIGNAL(vuLevelChanged(qreal,qreal)));
	connect(m_audioIODev,SIGNAL(frqSpectrumChanged(FrqSpectrum*)),this,SIGNAL(frqSpectrumChanged(FrqSpectrum*)));
}

IODeviceAudioBackend::~IODeviceAudioBackend()
{
#ifdef IS_QT6
	delete m_audioSink;
#else
	delete m_audioOutput;
#endif
	delete m_audioIODev;
}

bool IODeviceAudioBackend::setSourceFilename(const QString &path, const QString &fxName)
{
	if (!QFile::exists(path))
		return false;

	// for information set fx name
	setFxName(fxName);

	// Set filename
	m_audioIODev->setSourceFilename(path);
	m_mediaPath = path;
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

#ifdef IS_QT6
	if (m_startDelayedTimerId == 0) {
		m_audioSink->start(m_audioIODev);
	}
	else {
		m_audioSink->start(m_audioIODev);
		pause(true);
		LOGTEXT(tr("Start IODeviceBackend audio in delay mode: %1 (%2)")
				.arg(m_mediaPath).arg(m_audioErrorString));
	}

	if (m_audioSink->error() != QAudio::NoError)
		qWarning() << "start audio" << m_audioSink->error();

	if (m_gotBufferSize != m_audioSink->bufferSize()) {
		m_gotBufferSize = m_audioSink->bufferSize();
		LOGTEXT(tr("Audio requested buffer size: %1 -> got %2 bytes").arg(m_requestedBufferSize).arg(m_gotBufferSize));
	}

#else
	if (m_startDelayedTimerId == 0) {
		m_audioOutput->start(m_audioIODev);
	} else {
		m_audioOutput->start(m_audioIODev);
		pause(true);
		LOGTEXT(tr("Start IODeviceBackend audio in delay mode: %1 (%2)")
				.arg(m_mediaPath).arg(m_audioErrorString));
	}

	if (m_audioOutput->error() != QAudio::NoError)
		qWarning() << "start audio" << m_audioOutput->error();
#endif
}

void IODeviceAudioBackend::stop()
{
	m_audioIODev->stop();
#ifdef IS_QT6
	m_audioSink->stop();
#else
	m_audioOutput->stop();
#endif
}

void IODeviceAudioBackend::pause(bool state)
{
#ifdef IS_QT6
	if (state) {
		m_audioSink->suspend();
	} else {
		m_audioSink->resume();
	}
#else
	if (state) {
		m_audioOutput->suspend();
	} else {
		m_audioOutput->resume();
	}
#endif
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
#ifdef IS_QT6
	m_audioSink->setVolume(v);
#else
	m_audioOutput->setVolume(v);
#endif
}

int IODeviceAudioBackend::volume() const
{
	return m_currentVolume;
}

AudioStatus IODeviceAudioBackend::state() const
{
	switch (m_currentOutputState) {
	case QAudio::ActiveState:
		return AUDIO_RUNNING;
	case QAudio::SuspendedState:
		return AUDIO_PAUSED;
#if QT_VERSION >= 0x050b00 && QT_VERSION_MAJOR < 6
	case QAudio::InterruptedState:
#endif
	case QAudio::IdleState:
	case QAudio::StoppedState:
		return AUDIO_STOPPED;
	}

	return AUDIO_STOPPED;
}

void IODeviceAudioBackend::setAudioBufferSize(int bytes)
{
	qDebug() << "set audio buffer size" << bytes;
#ifdef IS_QT6
	// m_audioSink->setBufferSize(bytes);
#else
	m_audioOutput->setBufferSize(bytes);
#endif
	if (m_requestedBufferSize != bytes) {
		m_requestedBufferSize = bytes;
		m_gotBufferSize = 0;
	}
}

int IODeviceAudioBackend::audioBufferSize() const
{
#ifdef IS_QT6
	return m_audioSink->bufferSize();
#else
	return m_audioOutput->bufferSize();
#endif
}

void IODeviceAudioBackend::delayedStartEvent()
{
	pause(false);
	LOGTEXT(tr("<font color=green> Started delayed audio:</font> %1").arg(m_fxName));
}

void IODeviceAudioBackend::setFFTEnabled(bool state)
{
	m_audioIODev->setFFTEnabled(state);
}

bool IODeviceAudioBackend::isFFTEnabled() const
{
	return m_audioIODev->isFFTEnabled();
}

void IODeviceAudioBackend::onAudioOutputStatusChanged(QAudio::State state)
{
	m_currentOutputState = state;
	// qDebug() << "QAudioDevice state changed" << state << m_audioSink->state();

	AudioStatus audiostatus;

	switch (state) {
	case QAudio::ActiveState:
		audiostatus = AUDIO_RUNNING;
		break;
	case QAudio::IdleState:
		audiostatus = AUDIO_IDLE;
#ifdef IS_QT6
        m_audioSink->stop();
        m_audioSink->reset();
#endif
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
#ifdef IS_QT6
        if (m_audioSink->error() != QAudio::NoError) {
            qWarning() << "Audio error" << m_audioSink->error();
        }
        m_audioSink->stop();
        m_audioSink->reset();
#endif
		break;

#if QT_VERSION >= 0x050b00 && QT_VERSION_MAJOR < 6
	case QAudio::InterruptedState:
		DEBUGERROR("%s: QAudio::Interrupted",Q_FUNC_INFO);
		audiostatus = AUDIO_NO_STATE;
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
		// qDebug() << "audio player status changed" << m_currentAudioStatus << audiostatus;
		m_currentAudioStatus = audiostatus;
		emit statusChanged(audiostatus);
	}
}

void IODeviceAudioBackend::onAudioIODevReadReady()
{
	// qDebug("Audio io ready read");
#ifdef IS_QT6
	m_audioSink->stop();
#else
	m_audioOutput->stop();
#endif
	m_audioIODev->stop();
}

void IODeviceAudioBackend::onMediaDurationDetected(qint64 ms)
{
	emit mediaDurationChanged(ms);
}

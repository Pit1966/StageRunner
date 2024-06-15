//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2024 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2024 stonechip entertainment
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

#include "iodeviceaudiobackend6.h"
#include "audioiodevice6.h"
#include "audioslot.h"
#include "system/log.h"
#include "system/extelapsedtimer.h"

#include <QAudioSink>
#include <QMediaDevices>

using namespace AUDIO;

IODeviceAudioBackend::IODeviceAudioBackend(AudioSlot &audioChannel, const QString &devName)
	: AudioPlayer(audioChannel)
	, m_audioIODev(nullptr)
	, m_audioSink(nullptr)
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
		m_audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), AudioFormat::defaultFormat(), this);
	}
	else {
		bool ok;
		QAudioDevice dev = AudioIODevice::getAudioDevice(devName, &ok);
		if (ok) {
			m_audioSink = new QAudioSink(dev, AudioFormat::defaultFormat(), this);
		} else {
			m_audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), AudioFormat::defaultFormat(), this);
		}
	}

	connect(m_audioSink,SIGNAL(stateChanged(QAudio::State)),this,SLOT(onAudioOutputStatusChanged(QAudio::State)));
	connect(m_audioIODev,SIGNAL(readReady()),this,SLOT(onAudioIODevReadReady()),Qt::QueuedConnection);
	connect(m_audioIODev,SIGNAL(audioDurationDetected(qint64)),this,SLOT(onMediaDurationDetected(qint64)));
	connect(m_audioIODev,SIGNAL(vuLevelChanged(qreal,qreal)),this,SIGNAL(vuLevelChanged(qreal,qreal)));
	connect(m_audioIODev,SIGNAL(frqSpectrumChanged(FrqSpectrum*)),this,SIGNAL(frqSpectrumChanged(FrqSpectrum*)));
}

IODeviceAudioBackend::~IODeviceAudioBackend()
{
	delete m_audioSink;
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

	if (m_startDelayedTimerId == 0) {
		m_audioSink->start(m_audioIODev);
	} else {
		m_audioSink->start(m_audioIODev);
		pause(true);
		LOGTEXT(tr("Start IODeviceBackend audio in delay mode: %1 (%2)")
				.arg(m_mediaPath, m_audioErrorString));
	}

	if (m_audioSink->error() != QAudio::NoError)
		qWarning() << "start audio" << m_audioSink->error();
}

void IODeviceAudioBackend::stop()
{
	ExtElapsedTimer debugtime;
	m_audioSink->stop();
	m_audioIODev->stop();
	qint64 t = debugtime.nsecsElapsed() / 1000;
	qDebug() << "stop audio tooks" << t << "µs";
}

void IODeviceAudioBackend::pause(bool state)
{
	if (state) {
		m_audioSink->suspend();
	} else {
		m_audioSink->resume();
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
	m_audioSink->setVolume(v);
}

void IODeviceAudioBackend::setPanning(int pan, int maxpan)
{
	m_currentPan = pan;
	// we have to do the panning by hand.
	// so audio data must be manipulated and we do that in the device where the data is read
	m_audioIODev->setPanning(pan, maxpan);
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
	case QAudio::IdleState:
	case QAudio::StoppedState:
		return AUDIO_STOPPED;
	}

	return AUDIO_STOPPED;
}

void IODeviceAudioBackend::setAudioBufferSize(int bytes)
{
	m_audioSink->setBufferSize(bytes);
}

int IODeviceAudioBackend::audioBufferSize() const
{
	return m_audioSink->bufferSize();
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
    // qDebug() << "QAudioDevice state changed" << state;

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
	m_audioSink->stop();
	m_audioIODev->stop();
}

void IODeviceAudioBackend::onMediaDurationDetected(qint64 ms)
{
	emit mediaDurationChanged(ms);
}

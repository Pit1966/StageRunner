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

#ifndef IODEVICEAUDIOBACKEND_H
#define IODEVICEAUDIOBACKEND_H

#include "system/audioplayer.h"
#include "commandsystem.h"

#include <QObject>
#include <QAudio>
#include <QMutex>

class AudioIODevice;
class QAudioOutput;

class IODeviceAudioBackend : public AudioPlayer
{
	Q_OBJECT
private:
	AudioIODevice *m_audioIODev;
	QAudioOutput *m_audioOutput;
	QAudio::State m_currentOutputState;
	AUDIO::AudioStatus m_currentAudioStatus;			///< This is AudioIODevice state and output state translated to StageRunner audio state


public:
	IODeviceAudioBackend(AudioSlot &audioChannel, const QString &devName);
	~IODeviceAudioBackend() override;
	AUDIO::AudioOutputType outputType() const override {return AUDIO::OUT_DEVICE;}

	bool setSourceFilename(const QString &path, const QString &fxName) override;
	void start(int loops) override;
	void stop() override;
	void pause(bool state) override;
	qint64 currentPlayPosUs() const override;
	qint64 currentPlayPosMs() const override;
	bool seekPlayPosMs(qint64 posMs) override;
	void setVolume(int vol, int maxvol) override;
	int volume() const override;
	AUDIO::AudioStatus state() const override;

	void setAudioBufferSize(int bytes) override;
	int audioBufferSize() const override;

	void delayedStartEvent();


private slots:
	void onAudioOutputStatusChanged(QAudio::State state);
	void onAudioIODevReadReady();
	void onMediaDurationDetected(qint64 ms);

};

#endif // IODEVICEAUDIOBACKEND_H

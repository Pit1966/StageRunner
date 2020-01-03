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

#ifndef MEDIAPLAYERAUDIOBACKEND_H
#define MEDIAPLAYERAUDIOBACKEND_H

#include "audioplayer.h"
#include "commandsystem.h"

#include <QMediaPlayer>
#include <QAudioBuffer>
#include <QObject>


class QAudioProbe;

class MediaPlayerAudioBackend : public AudioPlayer
{
	Q_OBJECT
private:
	QMediaPlayer *m_mediaPlayer;
	QAudioProbe *m_audioProbe;
	QMediaPlayer::State m_currentMediaPlayerState;
	AUDIO::AudioStatus m_currentAudioStatus;			///< This is QMediaPlayer::State and QMediaPlayer::MediaStatus translated to StageRunner status

public:
	MediaPlayerAudioBackend(AudioSlot &audioChannel);
	~MediaPlayerAudioBackend() override;
	AUDIO::AudioOutputType outputType() const override {return AUDIO::OUT_MEDIAPLAYER;}

	bool setSourceFilename(const QString &path) override;
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

	void delayedStartEvent() override;

private slots:
	void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
	void onPlayerStateChanged(QMediaPlayer::State state);
	void onMediaDurationChanged(qint64 ms);
	void calculateVuLevel(QAudioBuffer buffer);

};

#endif // MEDIAPLAYERAUDIOBACKEND_H

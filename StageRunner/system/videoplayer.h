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

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>

#include "commandsystem.h"

using namespace AUDIO;

class PsVideoWidget;
class FxClipItem;

class VideoPlayer : public QMediaPlayer
{
	Q_OBJECT
protected:
	PsVideoWidget *m_videoWid;
	int loopTarget;
	int loopCnt;
	int m_slotNumber;
	QMediaPlayer::State currentState;
	FxClipItem *m_currentFxClipItem;

public:
	VideoPlayer(PsVideoWidget *videoWid);
	bool playFxClip(FxClipItem *fxc, int slotNum);
	inline int slotNumber() const {return m_slotNumber;}
	void stop();

private slots:
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_play_state_changed(QMediaPlayer::State state);
	void on_playback_position_changed(qint64 pos);

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);
	void clipCtrlMsgEmitted(AudioCtrlMsg msg);
	void clipProgressChanged(FxClipItem *fxclip, int perMille);
	void audioCtrlMsgEmitted(const AudioCtrlMsg &msg);

	void endReached(qint64 ms);
	void seekMe(qint64 ms);
};

#endif // VIDEOPLAYER_H

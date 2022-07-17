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
#include <QTimeLine>

#include "commandsystem.h"

using namespace AUDIO;

class PsVideoWidget;
class FxClipItem;
class VideoControl;

class VideoPlayer : protected QMediaPlayer
{
	Q_OBJECT
protected:
	VideoControl *m_videoCtrl;
	PsVideoWidget *m_videoWid;
	int loopTarget;
	int loopCnt;
	int m_slotNumber;
	int m_currentVolume;					///< current volume in StageRunner range (0 - MAX_VOLUME)
	int m_currentMasterVolume;
	QMediaPlayer::State currentState;
	VIDEO::VideoViewStatus m_viewState;		///< this is current widget status of videoplayer and still picture video overlays
	FxClipItem *m_currentFxClipItem;

	QTimeLine m_overlayFadeTimeLine;		///< used to fade in and out the overlay picture
	bool m_overlayFadeOutFirst;

public:
	VideoPlayer(VideoControl *parent, PsVideoWidget *videoWid);

	bool playFxClip(FxClipItem *fxc, int slotNum);
	inline int slotNumber() const {return m_slotNumber;}
	bool isRunning() const;
	bool isCurrentPicture() const;
	void stop();
	bool stopAndWait();
	void setVolume(int vol);
	void setMasterVolume(int vol);
	void setPicClipOverlayDisabled();
	bool isPicClipOverlayVisible();
	bool fadePicClipOverlayOut(int ms);
	bool fadePicClipOverlayIn(int ms);

	inline QMultimedia::AvailabilityStatus availability() const {return QMediaPlayer::availability();}
	inline QMediaPlayer * mediaPlayer() {return this;}

private slots:
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_play_state_changed(QMediaPlayer::State state);
	void on_playback_position_changed(qint64 pos);
	void onVideoEnd();
	void setOverlayFade(qreal val);
	void setOverlayFadeFinished();

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);
	void clipCtrlMsgEmitted(AudioCtrlMsg msg);
	void clipProgressChanged(FxClipItem *fxclip, int perMille);
	void audioCtrlMsgEmitted(const AudioCtrlMsg &msg);

	void endReached(qint64 ms);
	void seekMe(qint64 ms);
};

#endif // VIDEOPLAYER_H

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
#include <QTimer>
#include <QPointer>

#include "commandsystem.h"

//using namespace AUDIO;

class PsVideoWidget;
class FxClipItem;
class VideoControl;
class Executer;

#define PIC_OVERLAY_COUNT 2

class VideoPlayer : public QMediaPlayer
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
	FxClipItem *m_currentFxClipItem;		///< @todo make guarded
	FxClipItem *m_ctrlFxItem;				///< Item used for current control command (e.g. PRE_DELAY)

	QTimer m_ctrlTimer;
	QTimeLine m_overlayFadeTimeLine[PIC_OVERLAY_COUNT];		///< used to fade in and out the overlay picture
	bool m_overlayFadeOutFirst;				///< Overlay PicClip has to be started after current one was faded out
	bool m_stopVideoAtEventEnd;				///< video player has to be stopped. Usaly after a fade out

public:
	VideoPlayer(VideoControl *unitVideo, PsVideoWidget *videoWid);
	FxClipItem *currentFxClipItem();

	VIDEO::VideoViewStatus viewState() const {return m_viewState;}
	static QString viewStateToString(VIDEO::VideoViewStatus viewState);

	bool playFxClip(FxClipItem *fxc, int slotNum);
	inline int slotNumber() const {return m_slotNumber;}
	bool isRunning() const;
	bool isCurrentFxClipAPicClip() const;
	void stop();
	bool stopAndWait();
	void setVolume(int vol);
	void setMasterVolume(int vol);
	void setPicClipOverlaysDisabled();
	bool isPicClipOverlaysActive();
	bool fadePicClipOverlayOut(int ms, int layer);
	bool fadePicClipOverlayIn(int ms, int layer);
	bool fadeVideoToBlack(int ms);

	inline QMultimedia::AvailabilityStatus availability() const {return QMediaPlayer::availability();}
	inline QMediaPlayer * mediaPlayer() {return this;}
	void clearViewState();

protected:
	void setViewState(VIDEO::VideoViewStatus state, bool addState = false);
	void fadeOutCurrent(int ms);

private:
	bool _playPicClip(FxClipItem *fxc, FxClipItem *old_fxc);
	bool _playVideoClip(FxClipItem *fxc, FxClipItem *old_fxc, int slotNum);

	bool fadeInCurrentFxClipItem(int ms, int layer);

private slots:
	void on_media_status_changed(QMediaPlayer::MediaStatus status);
	void on_play_state_changed(QMediaPlayer::State state);
	void on_playback_position_changed(qint64 pos);
	void onVideoEnd();
	void setOverlayFade(qreal val, int layer);
	void setOverlayFadeFinished(int layer);
	void onCtrlTimerFinished();
	void stopAllOverlayFades();

signals:
	void statusChanged(QMediaPlayer::MediaStatus status);
	void clipCtrlMsgEmitted(AUDIO::AudioCtrlMsg msg);
	void clipProgressChanged(FxClipItem *fxclip, int perMille);
	void audioCtrlMsgEmitted(const AUDIO::AudioCtrlMsg &msg);
	void viewStateChanged(int viewState, int oldViewState);

	void endReached(qint64 ms);
	void seekMe(qint64 ms);
	void fadeToBlackFinished();
};

#endif // VIDEOPLAYER_H

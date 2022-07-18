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

#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

class FxAudioItem;
class Executer;


enum CtrlCmd {
	CMD_NONE					= 0,
	CMD_AUDIO_STATUS_CHANGED	= 1,
	CMD_AUDIO_START				= 2,
	CMD_AUDIO_START_AT			= 3,
	CMD_AUDIO_STOP				= 4,
	CMD_AUDIO_PAUSE				= 5,
	CMD_AUDIO_FADEIN			= 6,
	CMD_AUDIO_FADEOUT			= 7,
	CMD_AUDIO_CHANGE_VOL		= 8,
	CMD_SCENE_BLACK				= 9,
	CMD_SCENE_FADEIN			= 10,
	CMD_SCENE_FADEOUT			= 11,
	CMD_SCENE_FADETO			= 12,
	CMD_VIDEO_START				= 13,
	CMD_VIDEO_STATUS_CHANGED	= 14,


	CMD_STATUS_REPORT		= CMD_NONE,
	CMD_FX_START			= CMD_AUDIO_START,
	CMD_FX_START_AT			= CMD_AUDIO_START_AT,
	CMD_FX_STOP				= CMD_AUDIO_STOP,
	CMD_SCENE_START			= CMD_AUDIO_START,
	CMD_SCENE_STOP			= CMD_AUDIO_STOP
};

namespace AUDIO {

enum AudioStatus {
	AUDIO_NO_STATE,			// 0
	AUDIO_NO_FREE_SLOT,		// 1
	AUDIO_ERROR,			// 2
	AUDIO_IDLE,				// 3
	AUDIO_INIT,				// 4
	AUDIO_BUFFERING,		// 5
	AUDIO_RUNNING,			// 6
	AUDIO_PAUSED,			// 7
	AUDIO_MEDIA_STALLED,	// 8
	VIDEO_INIT,				// 9
	VIDEO_RUNNING,			// 10

	AUDIO_STOPPED = AUDIO_IDLE,
	VIDEO_IDLE = AUDIO_IDLE
};

enum AudioSeqState {
	AUDIO_OFF,
	AUDIO_PRE_DELAY,
	AUDIO_PLAYTIME,
	AUDIO_FADEOUT,
	AUDIO_POST_DELAY
};

enum AudioFadeMode {
	AUDIO_FADE_IDLE,
	AUDIO_FADE_IN,
	AUDIO_FADE_OUT
};

enum AudioErrorType {
	AUDIO_ERR_NONE,
	AUDIO_ERR_TIMEOUT,
	AUDIO_ERR_DECODER,
	AUDIO_ERR_PROBE,
	AUDIO_ERR_FILE_NOT_OPENED
};

enum AudioOutputType {
	OUT_NONE,
	OUT_DEVICE,
	OUT_MEDIAPLAYER,
	OUT_SDL2
};

class AudioCtrlMsg {
public:
	int slotNumber;
	CtrlCmd ctrlCmd;
	AudioStatus currentAudioStatus;
	int volume;
	int fadetime;
	int progress;
	int progressTime;
	int loop;
	int maxloop;
	bool isActive;
	FxAudioItem *fxAudio;			// maybe FxClipItem in some cases too
	Executer *executer;

public:
	AudioCtrlMsg(int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, AudioStatus status = AUDIO_NO_STATE, Executer *exec = nullptr)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(status)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, progressTime(0)
		, loop(0)
		, maxloop(0)
		, isActive(false)
		, fxAudio(nullptr)
		, executer(exec)
	{}

	AudioCtrlMsg(FxAudioItem *fxaudio, int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, Executer *exec = nullptr)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(AUDIO_NO_STATE)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, loop(0)
		, maxloop(0)
		, isActive(false)
		, fxAudio(fxaudio)
		, executer(exec)
	{}
};

}

namespace LIGHT {
enum SceneFlags {
	// !!!! Do not change the shift indices  !!!
	// Look FxSceneItem::initSceneCommand
	SCENE_IDLE = 0,					///< Scene does nothing and is not on stage and not direct
	SCENE_STAGE_LIVE = 1<<0,		///< Scene is live (Lightdesk is opened)
	SCENE_STAGE_INTERN = 1<<1,		///< Scene is visible on stage controlled by application logic
	SCENE_STAGE_EXTERN = 1<<2,		///< Scene is visible on stage controlled by input
	SCENE_ACTIVE_INTERN = 1<<8,		///< Scene is fading at the moment
	SCENE_ACTIVE_EXTERN = 1<<9		///< Scene is fading caused by direct input
};

enum SceneSeqState {
	SCENE_OFF,
	SCENE_PRE_DELAY,
	SCENE_FADE_IN,
	SCENE_HOLD,
	SCENE_FADE_OUT,
	SCENE_POST_DELAY
};
}

namespace VIDEO {
/**
 * @brief Flags for VideoPlayer and PicClip (video overlay) status
 */
enum VideoViewStatus {
	VIEW_UNUSED			= 0,
	VIEW_BLACK			= 1<<0,			// 1
	VIEW_VIDEO_VISIBLE	= 1<<1,			// 2
	VIEW_VIDEO_FADEIN	= 1<<2,			// 4
	VIEW_VIDEO_FADEOUT	= 1<<3,			// 8
	VIEW_PIC_VISIBLE	= 1<<4,			// 16
	VIEW_PIC_FADEIN		= 1<<5,			// 32
	VIEW_PIC_FADEOUT	= 1<<6,			// 64

};
}


#endif // COMMANDSYSTEM_H

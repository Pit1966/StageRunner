#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

class FxAudioItem;

enum CtrlCmd {
	CMD_NONE = 0,
	CMD_AUDIO_START = 1,
	CMD_AUDIO_STOP = 2,
	CMD_AUDIO_FADEIN = 3,
	CMD_AUDIO_FADEOUT = 4,
	CMD_AUDIO_CHANGE_VOL = 5,
	CMD_SCENE_BLACK = 6,
	CMD_SCENE_FADEIN = 7,
	CMD_SCENE_FADEOUT = 8,
	CMD_SCENE_FADETO = 9,


	CMD_STATUS_REPORT = CMD_NONE,
	CMD_FX_START = CMD_AUDIO_START,
	CMD_FX_STOP = CMD_AUDIO_STOP,
	CMD_SCENE_START = CMD_AUDIO_START,
	CMD_SCENE_STOP = CMD_AUDIO_STOP
};


namespace AUDIO {

enum AudioStatus {
	AUDIO_IDLE,
	AUDIO_ERROR,
	AUDIO_INIT,
	AUDIO_RUNNING
};


class AudioCtrlMsg {
public:
	int slotNumber;
	CtrlCmd ctrlCmd;
	AudioStatus currentAudioStatus;
	int volume;
	int fadetime;
	int progress;
	FxAudioItem *fxAudio;

public:
	AudioCtrlMsg(int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, AudioStatus status = AUDIO_IDLE)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(status)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, fxAudio(0)
	{}

	AudioCtrlMsg(FxAudioItem *fxaudio, int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(AUDIO_IDLE)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, fxAudio(fxaudio)
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
}


#endif // COMMANDSYSTEM_H

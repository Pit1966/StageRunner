#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

class FxAudioItem;
class Executer;


enum CtrlCmd {
	CMD_NONE = 0,
	CMD_AUDIO_STATUS_CHANGED = 1,
	CMD_AUDIO_START = 2,
	CMD_AUDIO_START_AT = 3,
	CMD_AUDIO_STOP = 4,
	CMD_AUDIO_PAUSE = 5,
	CMD_AUDIO_FADEIN = 6,
	CMD_AUDIO_FADEOUT = 7,
	CMD_AUDIO_CHANGE_VOL = 8,
	CMD_SCENE_BLACK = 9,
	CMD_SCENE_FADEIN = 10,
	CMD_SCENE_FADEOUT = 11,
	CMD_SCENE_FADETO = 12,


	CMD_STATUS_REPORT = CMD_NONE,
	CMD_FX_START = CMD_AUDIO_START,
	CMD_FX_START_AT = CMD_AUDIO_START_AT,
	CMD_FX_STOP = CMD_AUDIO_STOP,
	CMD_SCENE_START = CMD_AUDIO_START,
	CMD_SCENE_STOP = CMD_AUDIO_STOP
};


namespace AUDIO {

enum AudioStatus {
	AUDIO_NO_STATE,
	AUDIO_IDLE,
	AUDIO_ERROR,
	AUDIO_INIT,
	AUDIO_RUNNING,
	AUDIO_NO_FREE_SLOT
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
	AUDIO_ERR_DECODER
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
	FxAudioItem *fxAudio;
	Executer *executer;

public:
	AudioCtrlMsg(int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, AudioStatus status = AUDIO_NO_STATE, Executer *exec = 0)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(status)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, progressTime(0)
		, loop(0)
		, maxloop(0)
		, fxAudio(0)
		, executer(exec)
	{}

	AudioCtrlMsg(FxAudioItem *fxaudio, int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, Executer *exec = 0)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(AUDIO_NO_STATE)
		, volume(-1)
		, fadetime(-1)
		, progress(-1)
		, loop(0)
		, maxloop(0)
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


#endif // COMMANDSYSTEM_H

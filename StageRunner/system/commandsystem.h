#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

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
	CMD_SCENE_START = CMD_AUDIO_START
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

public:
	AudioCtrlMsg(int slotnum = -1, CtrlCmd cmd = CMD_STATUS_REPORT, AudioStatus status = AUDIO_IDLE)
		: slotNumber(slotnum)
		, ctrlCmd(cmd)
		, currentAudioStatus(status)
		, volume(-1)
		, fadetime(-1)
	{}
};

}

namespace LIGHT {
enum SceneFlags {
	SCENE_IDLE = 0,
	SCENE_LIVE = 1<<0,
	SCENE_ACTIVE = 1<<1,
	SCENE_STAGE = 1<<2
};
}


#endif // COMMANDSYSTEM_H

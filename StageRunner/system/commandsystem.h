#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

enum CtrlCmd {
	CMD_NONE,
	CMD_AUDIO_START,
	CMD_AUDIO_STOP,
	CMD_AUDIO_FADEIN,
	CMD_AUDIO_FADEOUT,
	CMD_AUDIO_CHANGE_VOL,

	CMD_STATUS_REPORT = CMD_NONE,
	CMD_FX_START = CMD_AUDIO_START,
	CMD_FX_STOP = CMD_AUDIO_STOP
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
	AudioCtrlMsg(AudioStatus status = AUDIO_IDLE, int slotnum = -1)
		: slotNumber(slotnum)
		, ctrlCmd(CMD_NONE)
		, currentAudioStatus(status)
		, volume(0)
		, fadetime(0)
	{}
};

}


#endif // COMMANDSYSTEM_H

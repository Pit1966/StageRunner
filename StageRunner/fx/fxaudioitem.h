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

#ifndef FXAUDIOITEM_H
#define FXAUDIOITEM_H

#include "fxitem.h"
#include <QStringList>

using namespace AUDIO;

class FxList;

class FxAudioItem : public FxItem
{
public:
	enum AttachedCmd {
		ATTACHED_CMD_NONE,
		ATTACHED_CMD_FADEOUT_ALL,					///< Fadeout all audio FX
		ATTACHED_CMD_STOP_ALL,						///< Stop all audio FX
		ATTACHED_CMD_STOP_VIDEO,
		ATTACHED_CMD_START_FX,
		ATTACHED_CMD_STOP_ALL_SEQ_AND_SCRIPTS,
		ATTACHED_CMD_SET_MASTER_VOL,

		ATTACHED_CMD_CNT
	};
	static QStringList attachedCmdStrings;

	qint32 initialVolume;
	qint32 currentVolume;
	qint64 audioDuration;					///< Length of audio stream in ms (or 0, if unknown)
	qint64 initialSeekPos;					///< This is the time in ms the audio file should start on play
	qint64 stopAtSeekPos;					///< Playing of audio will be stopped at this time (ms) (if not NULL)
	qint32 loopTimes;
	qint32 playBackSlot;					///< The default playback slot (channel). 0 means auto select
	qint32 attachedStartCmd;				///< This is a command that is executed at the same time this Fx is started
	qint32 attachedStopCmd;
	qint32 attachedStartPara1;
	qint32 attachedStartPara2;
	qint32 attachedStopPara1;
	qint32 attachedStopPara2;
	pbool blackAtVideoEnd;

	bool startInProgress;					///< set this Flag to give a hint, that this effect will start in a short time
	bool isDmxStarted;						///< Flag shows that start through DMX has occured (must be resseted before new DMX driven start is allowed)
	bool isFxClip;							///< True, if Audio is recognized to be a video clip
	bool isPicClip;							///< True, if Audio is recognized as still picture

	qint32 tmpMasterVolAtStart;				///< Mastervolume, if started with master volume changer

protected:
	AudioSeqState mySeqStatus;
	qint64 mySeekPosition;					/// this is the time in ms the sound has been stopped or paused
	int mySeekPosPerMille;					/// this is the seek position in per mille (-1 means: not known, maybe audio duration is not available)

public:
	FxAudioItem(FxList *fxList);
	FxAudioItem(const FxAudioItem &o);
	FxAudioItem(const QString &path, FxList *fxList);
	AudioSeqState seqStatus() const {return mySeqStatus;}
	void setSeqStatus(AudioSeqState state) {mySeqStatus = state;}
	qint32 loopValue() const override {return loopTimes;}
	void setLoopValue(qint32 val) override {loopTimes = val;}
	void initForSequence() override;
	void resetFx() override;
	inline qint64 seekPosition() const {return mySeekPosition;}
	inline int seekPosPerMille() const {return mySeekPosPerMille;}
	void setSeekPosition(qint64 posMs);

	bool checkForVideoClip();

private:
	void init();
};

#endif // FXAUDIOITEM_H

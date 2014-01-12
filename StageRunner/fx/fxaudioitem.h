#ifndef FXAUDIOITEM_H
#define FXAUDIOITEM_H

#include "fxitem.h"

using namespace AUDIO;

class FxList;

class FxAudioItem : public FxItem
{
public:
	qint32 initialVolume;
	qint32 currentVolume;
	qint64 audioDuration;					///< Length of audio stream in ms (or 0, if unknown)
	qint64 initialSeekPos;					///< This is the time in ms the audio file should start on play
	qint64 stopAtSeekPos;					///< Playing of audio will be stopped at this time (ms) (if not NULL)
	qint32 loopTimes;

	bool startInProgress;					///< set this Flag to give a hint, that this effect will start in a short time
	bool isDmxStarted;						///< Flag shows that start through DMX has occured (must be resseted before new DMX driven start is allowed)

protected:
	AudioSeqState mySeqStatus;
	qint64 mySeekPosition;					/// this is the time in ms the sound has been stopped or paused
	int mySeekPosPerMille;					/// this is the seek position in per mille (-1 means: not known, maybe audio duration is not available)

public:
	FxAudioItem(FxList *fxList);
	FxAudioItem(const FxAudioItem &o);
	FxAudioItem(const QString &path, FxList *fxList);
	void setFilePath(const QString &path);
	AudioSeqState seqStatus() const {return mySeqStatus;}
	void setSeqStatus(AudioSeqState state) {mySeqStatus = state;}
	qint32 loopValue() {return loopTimes;}
	void setLoopValue(qint32 val) {loopTimes = val;}
	void initForSequence();
	void resetFx();
	inline qint64 seekPosition() const {return mySeekPosition;}
	inline int seekPosPerMille() const {return mySeekPosPerMille;}
	void setSeekPosition(qint64 posMs);

private:
	void init();
};

#endif // FXAUDIOITEM_H

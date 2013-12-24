#ifndef FXAUDIOITEM_H
#define FXAUDIOITEM_H

#include "fxitem.h"

using namespace AUDIO;

class FxAudioItem : public FxItem
{
public:
	qint32 initialVolume;
	qint32 currentVolume;
	qint64 audioDuration;					///< Length of audio stream in ms (or 0, if unknown)
	qint32 loopTimes;

protected:
	AudioSeqState mySeqStatus;

public:
	FxAudioItem();
	FxAudioItem(const FxAudioItem &o);
	FxAudioItem(const QString &path);
	void setFilePath(const QString &path);
	AudioSeqState seqStatus() const {return mySeqStatus;}
	void setSeqStatus(AudioSeqState state) {mySeqStatus = state;}
	qint32 loopValue() {return loopTimes;}
	void setLoopValue(qint32 val) {loopTimes = val;}
	void initForSequence();

private:
	void init();
};

#endif // FXAUDIOITEM_H

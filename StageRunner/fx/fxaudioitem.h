#ifndef FXAUDIOITEM_H
#define FXAUDIOITEM_H

#include "fxitem.h"

class FxAudioItem : public FxItem
{
public:
	qint32 initialVolume;
	qint32 currentVolume;
	qint64 audioDuration;					///< Length of audio stream in ms (or 0, if unknown)
	qint32 loopTimes;

public:
	FxAudioItem();
	FxAudioItem(const QString &path);
	void setFilePath(const QString &path);

private:
	void init();
};

#endif // FXAUDIOITEM_H

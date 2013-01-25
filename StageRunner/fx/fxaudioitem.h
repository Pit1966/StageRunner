#ifndef FXAUDIOITEM_H
#define FXAUDIOITEM_H

#include "fxitem.h"

class FxAudioItem : public FxItem
{
public:
	qint32 initialVolume;
	qint32 currentVolume;

public:
	FxAudioItem();
	FxAudioItem(const QString &path);

private:
	void init();
};

#endif // FXAUDIOITEM_H

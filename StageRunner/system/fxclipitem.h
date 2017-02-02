#ifndef FXCLIPITEM_H
#define FXCLIPITEM_H

#include "fxaudioitem.h"

class FxList;

class FxClipItem : public FxAudioItem
{
public:
	pbool blackAtVideoEnd;

public:
	FxClipItem(FxList *fxList);

private:
	void init();
};

#endif // FXCLIPITEM_H

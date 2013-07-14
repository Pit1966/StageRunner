#ifndef FXPLAYLISTITEM_H
#define FXPLAYLISTITEM_H

#include "fxitem.h"
#include "fxaudioitem.h"
#include "varsetlist.h"

class FxList;


class FxPlayListItem : public FxAudioItem
{
public:
	FxList *fxPlayList;

public:
	FxPlayListItem();
	~FxPlayListItem();
	bool addAudioTrack(const QString & path);
	int size();

private:
	void init();
};

#endif // FXPLAYLISTITEM_H

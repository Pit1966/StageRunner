#ifndef FXPLAYLISTITEM_H
#define FXPLAYLISTITEM_H

#include "fxaudioitem.h"
#include "varsetlist.h"

class FxPlayListItem : public FxAudioItem
{
public:
	VarSetList<FxAudioItem*>playList;

public:
	FxPlayListItem();
	bool addAudioTrack(const QString & path);
	inline int size() {return playList.size();}

private:
	void init();
};

#endif // FXPLAYLISTITEM_H

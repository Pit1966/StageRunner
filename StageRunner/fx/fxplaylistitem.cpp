#include "fxplaylistitem.h"
#include "fxlist.h"

//#ifdef IS_QT5
//#include <QtWidgets>
//#endif


FxPlayListItem::FxPlayListItem()
	: FxAudioItem()
	, fxPlayList(new FxList)
{
	init();
}

FxPlayListItem::~FxPlayListItem()
{
	delete fxPlayList;
}

bool FxPlayListItem::addAudioTrack(const QString &path)
{
	FxAudioItem *fxa = new FxAudioItem(path);
	if (fxa) {
		fxPlayList->append(fxa);
		return true;
	}
	return false;
}

int FxPlayListItem::size()
{
	return fxPlayList->size();
}

void FxPlayListItem::init()
{

	// Base initializing was done in FxAudioItem()

	myFxType = FX_AUDIO_PLAYLIST;

	addExistingVarSetList(fxPlayList->nativeFxList(),"AudioPlayList",PrefVarCore::FX_AUDIO_ITEM);

}

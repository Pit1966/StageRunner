#include "fxplaylistitem.h"

//#ifdef IS_QT5
//#include <QtWidgets>
//#endif


FxPlayListItem::FxPlayListItem()
	: FxAudioItem()
{
	init();
}

bool FxPlayListItem::addAudioTrack(const QString &path)
{
	FxAudioItem *fxa = new FxAudioItem(path);
	if (fxa) {
		playList.append(fxa);
		return true;
	}
	return false;
}

void FxPlayListItem::init()
{
	// Base initializing was done in FxAudioItem()

	myFxType = FX_AUDIO_PLAYLIST;

	addExistingVarSetList(playList,"AudioPlayList",PrefVarCore::FX_AUDIO_ITEM);

}

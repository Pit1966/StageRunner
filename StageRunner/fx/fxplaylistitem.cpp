#include "fxplaylistitem.h"
#include "fxlist.h"

#include "appcentral.h"
#include "audiocontrol.h"
#include "fxitemobj.h"

//#ifdef IS_QT5
//#include <QtWidgets>
//#endif


FxPlayListItem::FxPlayListItem()
	: FxAudioItem()
	, fxPlayList(new FxList)
	, itemObj(new FxItemObj(this))
{

	init();
}

FxPlayListItem::~FxPlayListItem()
{
	delete fxPlayList;
	delete itemObj;
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
	myclass = PrefVarCore::FX_PLAYLIST_ITEM;

	addExistingVar(widgetPos,"WidgetPos");
	addExistingVarSetList(fxPlayList->nativeFxList(),"AudioPlayList",PrefVarCore::FX_AUDIO_ITEM);
}

void FxPlayListItem::continuePlay(FxItem *fx, CtrlCmd cmd, Executer *exec)
{
	Q_UNUSED(exec);

	if (!FxItem::exists(fx) || fx->fxType() != FX_AUDIO) return;
	FxAudioItem *fxa = reinterpret_cast<FxAudioItem*>(fx);

	switch (cmd) {
	case CMD_AUDIO_START:
		AppCentral::instance()->unitAudio->continueFxAudioPlayList(this,fxa);
		break;
	default:
		break;
	}

}

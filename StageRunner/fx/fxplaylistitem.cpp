#include "fxplaylistitem.h"
#include "fxlist.h"

#include "appcentral.h"
#include "audiocontrol.h"
#include "fxitemobj.h"
#include "fxcontrol.h"


FxPlayListItem::FxPlayListItem(FxList *fxList)
	: FxAudioItem(fxList)
	, fxPlayList(new FxList(this))
	, itemObj(new FxItemObj(this))
{
	init();
}

FxPlayListItem::FxPlayListItem(const FxPlayListItem &o)
	: FxAudioItem(o.myParentFxList)
	, fxPlayList(new FxList(this))
	, itemObj(new FxItemObj(this))
{
	init();
	copyFrom(o);
}

FxPlayListItem::~FxPlayListItem()
{
	delete fxPlayList;
	delete itemObj;
}

void FxPlayListItem::copyFrom(const FxPlayListItem &o)
{
	cloneFrom(o);
	fxPlayList->copyFrom(*o.fxPlayList);
}

void FxPlayListItem::setLoopValue(qint32 val)
{
	loopTimes = val;
	fxPlayList->setLoopTimes(val);
}

qint32 FxPlayListItem::loopValue() const
{
	return loopTimes;
}

bool FxPlayListItem::isRandomized()
{
	return fxPlayList->myRandomizedFlag;
}

void FxPlayListItem::setRandomized(bool state)
{
	fxPlayList->myRandomizedFlag = state;
}

bool FxPlayListItem::addAudioTrack(const QString &path)
{
	FxAudioItem *fxa = new FxAudioItem(path,fxPlayList);
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
	addExistingVar(fxPlayList->myLoopFlag,"ListIsLooped");
	// addExistingVar(fxPlayList->myLoopTimes,"LoopTimes",1,1000000,1);
	addExistingVar(fxPlayList->myRandomizedFlag,"RandomizedList");
	addExistingVar(fxPlayList->myAutoProceedFlag,"AutoProceedList");
	addExistingVarSetList(fxPlayList->nativeFxList(),"AudioPlayList",PrefVarCore::FX_AUDIO_ITEM);
}

void FxPlayListItem::continuePlay(FxItem *fx, CtrlCmd cmd, Executer *exec)
{
	Q_UNUSED(exec);

	if (!FxItem::exists(fx) || fx->fxType() != FX_AUDIO) return;
	FxAudioItem *fxa = reinterpret_cast<FxAudioItem*>(fx);

	switch (cmd) {
	case CMD_AUDIO_START:
		AppCentral::instance()->unitFx->continueFxAudioPlayList(this,fxa);
		break;
	default:
		break;
	}

}

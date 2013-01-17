#include "fxlist.h"
#include "fxaudioitem.h"

FxList::FxList() :
	QObject()
{

}

FxList::~FxList()
{
	clear();
}

void FxList::clear()
{
	while (!fx_list.isEmpty()) {
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = fx_list.takeFirst();
		if (!fx->refCount.deref()) {
			delete fx;
		}
	}
}

bool FxList::addFxAudioSimple(const QString &path)
{
	FxAudioItem *fx = new FxAudioItem(path);
	fx->refCount.ref();

	fx_list.append(fx);

	return true;
}

FxItem *FxList::addFx(int fxtype)
{
	switch (fxtype) {
	case FX_AUDIO:
		{
			FxAudioItem *fx = new FxAudioItem();
			fx->refCount.ref();
			fx_list.append(fx);
			return fx;
		}
		break;
	default:
		break;
	}

	return 0;
}

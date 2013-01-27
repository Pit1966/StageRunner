#include "fxlist.h"
#include "fxaudioitem.h"

FxList::FxList() :
	QObject()
{
	fx_last = 0;
	fx_next = 0;
	fx_current = 0;
	id = 0;
}

FxList::~FxList()
{
	clear();
}

void FxList::clear()
{
	fx_last = 0;
	fx_next = 0;
	fx_current = 0;

	while (!fx_list.isEmpty()) {
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = fx_list.takeFirst();
		if (!fx->refCount.deref()) {
			delete fx;
		}
	}
}

FxItem *FxList::getSequenceNext()
{
	if (!fx_next) return 0;
	fx_last = fx_current;
	fx_current = fx_next;
	return fx_next;
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

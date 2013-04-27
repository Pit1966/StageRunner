#include "fxlist.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"

#include <QMutableListIterator>

FxList::FxList() :
	QObject()
{
	fx_last = 0;
	fx_next = 0;
	fx_current = 0;
	auto_proceed_f = false;
	id = 0;
	modified_f = false;
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
	modified_f = true;

	while (!fx_list.isEmpty()) {
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = fx_list.takeFirst();
		if (!fx->refCount.deref()) {
			delete fx;
		}
	}
}

void FxList::setNextFx(FxItem *nfx)
{
	if (nfx != fx_next) {
		fx_next = nfx;
		emit fxNextChanged(fx_next);
	}
}

FxItem *FxList::stepToSequenceNext()
{
	if (!fx_next) return 0;
	fx_last = fx_current;
	fx_current = fx_next;

	// Now check if auto proceed is selected and find next entry in list
	if (auto_proceed_f) {
		setNextFx( findSequenceFollower(fx_current) );
	}
	return fx_current;
}

FxItem *FxList::getFxByKeyCode(ushort keycode) const
{
	QListIterator<FxItem*> it(fx_list);
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->keyCode() == keycode) return fx;
	}
	return 0;
}

FxItem *FxList::findSequenceFollower(FxItem *curfx)
{
	FxItem *followfx = 0;
	int idx_cur = fx_list.indexOf(curfx);
	if (idx_cur >= 0) {
		if (idx_cur+1 < fx_list.size()) {
			followfx = fx_list.at(idx_cur+1);

		}
	}
	return followfx;
}

bool FxList::addFxAudioSimple(const QString &path, int pos)
{
	FxAudioItem *fx = new FxAudioItem(path);
	fx->refCount.ref();
	if (pos < 0 || pos >= fx_list.size()) {
		fx_list.append(fx);
	} else {
		fx_list.insert(pos,fx);
	}
	modified_f = true;

	return true;
}

bool FxList::addFxScene(int tubes)
{
	FxItem *fx = addFx(FX_SCENE,8);
	fx->setName("New Scene");
}

void FxList::moveFromTo(int srcidx, int destidx)
{
	FxItem *xitem = 0;
	if (srcidx >= 0 && srcidx < fx_list.size()) {
		xitem = fx_list.takeAt(srcidx);
	} else {
		return;
	}

	if (destidx < 0) {
		fx_list.append(xitem);
	}
	else if (srcidx < destidx) {
		fx_list.insert(destidx,xitem);
	}
	else if (destidx <= srcidx ){
		fx_list.insert(destidx,xitem);
	}
	modified_f = true;

	emit fxListChanged();
}

bool FxList::deleteFx(FxItem *fx)
{
	bool del = fx_list.removeOne(fx);
	if (del) {
		if (fx == fx_next) fx_next = 0;
		if (fx == fx_last) fx_last = 0;
		if (fx == fx_current) fx_current = 0;
		delete fx;
		emit fxListChanged();
		modified_f = true;
	}
	return del;
}

bool FxList::isModified() const
{
	bool modified = modified_f;
	QListIterator<FxItem*> it(fx_list);
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->isModified()) {
			modified = true;
		}
	}
	return modified;
}

void FxList::setModified(bool state)
{
	QMutableListIterator<FxItem*> it(fx_list);
	while (it.hasNext()) {
		FxItem *fx = it.next();
		fx->setModified(state);
	}
	modified_f = state;
}

FxItem *FxList::addFx(int fxtype, int option)
{
	switch (fxtype) {
	case FX_AUDIO:
		{
			FxAudioItem *fx = new FxAudioItem();
			fx->refCount.ref();
			fx_list.append(fx);
			modified_f = true;
			return fx;
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *fx = new FxSceneItem();
			fx->refCount.ref();
			fx_list.append(fx);
			modified_f = true;
			if (option >= 0) {
				fx->createDefaultTubes(option);
			}
			return fx;
		}
	default:
		break;
	}

	return 0;
}

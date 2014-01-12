#include "fxitem.h"
#include "fxlist.h"

QList<FxItem*>*FxItem::global_fx_list = 0;


FxItem::FxItem(FxList *fxList)
	: VarSet()
	, myParentFxList(fxList)
	, playedInRandomList(false)
{
	init();
	myId = init_generate_id();
}

FxItem::FxItem(const FxItem &o)
	: VarSet()
	, myParentFxList(0)
	, playedInRandomList(false)
{
	init();

	cloneFrom(o);
	myId = init_generate_id();
}

void FxItem::init()
{
	setClass(PrefVarCore::FX_ITEM,"FxItem");

	addExistingVar(myId,"FxId",0,10000,0);
	varList.last()->setDoNotCopy(true);
	addExistingVar(myFxType,"FxType",FX_NULL,FX_SIZE,FX_NULL);
	addExistingVar(myName,"DisplayName");
	addExistingVar(myFile,"FileName");
	addExistingVar(myPath,"FilePath");
	addExistingVar(myKey,"KeyCode");
	addExistingVar(defaultFadeInTime,"DefFadeInTime");
	addExistingVar(defaultFadeOutTime,"DefFadeOutTime");
	addExistingVar(defaultHoldTime,"DefHoldTime");
	addExistingVar(defaultPreDelay,"DefPreDelay");
	addExistingVar(defaultPostDelay,"DefPostDelay");
	addExistingVar(hookedToInputUniverse,"HookToInputUniverse",0,3,0);
	addExistingVar(hookedToInputDmxChannel,"HookedToInputDmxChannel",-1,511,-1);
}

int FxItem::init_generate_id()
{
	myId = 0;
	// hold a backup pointer to every effect in a global list
	if (!global_fx_list) {
		// create that list if not happend before
		global_fx_list = new QList<FxItem*>;
	}
	global_fx_list->append(this);

	// generate an unique id for the effect
	int new_id = 0;
	for (int t=0; t<global_fx_list->size(); t++) {
		int t_id = global_fx_list->at(t)->myId;
		if (t_id >= new_id)
			new_id = t_id + 1;
	}

	return new_id;
}


FxItem::~FxItem()
{
	// remove the reference to this effect from global list
	if (! global_fx_list->removeOne(this)) {
		// qDebug("%s: FxItem not found in global list",__func__);
	}

	// delete list if empty
	if (global_fx_list->isEmpty()) {
		delete global_fx_list;
		global_fx_list = 0;
	}
}

bool FxItem::exists(FxItem *item)
{
	return global_fx_list->contains(item);
}

/**
 * @brief Get the parent FxItem
 * @return Pointer to FxItem or NULL if no parent exists (e.g. FxItem is part of main fx list)
 *
 * If an FxItem is in the FxList of a FxSeqItem or FxAudioItem than it has a parent FxItem that contains
 * this list. So this function evaluates if a parent FxItem exists and returns a pointer to it
 */
FxItem *FxItem::parentFxItem()
{
	FxItem *parentfx = 0;
	if (myParentFxList) {
		parentfx = myParentFxList->parentFx();
	}
	return parentfx;
}

void FxItem::setName(const QString &name)
{
	if (name != myName) {
		myName = name;
		setModified(true);
	}
}

void FxItem::setKeyCode(int code)
{
	if (code == Qt::Key_Escape || code == Qt::Key_Backspace)
		code = 0;
	if (myKey != code) {
		myKey = code;
		setModified(true);
	}
}

bool FxItem::isHookedToInput(qint32 universe, qint32 channel)
{
	return hookedToInputUniverse == universe && hookedToInputDmxChannel == channel;
}

void FxItem::hookToInput(qint32 universe, qint32 channel)
{
	hookedToInputUniverse = universe;
	hookedToInputDmxChannel = channel;
}

void FxItem::setFadeInTime(qint32 val)
{
	if (defaultFadeInTime != val) {
		defaultFadeInTime = val;
		setModified(true);
	}
}

void FxItem::setFadeOutTime(qint32 val)
{
	if (defaultFadeOutTime != val) {
		defaultFadeOutTime = val;
		setModified(true);
	}
}

void FxItem::setPreDelay(qint32 val)
{
	if (defaultPreDelay != val) {
		defaultPreDelay = val;
		setModified(true);
	}
}

void FxItem::setPostDelay(qint32 val)
{
	if (defaultPostDelay != val) {
		defaultPostDelay = val;
		setModified(true);
	}
}

void FxItem::setHoldTime(qint32 val)
{
	if (defaultHoldTime != val) {
		defaultHoldTime = val;
		setModified(true);
	}
}



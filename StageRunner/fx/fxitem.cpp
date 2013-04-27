#include "fxitem.h"

QList<const FxItem*>*FxItem::global_fx_list = 0;


FxItem::FxItem()
	: VarSet()
{
	myId = 0;
	// hold a backup pointer to every effect in a global list
	if (!global_fx_list) {
		// create that list if not happend before
		global_fx_list = new QList<const FxItem*>;
	}
	global_fx_list->append(this);

	// generate an unique id for the effect
	int new_id = 0;
	for (int t=0; t<global_fx_list->size(); t++) {
		int t_id = global_fx_list->at(t)->myId;
		if (t_id >= new_id)
			new_id = t_id + 1;
	}

	init(new_id);

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

bool FxItem::exists(const FxItem *item)
{
	return global_fx_list->contains(item);
}

void FxItem::init(qint32 id)
{
	// qDebug("init id: %d",id);
	setClass(PrefVarCore::FX_ITEM,"FxItem");

	addExistingVar(myId,"FxId",0,10000,id);
	addExistingVar(myType,"FxType",FX_NULL,FX_SIZE,FX_NULL);
	addExistingVar(myName,"DisplayName");
	addExistingVar(myFile,"FileName");
	addExistingVar(myPath,"FilePath");
	addExistingVar(myKey,"KeyCode");
}

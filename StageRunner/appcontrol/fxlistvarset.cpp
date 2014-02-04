#include "fxlistvarset.h"
#include "fxlist.h"

FxListVarSet::FxListVarSet()
	: VarSet()
	, m_fxList(new FxList())
{
	init();
}

FxListVarSet::~FxListVarSet()
{
	delete m_fxList;
}

FxList *FxListVarSet::fxList()
{
	return m_fxList;
}

bool FxListVarSet::isModified()
{
	return VarSet::isModified() | m_fxList->isModified();
}

void FxListVarSet::init()
{
	setClass(PrefVarCore::VAR_SET_CLASS,"FxList");
	setDescription("This VarSet holds a list of FxItems");

	addExistingVarSetList(m_fxList->nativeFxList(),"FxList",PrefVarCore::FX_ITEM);

}

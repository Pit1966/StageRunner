#include "fxscriptitem.h"

FxScriptLine::FxScriptLine()
{
}

FxScriptLine::FxScriptLine(const FxScriptLine &o)
	: m_cmd(o.m_cmd)
	, m_paras(o.m_paras)
{
}

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

FxScriptList::FxScriptList()
	: QList<FxScriptLine>()
{

}

FxScriptList::FxScriptList(const FxScriptList &o)
	: QList<FxScriptLine>(o)
{

}

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

FxScriptItem::FxScriptItem(FxList *fxList)
	: FxItem(fxList)
{
	init();
}

FxScriptItem::FxScriptItem(const FxScriptItem &o)
	: FxItem(o.myParentFxList)
{
	init();
	cloneFrom(o);
}

qint32 FxScriptItem::loopValue() const
{
	return 0;
}

void FxScriptItem::setLoopValue(qint32 val)
{
	Q_UNUSED(val)
}

void FxScriptItem::resetFx()
{
}

void FxScriptItem::init()
{
	myFxType = FX_SCRIPT;
	myclass = PrefVarCore::FX_SCRIPT_ITEM;

	addExistingVar(m_scriptRaw,"ScriptText");
}


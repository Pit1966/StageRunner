#include "fxscripttools.h"

FxScriptLine::FxScriptLine(const QString &cmd, const QString &paras)
	: m_lineNum(0)
	, m_cmd(cmd)
	, m_paras(paras)
{
}

FxScriptLine::FxScriptLine(const FxScriptLine &o)
	: m_lineNum(o.m_lineNum)
	, m_cmd(o.m_cmd)
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

void FxScriptList::clear()
{
	QList<FxScriptLine>::clear();
}

void FxScriptList::append(const FxScriptLine &scriptLine)
{
	QList::append(scriptLine);
	last().m_lineNum = QList::size();
}

FxScriptLine *FxScriptList::at(int lineNum)
{
	if (lineNum < 0 || lineNum >= QList::size())
		return 0;

	return &QList::operator[](lineNum);
}

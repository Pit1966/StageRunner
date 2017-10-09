#include "fxscripttools.h"

using namespace SCRIPT;

ScriptKeyWord::ScriptKeyWord()
{
	m_keywordHash.insert("wait",KW_WAIT);
	m_keywordHash.insert("start",KW_START);
	m_keywordHash.insert("stop",KW_STOP);
	m_keywordHash.insert("fadein",KW_FADEIN);
}

QString ScriptKeyWord::keyWord(SCRIPT::KEY_WORD keyword)
{
	if (keyword < KW_START || keyword >= KW_COUNT)
		return QString("n/a");

	return m_keywordHash.key(keyword);
}

SCRIPT::KEY_WORD ScriptKeyWord::keyNumber(const QString &text)
{
	if (m_keywordHash.contains(text))
		return m_keywordHash.value(text);

	return KW_NONE;
}

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

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

ScriptKeyWord FxScriptList::keywords;

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

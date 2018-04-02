#include "fxscriptitem.h"

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

/**
 * @brief This function generates the internal ScriptList from raw text
 * @return
 */
bool FxScriptItem::updateScript()
{
	int linecnt = rawToScript(m_scriptRaw, m_script);
	if (linecnt < 0)
		return false;

	return true;
}

int FxScriptItem::rawToScript(const QString &rawlines, FxScriptList &scriptlist)
{
	QStringList lines = rawlines.split(';');
	int linecnt = 0;
	foreach (QString line, lines) {
		QString cmd = line.section(' ',0,0,QString::SectionSkipEmpty);
		QString paras = line.section(' ',1,-1,QString::SectionSkipEmpty);
		FxScriptLine sl(cmd,paras);
		scriptlist.append(sl);
		linecnt++;
	}

	return linecnt;
}

void FxScriptItem::init()
{
	myFxType = FX_SCRIPT;
	myclass = PrefVarCore::FX_SCRIPT_ITEM;

	addExistingVar(m_scriptRaw,"ScriptText");
}


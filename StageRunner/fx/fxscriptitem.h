#ifndef FXSCRIPTITEM_H
#define FXSCRIPTITEM_H

#include "fxitem.h"
#include <QList>


class FxScriptLine
{
private:
	QString m_cmd;
	QString m_paras;

public:
	FxScriptLine();
	FxScriptLine(const FxScriptLine &o);
};

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

class FxScriptList : public QList<FxScriptLine>
{
public:
	FxScriptList();
	FxScriptList(const FxScriptList &o);
};

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

class FxScriptItem : public FxItem
{
private:
	QString m_scriptRaw;
	FxScriptList m_script;

public:
	FxScriptItem(FxList *fxList);
	FxScriptItem(const FxScriptItem &o);

	qint32 loopValue() const;
	void setLoopValue(qint32 val);
	void resetFx();

private:
	void init();

	friend class FxScriptWidget;
};

#endif // FXSCRIPTITEM_H

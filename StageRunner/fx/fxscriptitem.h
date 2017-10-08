#ifndef FXSCRIPTITEM_H
#define FXSCRIPTITEM_H

#include "fxitem.h"
#include "fxscripttools.h"

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

	inline const QString & rawScript() const {return m_scriptRaw;}
	bool updateScript();

	static int rawToScript(const QString &rawlines, FxScriptList &scriptlist);

private:
	void init();

	friend class FxScriptWidget;
};

#endif // FXSCRIPTITEM_H

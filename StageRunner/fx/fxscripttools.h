#ifndef FXSCRIPTTOOLS_H
#define FXSCRIPTTOOLS_H

#include <QString>
#include <QList>
#include <QHash>


namespace SCRIPT {

enum KEY_WORD {
	KW_NONE,
	KW_WAIT,
	KW_START,
	KW_STOP,
	KW_FADEIN,
	KW_FADEOUT,
	KW_LOOP,
	KW_YADI_DMX_MERGE,
	KW_GRAP_SCENE,
	KW_COUNT
};

}

class ScriptKeyWord
{
private:
	QHash<QString,SCRIPT::KEY_WORD> m_keywordHash;

public:
	ScriptKeyWord();
	QString keyWord(SCRIPT::KEY_WORD keyword);
	SCRIPT::KEY_WORD keyNumber(const QString &text);

	QStringList keywordList() const {return m_keywordHash.keys();}
};

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

class FxScriptLine
{
private:
	int m_lineNum;
	int m_loopCount;
	QString m_cmd;
	QString m_paras;

public:
	FxScriptLine(const QString &cmd, const QString &paras);
	FxScriptLine(const FxScriptLine &o);

	int lineNumber() const {return m_lineNum;}
	const QString & command() const {return m_cmd;}
	const QString & parameters() const {return m_paras;}
	int loopCount() const {return m_loopCount;}
	void incLoopCount() {m_loopCount++;}
	void clearLoopCount() {m_loopCount = 0;}

	friend class FxScriptList;
};

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

class FxScriptList : protected QList<FxScriptLine>
{
public:
	static ScriptKeyWord keywords;

public:
	FxScriptList();
	FxScriptList(const FxScriptList &o);
	void clear();
	void append(const FxScriptLine &scriptLine);
	inline int size() const {return QList::size();}
	FxScriptLine *at(int lineNum);
};

#endif // FXSCRIPTTOOLS_H

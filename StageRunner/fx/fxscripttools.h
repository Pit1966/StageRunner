//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

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
	KW_COUNT,
	KW_BLACK,
	KW_REMOTE,
	KW_FADE_VOL
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
public:
	static ScriptKeyWord keywords;

private:
	SCRIPT::KEY_WORD m_cmdKey;
	int m_lineNum;
	int m_loopCount;
	int m_execTimeMs;
	int m_execDurationMs;
	QString m_cmd;
	QString m_paras;

public:
	FxScriptLine(const QString &cmd, const QString &paras);
	FxScriptLine(const QString &time, const QString &cmd, const QString &paras);
	FxScriptLine(const FxScriptLine &o);

	int lineNumber() const {return m_lineNum;}
	const QString & command() const {return m_cmd;}
	const QString & parameters() const {return m_paras;}
	int loopCount() const {return m_loopCount;}
	void incLoopCount() {m_loopCount++;}
	void clearLoopCount() {m_loopCount = 0;}
	int execDuration();
	inline int execTimeMs() const {return m_execTimeMs;}

private:
	void calculateDuration();

	friend class FxScriptList;
};

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

class FxScriptList : protected QList<FxScriptLine>
{
private:
	qint64 m_execDurationMs;

public:
	FxScriptList();
	FxScriptList(const FxScriptList &o);
	void clear();
	void append(const FxScriptLine &scriptLine);
	inline int size() const {return QList::size();}
	FxScriptLine *at(int lineNum);
	qint64 execDuration();

private:
	void calculateDuration();
};

#endif // FXSCRIPTTOOLS_H

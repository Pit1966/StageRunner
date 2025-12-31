//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include "fxscripttools.h"
#include "qtstatictools.h"

using namespace SCRIPT;

ScriptKeyWord::ScriptKeyWord()
{
	m_keywordHash.insert("wait",KW_WAIT);
	m_keywordHash.insert("start",KW_START);
	m_keywordHash.insert("stop",KW_STOP);
	m_keywordHash.insert("fadein",KW_FADEIN);
	m_keywordHash.insert("fadeout",KW_FADEOUT);
	m_keywordHash.insert("loop",KW_LOOP);
	m_keywordHash.insert("dmxmerge",KW_YADI_DMX_MERGE);
	m_keywordHash.insert("grapscene",KW_GRAP_SCENE);
	m_keywordHash.insert("black",KW_BLACK);
	m_keywordHash.insert("remote",KW_REMOTE);
	m_keywordHash.insert("fadevol",KW_FADE_VOL);
	m_keywordHash.insert("mode",KW_MODE);
	m_keywordHash.insert("pause",KW_PAUSE);
	m_keywordHash.insert("dmx",KW_DMX);
	m_keywordHash.insert("fix",KW_FIX);
	m_keywordHash.insert("default",KW_DEFAULT);
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
ScriptKeyWord FxScriptLine::keywords;


FxScriptLine::FxScriptLine(const QString &cmd, const QString &paras)
	: m_cmdKey(KW_NONE)
	, m_lineNum(0)
	, m_loopCount(0)
	, m_execTimeMs(-1)
	, m_execDurationMs(-1)
	, m_cmd(cmd)
	, m_paras(paras)
{
	m_cmdKey = FxScriptLine::keywords.keyNumber(cmd);
}

FxScriptLine::FxScriptLine(const QString &time, const QString &cmd, const QString &paras)
	: m_cmdKey(KW_NONE)
	, m_lineNum(0)
	, m_loopCount(0)
	, m_execDurationMs(-1)
	, m_cmd(cmd)
	, m_paras(paras)
{
	m_cmdKey = FxScriptLine::keywords.keyNumber(cmd);
	m_execTimeMs = QtStaticTools::timeStringToMS(time);
}

//FxScriptLine::FxScriptLine(const FxScriptLine &o)
//	: m_cmdKey(o.m_cmdKey)
//	, m_lineNum(o.m_lineNum)
//	, m_loopCount(o.m_loopCount)
//	, m_execTimeMs(o.m_execTimeMs)
//	, m_execDurationMs(o.m_execDurationMs)
//	, m_cmd(o.m_cmd)
//	, m_paras(o.m_paras)
//{
//}

int FxScriptLine::execDuration()
{
	if (m_execDurationMs == -1)
		calculateDuration();

	return m_execDurationMs;
}

/**
 * @brief Calculate execution Duration for this script line
 * This stores the execution duration internal in ms. Currently only affected if FxScriptLine is
 * a WAIT command, otherwise the duration is NULL
 */
void FxScriptLine::calculateDuration()
{
	switch (m_cmdKey) {
	case KW_WAIT:
		m_execDurationMs = QtStaticTools::timeStringToMS(m_paras);
		break;
	default:
		m_execDurationMs = 0;
		break;
	}

}

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
FxScriptList::FxScriptList()
	: QList<FxScriptLine>()
	, m_execDurationMs(-1)
{
}

FxScriptList::FxScriptList(const FxScriptList &o)
	: QList<FxScriptLine>(o)
	, m_execDurationMs(o.m_execDurationMs)
{
}

void FxScriptList::clear()
{
	QList<FxScriptLine>::clear();
	m_execDurationMs = 0;
}

void FxScriptList::append(const FxScriptLine &scriptLine)
{
	QList::append(scriptLine);
	last().m_lineNum = QList::size();

	if (m_execDurationMs == -1 || last().execTimeMs() > 0)
		calculateDuration();
	else
		m_execDurationMs += last().execDuration();
}

FxScriptLine *FxScriptList::at(int lineNum)
{
	if (lineNum < 0 || lineNum >= QList::size())
		return 0;

	return &QList::operator[](lineNum);
}

qint64 FxScriptList::execDuration()
{
	if (m_execDurationMs == -1)
		calculateDuration();

	return m_execDurationMs;
}


void FxScriptList::calculateDuration()
{
	int ms = 0;;
	for (FxScriptLine &line : *this) {
		if (line.execTimeMs() > 0) {	// seems to be an absulute execution time for this line
			ms = line.execTimeMs();
			// qDebug("duration %d", ms);
		} else {
			ms += line.execDuration();
			// qDebug("duration ++ %d", ms);
		}
	}

	m_execDurationMs = ms;
}

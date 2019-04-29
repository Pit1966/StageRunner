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
		if (!line.isEmpty() && line.at(0).isDigit()) {
			QString time = line.section(' ',0,0,QString::SectionSkipEmpty);
			QString cmd = line.section(' ',1,1,QString::SectionSkipEmpty);
			QString paras = line.section(' ',2,-1,QString::SectionSkipEmpty);
			FxScriptLine sl(time,cmd,paras);
			scriptlist.append(sl);
		} else {
			QString cmd = line.section(' ',0,0,QString::SectionSkipEmpty);
			QString paras = line.section(' ',1,-1,QString::SectionSkipEmpty);
			FxScriptLine sl(cmd,paras);
			scriptlist.append(sl);
		}
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


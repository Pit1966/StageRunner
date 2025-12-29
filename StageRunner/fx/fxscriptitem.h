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

#ifndef FXSCRIPTITEM_H
#define FXSCRIPTITEM_H

#include "fxitem.h"
#include "fxscripttools.h"

#include <QCoreApplication>

class FxScriptItem : public FxItem
{
	Q_DECLARE_TR_FUNCTIONS(FxScriptItem)

private:
	QString m_scriptRaw;
	FxScriptList m_script;

public:
	FxScriptItem(FxList *fxList);
	FxScriptItem(const FxScriptItem &o);

	qint32 loopValue() const override;
	void setLoopValue(qint32 val) override;
	void resetFx() override;

	inline const QString & rawScript() const {return m_scriptRaw;}
	void setRawScript(const QString &script);
	bool updateScript();

	static int rawToScript(const QString &rawlines, FxScriptList &scriptlist);
	static QString checkScript(const QString &rawlines);

private:
	void init();

	friend class FxScriptWidget;
};

#endif // FXSCRIPTITEM_H

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

#ifndef FXITEMTOOL_H
#define FXITEMTOOL_H

#include <QHash>

class FxItem;
class FxList;
class FxSeqItem;



/**
 * @brief This class implements a mapping from old FxID to new FxID used by copy functions
 */
class FxIdMap : public QHash<qint32,qint32>
{
};

class FxItemTool
{
public:
	FxItemTool();

	static FxItem* cloneFxItem(FxItem *srcItem, bool renameItem = true, int exactClone = 0, FxIdMap *oldNewMap = nullptr);
	static void setClonedFxName(FxItem *srcItem, FxItem *destItem, FxList *fxList = 0);
};

#endif // FXITEMTOOL_H

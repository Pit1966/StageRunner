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

#include "fxitemtool.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxseqitem.h"
#include "fxplaylistitem.h"
#include "fxscriptitem.h"

FxItemTool::FxItemTool()
{
}

/**
 * @brief FxItemTool::cloneFxItem
 * @param srcItem
 * @param renameItem
 * @param exactClone 0 [default]: id and keyCode is not cloned, 1: keyCode is cloned, 2: FxID and keyCode is cloned
 * @return Pointer to new FxItem or NULL on failure
 */
FxItem *FxItemTool::cloneFxItem(FxItem *srcItem, bool renameItem, int exactClone, FxIdMap *oldNewMap)
{
	if (!FxItem::exists(srcItem)) return nullptr;

	FxItem *newFx = nullptr;

	switch (srcItem->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxaudio = static_cast<FxAudioItem*>(srcItem);
			FxAudioItem *new_fxaudio = new FxAudioItem(*fxaudio);
			new_fxaudio->refCount.ref();
			newFx = new_fxaudio;
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *scene = static_cast<FxSceneItem*>(srcItem);
			FxSceneItem *new_scene = new FxSceneItem(*scene);
			new_scene->refCount.ref();
			newFx = new_scene;
		}
		break;
	case FX_AUDIO_PLAYLIST:
		{
			FxPlayListItem *fxplay = static_cast<FxPlayListItem*>(srcItem);
			FxPlayListItem *new_fxplay = new FxPlayListItem(*fxplay);
			new_fxplay->refCount.ref();
			newFx = new_fxplay;
		}
		break;
	case FX_SEQUENCE:
		{
			FxSeqItem *seqitem = static_cast<FxSeqItem*>(srcItem);
			FxSeqItem *new_seqitem = new FxSeqItem(*seqitem);
			new_seqitem->refCount.ref();
			newFx = new_seqitem;
		}
		break;

	case FX_SCRIPT:
		{
			FxScriptItem *scriptitem = static_cast<FxScriptItem*>(srcItem);
			FxScriptItem *new_scriptitem = new FxScriptItem(*scriptitem);
			new_scriptitem->refCount.ref();
			newFx = new_scriptitem;
		}
		break;

	default:
		break;

	}

	if (newFx) {
		if (renameItem)
			setClonedFxName(srcItem,newFx);
		if (exactClone == 0)
			newFx->setKeyCode(0);
		else if (exactClone > 1)
			newFx->myId = srcItem->myId;

		if (oldNewMap) {
			if (!oldNewMap->contains(srcItem->id())) {
				oldNewMap->insert(srcItem->id(), newFx->id());
			} else {
				qWarning("FxItem ID %d used more than once!",srcItem->id());
			}
		}
	}

	return newFx;
}

void FxItemTool::setClonedFxName(FxItem *srcItem, FxItem *destItem, FxList *fxList)
{
	Q_UNUSED(fxList);

	int pos = srcItem->name().indexOf("_cp");
	if (pos > 0) {
		int num = srcItem->name().mid(pos + 3).toInt();
		QString destname = srcItem->name().left(pos) + "_cp" + QString::number(num+1);
		destItem->setName(destname);
	}
	else {
		destItem->setName(srcItem->name() + "_cp");
	}
}


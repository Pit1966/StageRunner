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
 * @param exactClone 0 [default]: id and keyCode is not cloned, 1: keyCode is cloned
 * @return
 */
FxItem *FxItemTool::cloneFxItem(FxItem *srcItem, bool renameItem, int exactClone)
{
	if (!FxItem::exists(srcItem)) return nullptr;

	FxItem *newFx = nullptr;

	switch (srcItem->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxaudio = reinterpret_cast<FxAudioItem*>(srcItem);
			FxAudioItem *new_fxaudio = new FxAudioItem(*fxaudio);
			new_fxaudio->refCount.ref();
			newFx = new_fxaudio;
			if (renameItem)
				setClonedFxName(fxaudio,new_fxaudio);
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *scene = reinterpret_cast<FxSceneItem*>(srcItem);
			FxSceneItem *new_scene = new FxSceneItem(*scene);
			new_scene->refCount.ref();
			newFx = new_scene;
			if (renameItem)
				setClonedFxName(scene,new_scene);
		}
		break;
	case FX_AUDIO_PLAYLIST:
		qWarning() << __PRETTY_FUNCTION__ << "implement me!!";
		break;
	case FX_SEQUENCE:
		{
			FxSeqItem *seqitem = reinterpret_cast<FxSeqItem*>(srcItem);
			FxSeqItem *new_seqitem = new FxSeqItem(*seqitem);
			new_seqitem->refCount.ref();
			newFx = new_seqitem;
			if (renameItem)
				setClonedFxName(seqitem,new_seqitem);
		}
		break;

	case FX_SCRIPT:
		{
			FxScriptItem *scriptitem = reinterpret_cast<FxScriptItem*>(srcItem);
			FxScriptItem *new_scriptitem = new FxScriptItem(*scriptitem);
			new_scriptitem->refCount.ref();
			newFx = new_scriptitem;
			if (renameItem)
				setClonedFxName(scriptitem,new_scriptitem);
		}
		break;

	default:
		break;

	}

	if (newFx) {
		if (exactClone == 0)
			newFx->setKeyCode(0);
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


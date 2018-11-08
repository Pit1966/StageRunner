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

FxItem *FxItemTool::cloneFxItem(FxItem *srcItem, bool renameItem)
{
	if (!FxItem::exists(srcItem)) return 0;

	FxItem *newFx = 0;

	switch (srcItem->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxaudio = reinterpret_cast<FxAudioItem*>(srcItem);
			FxAudioItem *new_fxaudio = new FxAudioItem(*fxaudio);
			new_fxaudio->refCount.ref();
			newFx = new_fxaudio;
			if(renameItem)
				setClonedFxName(fxaudio,new_fxaudio);
			new_fxaudio->setKeyCode(0);
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *scene = reinterpret_cast<FxSceneItem*>(srcItem);
			FxSceneItem *new_scene = new FxSceneItem(*scene);
			new_scene->refCount.ref();
			newFx = new_scene;
			if(renameItem)
				setClonedFxName(scene,new_scene);
			new_scene->setKeyCode(0);
		}
		break;
	case FX_AUDIO_PLAYLIST:
		break;
	case FX_SEQUENCE:
		{
			FxSeqItem *seqitem = reinterpret_cast<FxSeqItem*>(srcItem);
			FxSeqItem *new_seqitem = new FxSeqItem(*seqitem);
			new_seqitem->refCount.ref();
			newFx = new_seqitem;
			if(renameItem)
				setClonedFxName(seqitem,new_seqitem);
			new_seqitem->setKeyCode(0);
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
			new_scriptitem->setKeyCode(0);
		}
		break;

	default:
		break;

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


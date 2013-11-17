#include "fxitemtool.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxplaylistitem.h"

FxItemTool::FxItemTool()
{
}

FxItem *FxItemTool::cloneFxItem(FxItem *srcItem)
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

			new_fxaudio->setName(fxaudio->name() + "_cp");
			new_fxaudio->setKeyCode(0);
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *scene = reinterpret_cast<FxSceneItem*>(srcItem);
			FxSceneItem *new_scene = new FxSceneItem(*scene);
			new_scene->refCount.ref();
			newFx = new_scene;

			new_scene->setName(scene->name() + "_cp");
			new_scene->setKeyCode(0);
		}
		break;
	case FX_AUDIO_PLAYLIST:
		break;

	default:
		break;

	}

	return newFx;
}

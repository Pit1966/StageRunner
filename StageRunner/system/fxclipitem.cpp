#include "fxclipitem.h"

FxClipItem::FxClipItem(FxList *fxList)
	: FxAudioItem(fxList)
{
}

void FxClipItem::init()
{
	myFxType = FX_CLIP;
	myclass = PrefVarCore::FX_AUDIO_ITEM;

	addExistingVar(blackAtVideoEnd,"BlackAtVideoEnd");

}

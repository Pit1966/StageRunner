#include "fxitemobj.h"
#include "fxitem.h"
#include "fxaudioitem.h"
#include "fxplaylistitem.h"

FxItemObj::FxItemObj(FxItem *fx) :
	QObject()
  , parentFx(fx)
{
}

void FxItemObj::playFxPlayList(FxItem *fx, CtrlCmd cmd, Executer *exec)
{
	if (parentFx->fxType() == FX_AUDIO_PLAYLIST) {
		FxPlayListItem *fxplay = reinterpret_cast<FxPlayListItem*>(parentFx);
		fxplay->continuePlay(fx, cmd, exec);
	}
}

#include "fxsceneitem.h"
#include "dmxchannel.h"

FxSceneItem::FxSceneItem()
{
	myFxType = FX_SCENE;

	init();
}

void FxSceneItem::createDefaultTubes(int tubecount)
{
	for (int t=0; t<tubecount; t++) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tube = t;
		dmx->dmxType = DMX_INTENSITY;
		tubes.append(dmx);
	}
}

void FxSceneItem::init()
{

}

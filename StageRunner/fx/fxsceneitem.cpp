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
		dmx->dmxUniverse = 0;
		dmx->dmxChannel = t;
		dmx->dmxType = DMX_INTENSITY;
		tubes.append(dmx);
	}
}

void FxSceneItem::setTubeCount(int tubecount)
{
	while (tubes.size() > tubecount) {
		delete tubes.takeAt(tubes.size()-1);
	}

	while (tubes.size() < tubecount) {
		DmxChannel *dmx = new DmxChannel;
		dmx->tube = tubes.size();
		dmx->dmxUniverse = 0;
		dmx->dmxChannel = tubes.size();
		dmx->dmxType = DMX_INTENSITY;
		tubes.append(dmx);
	}
}

void FxSceneItem::init()
{
	dmxChannelDummy = new DmxChannel;
	dmxChannelDummy->dmxUniverse = 1;

	addExistingVar(defaultFadeInTime,"DefFadeInTime");
	addExistingVar(defaultFadeOutTime,"DefFadeOutTime");
	addExistingVar(*dmxChannelDummy,"DmxChannelDummy");
	addExistingVarSetList(tubes,"SceneTubes",PrefVarCore::DMX_CHANNEL);

}

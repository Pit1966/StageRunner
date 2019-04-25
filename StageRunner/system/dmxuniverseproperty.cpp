#include "dmxuniverseproperty.h"

DmxUniverseProperty::DmxUniverseProperty()
	: dmxChanLayout(this)
{
	init();
}

void DmxUniverseProperty::initDmxChannels(int universe, int count)
{
	while (!dmxChanLayout.isEmpty())
		delete dmxChanLayout.takeFirst();

	for (int t=0; t<count; t++) {
		DmxChanProperty *prop = new DmxChanProperty();
		prop->dmxUniverse = universe;
		prop->dmxChannel = t;
		dmxChanLayout.append(prop);
	}

	universeLabel = QString("Universe #%1").arg(universe+1);
	dmxUniverse = universe;
	dmxChannelCount = count;
}

void DmxUniverseProperty::init()
{
	setClass(PrefVarCore::DMX_CHANNEL,"DmxUniverseProperty");
	setDescription("A complete configuration of 512 DMX Channels for a DMX universe");

	addExistingVar(universeLabel,"UniverseLabel");
	addExistingVar(dmxUniverse,"UniverseNumber");
	addExistingVar(dmxChannelCount,"UniverseDmxChannelSize",0,512,512);
	addExistingVarSetList(dmxChanLayout,"UniverseLayout",PrefVarCore::DMX_CHANNEL_PROPERTY);

}

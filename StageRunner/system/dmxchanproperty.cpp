#include "dmxchanproperty.h"
#include "config.h"

DmxChanProperty::DmxChanProperty()
{
	init();
}

void DmxChanProperty::init()
{
	setClass(PrefVarCore::DMX_CHANNEL,"DmxChanProperty");
	setDescription("DMX channel configuration");

	addExistingVar(dmxUniverse,"DmxUniverse",0,3,0);
	addExistingVar(dmxChannel,"DmxChannel",0,511,0);
	addExistingVar(dmxType,"DmxType",DMX_GENERIC,DMX_TYPES,DMX_GENERIC);
	addExistingVar(minValue,"MinValue",0,255,0);
	addExistingVar(maxValue,"MaxValue",0,255,255);
	addExistingVar(labelShort,"LabelShort");
	addExistingVar(labelLong,"LabelLong");

}

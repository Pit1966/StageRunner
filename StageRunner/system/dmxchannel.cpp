#include "dmxchannel.h"

DmxChannel::DmxChannel()
	: VarSet()
{
	init();
}

DmxChannel::~DmxChannel()
{
}

void DmxChannel::init()
{
	setClass(PrefVarCore::DMX_CHANNEL,"DmxChannel");
	setDescription("Output mapping from tube number to dmx channel and configuration");
	addExistingVar(tube,"TubeNumber",0,1000,0);
	addExistingVar(dmxType,"DmxType",DMX_GENERIC,DMX_TYPES,DMX_GENERIC);
	addExistingVar(dmxUniverse,"DmxUniverse",0,3,0);
	addExistingVar(dmxChannel,"DmxChannel",0,511,0);
	addExistingVar(dmxValue,"DmxValue",0,255,0);
	addExistingVar(curValue,"Value",0,255,0);
	addExistingVar(targetFullValue,"TargetFullValue",255,100000,10000);
	addExistingVar(targetValue,"TargetValue",0,100000,0);
	addExistingVar(deskVisibleFlag,"DeskVisible",true);
}

#ifndef DMXUNIVERSEPROPERTY_H
#define DMXUNIVERSEPROPERTY_H

#include "dmxchanproperty.h"
#include "varsetlist.h"

class DmxUniverseProperty : public VarSet
{
public:
	qint32 dmxUniverse;							///< Universe number
	qint32 dmxChannelCount;						///< ammount of configured dmx Channels
	QString universeLabel;
	VarSetList<DmxChanProperty*> dmxChanLayout;

public:
	DmxUniverseProperty();
	void initDmxChannels(int universe, int count);

private:
	void init();

};

#endif // DMXUNIVERSEPROPERTY_H

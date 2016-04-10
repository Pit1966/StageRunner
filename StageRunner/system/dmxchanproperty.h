#ifndef DMXCHANPROPERTY_H
#define DMXCHANPROPERTY_H

#include "varset.h"
#include "dmxtypes.h"

class DmxChanProperty : public VarSet
{
public:
	qint32 dmxUniverse;			///< This is the universe this dmx channel lives in (This will be mapped to the output universe later by the plugin configuration)
	qint32 dmxChannel;			///< This is the native DMX channel
	qint32 dmxType;				///< @see DmxChannelType
	qint32 minValue;			///< this is a value between 0 - 255: default 0
	qint32 maxValue;			///< this is a value between 0 - 255: default 255
	QString labelShort;
	QString labelLong;

public:
	DmxChanProperty();

private:
	void init();
};

#endif // DMXCHANPROPERTY_H

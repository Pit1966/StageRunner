#ifndef DMXHELP_H
#define DMXHELP_H

#include "system/dmxtypes.h"
#include <QtCore>

class DMXHelp
{
public:
	DMXHelp();
	static QString dmxTypeToString(DmxChannelType type);
	static QString dmxTypeToShortString(DmxChannelType type);
	static bool isTypeDimmer(DmxChannelType type);
};

#endif // DMXHELP_H

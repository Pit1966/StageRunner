#ifndef TUBEDATA_H
#define TUBEDATA_H

class TubeData
{
public:
	int tubeId		= -1;
	int universe	= 0;
	int dmxChan		= -1;
	int dmxType		= 0;		///< @see DmxChannelType in dmxtypes.h
	int targetValue	= -1;
	int fullValue	= 10000;
	int curValue[2]	= {0,0};	///< current value MIX_INTERN and MIX_EXTERN
};


#endif // TUBEDATA_H

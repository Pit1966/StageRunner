#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

enum DmxChannelType {
	DMX_GENERIC,
	DMX_INTENSITY,
	DMX_PAN,
	DMX_TILT,
	DMX_GOBO,
	DMX_COLOR
};


class DmxChannel : public VarSet
{
protected:
	qint32 myId;
	qint32 myType;
public:
	DmxChannel();
};

#endif // DMXCHANNEL_H

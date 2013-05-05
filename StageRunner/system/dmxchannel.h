#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include "varset.h"
#include "commandsystem.h"

enum DmxChannelType {
	DMX_GENERIC,
	DMX_INTENSITY,
	DMX_PAN,
	DMX_TILT,
	DMX_GOBO,
	DMX_COLOR,


	DMX_TYPES					///< entry count
};


class DmxChannel : public VarSet
{
public:
	/**
	 * This is the internal channel number.
	 * Channels are called tubes and they do not automatically respond to a dmx channel (but can)
	 */
	qint32 tube;
	/**
	 * Type or Function this channel is used for
	 */
	qint32 dmxType;
	qint32 dmxUniverse;					///< DMX universe number
	qint32 dmxChannel;					///< DMX channel the function uses
	qint32 dmxValue;					///< current output value of this dmx channel (0-255)
	qint32 curValue;					///< current output value (0 - targetFullRange)
	qint32 targetFullValue;				///< The maximum value this channel can be set to (default 10000)
	qint32 targetValue;					///< 100% Output value target in a scene (0 - targetFullValue) The dmx Output will be scaled to this
	bool deskVisibleFlag;				///< Show this channel in GUI (scene)

protected:
	// These Vars are mainly for animation/fading control in the scenes
	CtrlCmd curCmd;						///< This is the command currently running on this channel
	float fadeTargetValue;
	float fadeStartValue;
	float fadeValue;
	float fadeStep;

public:
	DmxChannel();
	~DmxChannel();

	bool initFadeCmd(CtrlCmd cmd, qint32 time_ms, qint32 target_value=0 );
	bool loopFunction();

private:
	void init();
};

#endif // DMXCHANNEL_H

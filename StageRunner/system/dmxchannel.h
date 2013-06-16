#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include "varset.h"
#include "commandsystem.h"

#ifndef MIX_LINES
#define MIX_INTERN 0
#define MIX_EXTERN 1
#define MIX_LINES 2
#endif

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
	qint32 curValue[2];					///< current output value (0 - targetFullRange) HTP with directValue
	qint32 targetFullValue;				///< The maximum value this channel can be set to (default 10000)
	qint32 targetValue;					///< 100% Output value target in a scene (0 - targetFullValue) The dmx Output will be scaled to this
	bool deskVisibleFlag;				///< Show this channel in GUI (scene)
	qint32 deskPositionIndex;			///< The Index the Channel will be positioned in the DeskSceneWidget
	QString labelText;					///< Spezielle Beschriftung des Kanals

	int tempTubeListIdx;
	int tempDeskPosIdx;

protected:
	// These Vars are mainly for animation/fading control in the scenes
	CtrlCmd curCmd[MIX_LINES];						///< This is the command currently running on this channel
	float fadeTargetValue[MIX_LINES];
	float fadeStartValue[MIX_LINES];
	float fadeValue[MIX_LINES];
	float fadeStep[MIX_LINES];

public:
	DmxChannel();
	DmxChannel(const DmxChannel &o);
	~DmxChannel();

	bool initFadeCmd(int mixline, CtrlCmd cmd, qint32 time_ms, qint32 target_value=0 );
	bool loopFunction(int mixline);

private:
	void init();
};

#endif // DMXCHANNEL_H

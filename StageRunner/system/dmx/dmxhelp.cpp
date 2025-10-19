#include "dmxhelp.h"


DMXHelp::DMXHelp()
{

}

QString DMXHelp::dmxTypeToString(DmxChannelType type)
{
	switch (type) {
	case DMX_GENERIC:
		return "DMX Generic";

	case DMX_INTENSITY_DIMMER:
		return "DMX Intensity Dimmer";
	case DMX_INTENSITY_DIMMER_FINE:
		return "DMX Intensity Dimmer Fine";


	case DMX_POSITION_PAN:
		return "DMX Position Pan";
	case DMX_POSITION_PAN_FINE:
		return "DMX Position Pan Fine";
	case DMX_POSITION_TILT:
		return "DMX Position TILT";
	case DMX_POSITION_TILT_FINE:
		return "DMX Position Tilt Fine";

	case DMX_COLOR_CTB_MIXER:
		return "DMX Color CTB Mixer";

	default:
		return QString("DMX TYPE #%1").arg(type);
	}
}

QString DMXHelp::dmxTypeToShortString(DmxChannelType type)
{
	switch (type) {
	case DMX_GENERIC:
		return "I";

	case DMX_INTENSITY_DIMMER:
		return "Dim";
	case DMX_INTENSITY_DIMMER_FINE:
		return "DimF";

	case DMX_POSITION_PAN:
		return "Pan";
	case DMX_POSITION_PAN_FINE:
		return "PanF";
	case DMX_POSITION_TILT:
		return "Tilt";
	case DMX_POSITION_TILT_FINE:
		return "TiltF";

	case DMX_COLOR_CTB_MIXER:
		return "CTBcol";

	default:
		return QString("#%1").arg(type);
	}
}

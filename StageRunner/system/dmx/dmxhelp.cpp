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

	case DMX_INTENSITY_RED:
		return "DMX Intensity RED";
	case DMX_INTENSITY_RED_FINE:
		return "DMX Intensity RED Fine";
	case DMX_INTENSITY_GREEN:
		return "DMX Intensity GREEN";
	case DMX_INTENSITY_GREEN_FINE:
		return "DMX Intensity GREEN Fine";
	case DMX_INTENSITY_BLUE:
		return "DMX Intensity BLUE";
	case DMX_INTENSITY_BLUE_FINE:
		return "DMX Intensity BLUE Fine";
	case DMX_INTENSITY_CYAN:
		return "DMX Intensity CYAN";
	case DMX_INTENSITY_CYAN_FINE:
		return "DMX Intensity CYAN Fine";
	case DMX_INTENSITY_MAGENTA:
		return "DMX Intensity MAGENTA";
	case DMX_INTENSITY_MAGENTA_FINE:
		return "DMX Intensity MAGENTA Fine";
	case DMX_INTENSITY_YELLOW:
		return "DMX Intensity YELLOW";
	case DMX_INTENSITY_YELLOW_FINE:
		return "DMX Intensity YELLOW Fine";
	case DMX_INTENSITY_AMBER:
		return "DMX Intensity AMBER";
	case DMX_INTENSITY_AMBER_FINE:
		return "DMX Intensity AMBER Fine";
	case DMX_INTENSITY_WHITE:
		return "DMX Intensity WHITE";
	case DMX_INTENSITY_WHITE_FINE:
		return "DMX Intensity WHITE Fine";
	case DMX_INTENSITY_UV:
		return "DMX Intensity UV";
	case DMX_INTENSITY_UV_FINE:
		return "DMX Intensity UV Fine";
	case DMX_INTENSITY_INDIGO:
		return "DMX Intensity INDIGO";
	case DMX_INTENSITY_INDIGO_FINE:
		return "DMX Intensity INDIGO Fine";
	case DMX_INTENSITY_LIME:
		return "DMX Intensity LIME";
	case DMX_INTENSITY_LIME_FINE:
		return "DMX Intensity LIME Fine";


	case DMX_POSITION_PAN:
		return "DMX Position PAN";
	case DMX_POSITION_PAN_FINE:
		return "DMX Position PAN Fine";
	case DMX_POSITION_TILT:
		return "DMX Position TILT";
	case DMX_POSITION_TILT_FINE:
		return "DMX Position Tilt Fine";

	case DMX_SPEED_PAN_SLOW_FAST:
		return "DMX Speed PAN slow fast";
	case DMX_SPEED_PAN_FAST_SLOW:
		return "DMX Speed PAN fast slow";
	case DMX_SPEED_TILT_SLOW_FAST:
		return "DMX Speed Tilt slow fast";
	case DMX_SPEED_TILT_FAST_SLOW:
		return "DMX Speed TILT fast slow";
	case DMX_SPEED_PAN_TILT_SLOW_FAST:
		return "DMX Speed PAN TILT slow fast";
	case DMX_SPEED_PAN_TILT_FAST_SLOW:
		return "DMX Speed PAN TILT fast slow";
	case DMX_COLOR_MACRO:
		return "DMX Color Macro";
	case DMX_COLOR_WHEEL:
		return "DMX Color Wheel";
	case DMX_COLOR_WHEEL_FINE:
		return "DMX Color Wheel Fine";


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

	case DMX_INTENSITY_RED:
		return "Red";
	case DMX_INTENSITY_RED_FINE:
		return "RedF";
	case DMX_INTENSITY_GREEN:
		return "Green";
	case DMX_INTENSITY_GREEN_FINE:
		return "GreenF";
	case DMX_INTENSITY_BLUE:
		return "Blue";
	case DMX_INTENSITY_BLUE_FINE:
		return "BlueF";
	case DMX_INTENSITY_CYAN:
		return "Cyan";
	case DMX_INTENSITY_CYAN_FINE:
		return "CyanF";
	case DMX_INTENSITY_MAGENTA:
		return "Magenta";
	case DMX_INTENSITY_MAGENTA_FINE:
		return "MagentaF";
	case DMX_INTENSITY_YELLOW:
		return "Yellow";
	case DMX_INTENSITY_YELLOW_FINE:
		return "YellowF";
	case DMX_INTENSITY_AMBER:
		return "Amber";
	case DMX_INTENSITY_AMBER_FINE:
		return "AmberF";
	case DMX_INTENSITY_WHITE:
		return "White";
	case DMX_INTENSITY_WHITE_FINE:
		return "WhiteF";
	case DMX_INTENSITY_UV:
		return "UV";
	case DMX_INTENSITY_UV_FINE:
		return "UVF";
	case DMX_INTENSITY_INDIGO:
		return "Indigo";
	case DMX_INTENSITY_INDIGO_FINE:
		return "IndigoF";
	case DMX_INTENSITY_LIME:
		return "Lime";
	case DMX_INTENSITY_LIME_FINE:
		return "LimeF";



	case DMX_POSITION_PAN:
		return "Pan";
	case DMX_POSITION_PAN_FINE:
		return "PanF";
	case DMX_POSITION_TILT:
		return "Tilt";
	case DMX_POSITION_TILT_FINE:
		return "TiltF";

	case DMX_SPEED_PAN_SLOW_FAST:
		return "SpdPanSF";
	case DMX_SPEED_PAN_FAST_SLOW:
		return "SpdPanFS";
	case DMX_SPEED_TILT_SLOW_FAST:
		return "SpdTiltSF";
	case DMX_SPEED_TILT_FAST_SLOW:
		return "SpdTiltFS";
	case DMX_SPEED_PAN_TILT_SLOW_FAST:
		return "SpdPanTiltSF";
	case DMX_SPEED_PAN_TILT_FAST_SLOW:
		return "SpdPanTiltFS";
	case DMX_COLOR_MACRO:
		return "COLmac";
	case DMX_COLOR_WHEEL:
		return "COLwh";
	case DMX_COLOR_WHEEL_FINE:
		return "COLwhF";


	case DMX_COLOR_CTB_MIXER:
		return "CTBcol";

	default:
		return QString("#%1").arg(type);
	}
}

bool DMXHelp::isTypeDimmer(DmxChannelType type)
{
	return type <= DMX_INTENSITY_VALUE_FINE;
}

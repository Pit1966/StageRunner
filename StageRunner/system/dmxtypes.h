//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef DMXTYPES_H
#define DMXTYPES_H

//enum DmxChannelType {
//	DMX_GENERIC,
//	DMX_Intensity,
//	DMX_PAN,
//	DMX_TILT,
//	DMX_GOBO,
//	DMX_Color_,


//	DMX_TYPES					///< entry count
//};

enum DmxChannelType {
	DMX_GENERIC,
	DMX_INTENSITY_MASTER_DIMMER,
	DMX_INTENSITY_MASTER_DIMMER_FINE,
	DMX_INTENSITY_DIMMER,
	DMX_INTENSITY_DIMMER_FINE,
	DMX_INTENSITY_RED,
	DMX_INTENSITY_RED_FINE,
	DMX_INTENSITY_GREEN,
	DMX_INTENSITY_GREEN_FINE,
	DMX_INTENSITY_BLUE,
	DMX_INTENSITY_BLUE_FINE,
	DMX_INTENSITY_CYAN,
	DMX_INTENSITY_CYAN_FINE,
	DMX_INTENSITY_MAGENTA,
	DMX_INTENSITY_MAGENTA_FINE,
	DMX_INTENSITY_YELLOW,
	DMX_INTENSITY_YELLOW_FINE,
	DMX_INTENSITY_AMBER,
	DMX_INTENSITY_AMBER_FINE,
	DMX_INTENSITY_WHITE,
	DMX_INTENSITY_WHITE_FINE,
	DMX_INTENSITY_UV,
	DMX_INTENSITY_UV_FINE,
	DMX_INTENSITY_INDIGO,
	DMX_INTENSITY_INDIGO_FINE,
	DMX_INTENSITY_LIME,
	DMX_INTENSITY_LIME_FINE,
	DMX_INTENSITY_HUE,
	DMX_INTENSITY_HUE_FINE,
	DMX_INTENSITY_SATURATION,
	DMX_INTENSITY_SATURATION_FINE,
	DMX_INTENSITY_LIGHTNESS,
	DMX_INTENSITY_LIGHTNESS_FINE,
	DMX_INTENSITY_VALUE,
	DMX_INTENSITY_VALUE_FINE,
	DMX_POSITION_PAN,
	DMX_POSITION_PAN_FINE,
	DMX_POSITION_TILT,
	DMX_POSITION_TILT_FINE,
	DMX_POSITION_XAXIS,
	DMX_POSITION_YAXIS,
	DMX_SPEED_PAN_SLOW_FAST,
	DMX_SPEED_PAN_FAST_SLOW,
	DMX_SPEED_TILT_SLOW_FAST,
	DMX_SPEED_TILT_FAST_SLOW,
	DMX_SPEED_PAN_TILT_SLOW_FAST,
	DMX_SPEED_PAN_TILT_FAST_SLOW,
	DMX_COLOR_MACRO,
	DMX_COLOR_WHEEL,
	DMX_COLOR_WHEEL_FINE,
	DMX_COLOR_RGB_MIXER,
	DMX_COLOR_CTO_MIXER,
	DMX_COLOR_CTC_MIXER,
	DMX_COLOR_CTB_MIXER,
	DMX_GOBO_WHEEL,
	DMX_GOBO_WHEEL_FINE,
	DMX_GOBO_INDEX,
	DMX_GOBO_INDEX_FINE,
	DMX_SHUTTER_STROBE_SLOW_FAST,
	DMX_SHUTTER_STROBE_FAST_SLOW,
	DMX_SHUTTER_IRIS_MIN_TO_MAX,
	DMX_SHUTTER_IRIS_MAX_TO_MIN,
	DMX_SHUTTER_IRIS_FINE,
	DMX_BEAM_FOCUS_NEAR_FAR,
	DMX_BEAM_FOCUS_FAR_NEAR,
	DMX_BEAM_FOCUS_FINE,
	DMX_BEAM_ZOOM_SMALLBIG,
	DMX_BEAM_ZOOM_BIGSMALL,
	DMX_BEAM_ZOOM_FINE,
	DMX_PRISM_ROTATION_SLOW_FAST,
	DMX_PRISM_ROTATION_FAST_SLOW,
	DMX_NO_FUNCTION,
	DMX_LASTPRESET, // dummy for cycles
	DMX_TYPES
};


#endif // DMXTYPES_H

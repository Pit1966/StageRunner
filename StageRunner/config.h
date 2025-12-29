/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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
************************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/**
  * For basic defines, settings, application name and so far
  * look in configvars.pri
  */

#define MAX_AUDIO_SLOTS 4
#define MAX_DMX_UNIVERSE 4
#define MAX_STATIC_SCENES 4
#define NET_UDP_PORT 11101
#define NET_TCP_PORT 11102

#define MIDITEMPLATEDIR "/usr/share/stagerunner/miditemplates"
#define USERMIDITEMPLATEDIR ".stagerunner/miditemplates"

#define FX_AUDIO_START_WAIT_DELAY 100			// Time in ms that will be waited for a successful started audio effect

// FORMAT 3 since Jan 2020: introduces new dmxTypes for DmxChannel
// FORMAT 4 since Mai 2024: new FxTimeLine class
#define PROJECT_FORMAT 4						// This is the datafile format for the project file
#define MAX_VOLUME 100
#define MAX_PAN 200								// this is the complete to the right val
#define INITIAL_VOLUME 66
#define LIGHT_LOOP_INTERVAL_MS 20				// Interval time the light loop will be executed with in ms
#define MAX_DMX_FADER_RANGE 10000				// This value is mapped to 255. But the resolution in the scenes needs a better graduation

#endif // CONFIG_H

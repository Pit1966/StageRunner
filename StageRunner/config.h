#ifndef CONFIG_H
#define CONFIG_H

#include "configrev.h"

#define MAX_AUDIO_SLOTS 4
#define MAX_DMX_UNIVERSE 4

//# define APPNAME "StageRunner"
//# define APPVERSION "V0.8.2"
//# define APP_MILESTONE "ArtNet & MAC_SDL"
//# define APP_PRODUCER "2018 Stonechip Entertainment"
//# define APP_ORG_STRING "Stonechip"

#define MIDITEMPLATEDIR "/usr/share/stagerunner/miditemplates"
#define USERMIDITEMPLATEDIR ".stagerunner/miditemplates"

#define FX_AUDIO_START_WAIT_DELAY 100			// Time in ms that will be waited for a successful started audio effect


#define PROJECT_FORMAT 2						// This is the datafile format for the project file
#define MAX_VOLUME 100
#define INITIAL_VOLUME 66
#define LIGHT_LOOP_INTERVAL_MS 20				// Interval time the light loop will be executed with in ms
#define MAX_DMX_FADER_RANGE 10000				// This value is mapped to 255. But the resolution in the scenes needs a better graduation

#endif // CONFIG_H

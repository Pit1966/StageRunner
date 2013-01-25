#ifndef CONFIG_H
#define CONFIG_H


#define APP_NAME "StageRunner"
#define APP_ORG_STRING "Stonechip"
#define LOG_FILE_PATH "/tmp/StageRunner.log"
#define MAX_AUDIO_SLOTS 4

#define FX_AUDIO_START_WAIT_DELAY 100			// Time in ms that will be waited for a successful started audio effect


#ifdef IS_QT5
#define MAX_VOLUME 100
#define INITIAL_VOLUME 60

#else // IS_QT5
#define MAX_VOLUME 160
#define INITIAL_VOLUME 90

#endif // IS_QT5


#endif // CONFIG_H

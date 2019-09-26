#ifndef __FXMASTER_H
#define __FXMASTER_H

#include <QtGlobal>

/**
 * Include file from legacy fxMaster project, used to import fxm project files
 */
namespace FXM {

// how many audio slots wie have
#define SLOTS 6
#define MAX_SAMPLES 100
// size of audiobuffers used for playing soundeffects directly from disc	
#define PLAYAUDIOBUFFERSIZE 4096

// Der Maximale Pegel der Levelanzeige		
#define MAX_PEGEL 32768
// Projektdateiversion und Revision
#define MYVERSION 0
#define REV 6
// size of msg_system ring memory for MAX_MSG open commands
#define	MAX_MSG 40			
// debublevel for verbose messages on console
#define DEBUGLEVEL 0
// min time between two clicks on same key
#define MIN_DOUBLEKEY_TICS 80
//
#define MAX_DMX_CHANNEL 66
#define STARTDMX_VOL 37
#define STARTDMX_CMD 43
#define STARTDMX_VAL 44
#define MAX_DMX_LEVEL 255
#define DEF_DMX_INTERFACE_ADDRESS 0xd000

enum {
	Number_col,Type_col,Datei_col,Key_col,Sort_col,LeftCol_number
};

enum {
	PLAYMODE_NULL,PLAYMODE_NORMAL,PLAYMODE_DISC
};

enum {
	FX_TYPE_NULL,
	FX_TYPE_AUDIO			//Achtung !! nicht mit SAVE_CHUNK_TYPE_FX_AUDIO verwechseln
};

typedef struct {
	quint32 sc_type;			// Save Chunk Type
	quint32 sc_len;				// Länge des Chunks (ohne sc_type und sc_len)
	quint32 fx_key;				// Taste, die dem Effekt zugeordnet ist (NR, die durch Tabelle zugeordnet wird
	char * fx_name;				// Zeiger auf Dateiname incl. vollem Pfad (im Moment)
	quint32	fx_type;			// Type des Effektes
	quint32 fx_type_extra;		// Zusatz Info
	quint32 fx_id;				// ID einmalige ID des Effektes
	quint32 fx_pos;				// Position in Liste auf Screen (-1 = nicht auf Screen)
	quint32 fx_usersort;		// Feld nach dem Sortiert wird
	void *fx_Mix_Chunk;			// was Mix_Chunk ... Pointer auf geladene Sounddatei, falls 0, wurde Sample noch nicht geladen
	quint8 fx_Audio_fap;		// Soll abgespieltes Sample im Speicher bleiben (FREE AFTER PLAY)
	quint8 fx_Audio_preload;	// Ist Sample schon geladen worden, soll Sample beim Project einlesen geladen werden?
	quint8 fx_Audio_Vol;
	quint8 fx_keygroup;			// Tasten Gruppe, es können mehrere Tastengruppen definiert werden, damit Tasten mehrfach verwendet werden können (noch nicht aktiviert)
	quint32 fx_Audio_len;
	qint8 *fx_username;			// vom Benutzer eingegebener Name, der angezeigt wird
	quint8 fx_Audio_pfd;		// soll Sample von Disk abgespielt werden? (Nur bei WAVE möglich)
	quint8 fx_Loop;				// FX soll loopen
	quint8 fx_Offset_Vol;		// Volume for Chunk data (not channel!)
	quint8 fx_preFX;
	quint32 fx_prePara;
	quint8 fx_postFX;
	quint32 fx_postPara;
	quint8 fx_next_Slot;		// slot where audio fx will be played
	quint8 dummy5;
	quint8 dummy6;
	quint8 dummy7;
	quint32 dummy8;
	quint8 relink_pre_flag;
	quint8 relink_post_flag;
}FX;

typedef struct {
	quint32 key;
	qint8 txt[6];
	qint8 used;
}KEY_TXT;

/* Projekt Datei Chunks */


//SC_TYPES nicht FX_TYPES !!!
enum {
	old_SAVE_CHUNK_TYPE_NULL,
	old_SAVE_CHUNK_TYPE_VERSION,
	old_SAVE_CHUNK_TYPE_FX_AUDIO,
	old_SAVE_CHUNK_TYPE_FILENAME,
	old_SAVE_CHUNK_TYPE_USERNAME,
	old_SAVE_CHUNK_TYPE_AUDIO_SETTING,
	old_SAVE_CHUNK_TYPE_DMX_SETTING,
};

enum {
	SAVE_CHUNK_TYPE_NULL = 0x4c4c554e,  // NULL
	SAVE_CHUNK_TYPE_VERSION = 0x53524556, // VERS
	SAVE_CHUNK_TYPE_FX_AUDIO = 0x55415846, // FXAU
	SAVE_CHUNK_TYPE_FILENAME = 0x414e4946, // FINA
	SAVE_CHUNK_TYPE_USERNAME = 0x414e5355, // USNA
	SAVE_CHUNK_TYPE_AUDIO_SETTING = 0x53445541, //AUDS
	SAVE_CHUNK_TYPE_DMX_SETTING = 0x53584d44, //DMXS
};

typedef struct {
	quint32 sc_type;						// Type des Chunks in abgespeicherter Projekt Datei
	quint32 sc_len;							// Länge des Chunks wird beim abspeichern eingetragen
	quint32 sc_version;						//Version
	quint32 sc_rev;							//Revision
}SAVE_CHUNK_VERSION;

typedef struct {
	quint32 sc_type;
	quint32 sc_len;
	quint32 sc_playbuffersize;
}SAVE_CHUNK_AUDIO_SETTING;

typedef struct {
	quint32 sc_type;
	quint32 sc_len;
	quint32 sc_startdmx_vol;
	quint32 sc_startdmx_cmd;
	quint32 sc_startdmx_val;
	quint32 sc_dmx_max_value;
	quint64 sc_dmx_interface_address;
	quint32 sc_startflag;
}SAVE_CHUNK_DMX_SETTING;

typedef struct {
	quint32 sc_type;
	quint32 sc_len;
	quint32 sc_id;
	qint8 sc_name[0];
}SAVE_CHUNK_FILENAME;

typedef struct {
	quint32 sc_type;
	quint32 sc_len;
	quint32 sc_id;					// ID, die zum Namen gehört
	qint8 sc_name[0];
}SAVE_CHUNK_USERNAME;

// Message structure for communication between subroutines (threads)
typedef struct {
	quint32 msg_command;
	quintptr msg_para1;
	int msg_para2;
}MESSAGE;

enum {
	MSG_NULL,
	MSG_START_AUDIO_FX_DISC,
	MSG_INIT_AUDIO_POST_FX
};

enum {
	PREFX_NULL,
	PREFX_FADEOUT_ALL,
	PREFX_STOP_ALL,
	PREFX_FADEOUT,
	PREFX_STOP,
	
	
	PREFX_LAST
};

enum {
	POSTFX_NULL,
	POSTFX_START_FX,
	
	POSTFX_LAST
};

enum {
	PARATYPE_NULL,
	PARATYPE_ID
};

typedef struct {
	quint8 cmd;
	quint8 paratype;
	quint32 para;
	qint8 *name;
}PREFX_CMD;

typedef struct {
	quint8 cmd;
	quint8 paratype;
	quint32 para;
	qint8 *name;
}POSTFX_CMD;


}
#endif

#ifndef FX_DEFINES_H
#define FX_DEFINES_H

/// TimeLine Obj types
enum LINKED_OBJ_TYPE {
	LINKED_UNDEF	= 0,
	LINKED_FX_SCENE	= 10,
	LINKED_FX_AUDIO,
	LINKED_FX_CLIP,
	LINKED_FX_SEQ,
	LINKED_FX_SCRIPT,

	LINKED_FX_LAST = LINKED_FX_SCRIPT	// !! adapt this, if new LINKED_FX... are inserted
	// insert other types behind

	//
};

#endif // FX_DEFINES_H

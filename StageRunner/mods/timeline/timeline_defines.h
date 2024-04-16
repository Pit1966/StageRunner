#ifndef TIMELINE_DEFINES_H
#define TIMELINE_DEFINES_H

#include <QGraphicsItem>

#define TIMELINE_MAX_TRACKS 9



namespace PS_TL {

enum GFX_OBJ_TYPE {
	TL_ITEM = QGraphicsItem::UserType + 1,
	TL_CURSOR
};

}

#endif // TIMELINE_DEFINES_H

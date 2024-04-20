#ifndef EXTTIMELINEITEM_H
#define EXTTIMELINEITEM_H

#include "mods/timeline/timelineitem.h"
#include "fx/fx_defines.h"

class ExtTimeLineItem : public PS_TL::TimeLineItem
{
	Q_OBJECT
private:
	LINKED_OBJ_TYPE m_linkedObjType	= LINKED_UNDEF;
	int m_fxID						= 0;

public:
	ExtTimeLineItem(PS_TL::TimeLineWidget *timeline, int trackId = 1);

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	// reimplementations from base class
	void rightClicked(QGraphicsSceneMouseEvent *event) override;


private:
	void contextDeleteMe();
	void contextEditLabel();
	void contextLinkToFx();

	friend class FxTimeLineEditWidget;
};

#endif // EXTTIMELINEITEM_H

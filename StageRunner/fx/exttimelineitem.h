#ifndef EXTTIMELINEITEM_H
#define EXTTIMELINEITEM_H

#include "mods/timeline/timelineitem.h"
#include "fx/fx_defines.h"
#include "fx/timelineitemdata.h"

class FxItem;


class ExtTimeLineItem : public PS_TL::TimeLineItem, public TimeLineItemData
{
	Q_OBJECT

public:
	ExtTimeLineItem(PS_TL::TimeLineWidget *timeline, int trackId = 1);

	FxItem * linkToFxWithId(int fxId);
	bool linkToFxItem(FxItem *fx);

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	// reimplementations from base class
	void rightClicked(QGraphicsSceneMouseEvent *event) override;
	qreal maxDuration() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
	void contextDeleteMe();
	void contextEditLabel();
	void contextLinkToFx();
	void contextFadeInTime();
	void contextFadeOutTime();

	friend class FxTimeLineEditWidget;
	friend class ExtTimeLineWidget;
};

#endif // EXTTIMELINEITEM_H

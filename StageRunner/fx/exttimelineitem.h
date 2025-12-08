#ifndef EXTTIMELINEITEM_H
#define EXTTIMELINEITEM_H

#include "mods/timeline/timelinebox.h"
#include "fx/fx_defines.h"
#include "fx/timelineitemdata.h"

class FxItem;


class ExtTimeLineItem : public PS_TL::TimeLineBox, public TimeLineItemData
{
	Q_OBJECT

public:
	ExtTimeLineItem(PS_TL::TimeLineWidget *timeline, int trackId = 1);

	FxItem * linkToFxWithId(int fxId);
	bool linkToFxItem(FxItem *fx);

protected:
	// reimplementations from base class
	// void doubleClicked(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	void rightClicked(QGraphicsSceneMouseEvent *event) override;
	int maxDuration() const override;
	int fadeInTime() const override;
	void setFadeInTime(int ms) override;
	int fadeOutTime() const override;
	void setFadeOutTime(int ms) override;


private:
	void contextDeleteMe();
	void contextEditLabel();
	void contextLinkToFx();
	void contextLinkToPause();
	void contextFadeInTime();
	void contextFadeOutTime();

	friend class FxTimeLineEditWidget;
	friend class ExtTimeLineWidget;
};

#endif // EXTTIMELINEITEM_H

#ifndef EXTTIMELINEITEM_H
#define EXTTIMELINEITEM_H

#include "mods/timeline/timelinebox.h"
#include "fx/fx_defines.h"
#include "fx/timelineitemdata.h"

class FxItem;


class ExtTimeLineItem : public PS_TL::TimeLineBox, public TimeLineItemData
{
	Q_OBJECT
private:
	// more configuration for different LINKED_OBJ_TYPEs
	QString m_confStr01;			// only used by CMD_SCRIPT_CMD

public:
	ExtTimeLineItem(PS_TL::TimeLineWidget *timeline, int trackId = 1);

	FxItem * linkToFxWithId(int fxId);
	bool linkToFxItem(FxItem *fx);

	// reimplemented functions from TimeLineItem
	QString getConfigDat() const override;
	bool setConfigDat(const QString &dat) override;

protected:
	// reimplementations from base class
	bool doubleClicked(QGraphicsSceneMouseEvent *event) override;
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
	void contextLinkToScriptCmd();
	void contextFadeInTime();
	void contextFadeOutTime();

	bool openScriptCommandDialog(QString &cmdRef);

	friend class FxTimeLineEditWidget;
	friend class ExtTimeLineWidget;
};

#endif // EXTTIMELINEITEM_H

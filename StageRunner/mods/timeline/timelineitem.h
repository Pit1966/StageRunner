#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include "timelinebase.h"

#include <QObject>
#include <QColor>


namespace PS_TL {

class TimeLineWidget;

class TimeLineItem : public TimeLineBase
{
	Q_OBJECT
public:

enum GRAB {
	GRAB_NONE,
	GRAB_LEFT,
	GRAB_RIGHT,
	GRAB_FADEIN,
	GRAB_FADEOUT
};

public:
	QVariant userData;			// payload data for arbitrary usage

protected:
	int m_id					= 0;
	int m_itemType				= TL_ITEM;
	int m_trackId				= 0;
	bool m_isFirstInit			= false;

	// user data
	qreal m_penWidthBorder		= 1;
	QColor m_colorBG			= QColor(0x225522);
	QColor m_colorBorder		= QColor(0xeeeeee);
	QString m_label;


	// temp
	QPointF m_clickPos;						///< coordinates when item was clicked
	QPointF m_itemPos;						///< position for item when clicked
	qreal m_clickXSize			= 0;		///< x size on item grab event begin
	bool m_isClicked			= false;
	bool m_isHover				= false;
	GRAB m_grabMode				= GRAB_NONE;
	int m_clickFadeInMs			= 0;		///< fadein time when item is clicked
	int m_clickFadeOutMs		= 0;		///< fadeout time when item is clicked


public:
	TimeLineItem(TimeLineWidget *timeline, int trackId = 1);
	int type() const override {return m_itemType;}
	int trackID() const {return m_trackId;}
	bool isFirstInitReady() const {return m_isFirstInit;}

	qreal yPos() const;
	void setYPos(qreal yPixelPos);
	int endPosition() const {return position() + duration();}
	void moveToEndPosition(int ms);
	const QString & label() const {return m_label;}
	void setLabel(const QString &label);
	void setBackgroundColor(const QColor &col);

	// reimplemented functions from TimeLineBase
	void recalcPixelPos() override;

	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	QRectF boundingRect() const override;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

	// propagade some mouse events to derived classes
	virtual void leftClicked(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);}
	virtual void rightClicked(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);}

	// functions maybe implemented in derived classes in order to tweak controls
	virtual int maxDuration() const {return 0;}
	virtual int fadeInTime() const {return 0;}
	virtual void setFadeInTime(int ms) {Q_UNUSED(ms);}
	virtual void setFadeOutTime(int ms) {Q_UNUSED(ms);}
	virtual int fadeOutTime() const {return 0;}

signals:
	void labelChanged(const QString &txt);

};

} // namespace PS_TL

#endif // TIMELINEITEM_H

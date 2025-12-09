#ifndef TIMELINEBOX_H
#define TIMELINEBOX_H

#include "timelineitem.h"

#include <QObject>
#include <QColor>


namespace PS_TL {

class TimeLineWidget;

class TimeLineBox : public TimeLineItem
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

	// user data
	qreal m_penWidthBorder		= 1;

	// temp
	QPointF m_clickPos;						///< coordinates when item was clicked
	QPointF m_itemPos;						///< position for item when clicked
	qreal m_clickXSize			= 0;		///< x size on item grab event begin
	bool m_isClicked			= false;
	bool m_isHover				= false;
	GRAB m_grabMode				= GRAB_NONE;
	int m_clickFadeInMs			= 0;		///< fadein time when item is clicked
	int m_clickFadeOutMs		= 0;		///< fadeout time when item is clicked

	// move to other track
	int m_moveToTrackId = 0;


public:
	TimeLineBox(TimeLineWidget *timeline, int trackId = 1);

	int endPosition() const {return position() + duration();}
	void moveToEndPosition(int ms);

	// reimplemented functions from TimeLineBase
	void recalcPixelPos() override;

	// reimplemented functions from QGraphicsObject that actualy make the TimeLineBox work
	QRectF boundingRect() const override;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
	// void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

	// reimplemented from base class
	bool mouseHoverEvent(qreal x, qreal y) override;

	// propagade some mouse events to derived classes
	virtual void leftClicked(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);}
	virtual void rightClicked(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);}
	virtual void doubleClicked(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);}

};

} // namespace PS_TL

#endif // TIMELINEBOX_H

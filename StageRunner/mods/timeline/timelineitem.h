#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QGraphicsObject>
#include <QObject>
#include <QColor>

namespace PS_TL {

class TimeLineWidget;

class TimeLineItem : public QGraphicsObject
{
	Q_OBJECT
public:
enum {
	TL_ITEM = UserType + 1
};

private:
	TimeLineWidget *m_timeline;
	int m_id					= 0;
	int m_type					= TL_ITEM;
	int m_trackId				= 0;

	// user data
	int m_positionMs			= 0;		///< position of item in milliseconds
	int m_durationMs			= 0;		///< length of item in milliseconds
	qreal m_penWidthBorder		= 1;
	QColor m_colorBG			= QColor(0x225522);
	QColor m_colorBorder		= QColor(0xeeeeee);
	QString m_label;

	// current item dimensions in pixels. Depends on the timeline scaling
	qreal m_xSize				= 200.0;
	qreal m_ySize				= 30.0;
	bool m_isTimePosValid		= false;	///< this is true, if m_timePosMs and m_timeLenMs are valid.
	bool m_isPixelPosValid		= true;		///< this is true, if pixel position information is correct

	// temp
	QPointF m_clickPos;						///< coordinates when item was clicked
	QPointF m_itemPos;						///< position for item when clicked
	bool m_isClicked			= false;


public:
	TimeLineItem(TimeLineWidget *timeline, int trackId = 1);
	int type() const override {return m_type;}
	int trackID() const {return m_trackId;}

	int position() const {return m_positionMs;}
	void setPosition(int ms);
	int duration() const {return m_durationMs;}
	void setDuration(int ms);
	qreal yPos() const;
	void setYPos(qreal yPixelPos);
	int endPosition() const {return m_positionMs + m_durationMs;}
	void setLabel(const QString &label);
	const QString & label() const {return m_label;}
	void setBackgroundColor(const QColor &col);
	void recalcPixelPos();

	// reimplemented functions that actualy make the TimeLineItem work
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

};

} // namespace PS_TL

#endif // TIMELINEITEM_H

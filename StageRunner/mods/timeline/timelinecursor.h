#ifndef TIMELINECURSOR_H
#define TIMELINECURSOR_H

#include "timelinebase.h"

#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QLineF>

namespace PS_TL {

class TimeLineWidget;

class TimeLineCursor : public TimeLineBase
{
	Q_OBJECT
private:
	int m_type					= TL_CURSOR;
	int m_gripSize				= 7;
	int m_gripOffset			= 14;

	QPolygonF m_handle;
	QLineF m_line;
	QColor m_lineCol			= 0xaaaaaa;
	QColor m_handleBorderCol	= 0xaaaaaa;
	QColor m_handleCol			= 0x228080;
	qreal m_height	= 40;


	// temporary
	bool m_clicked	= false;

public:
	TimeLineCursor(TimeLineWidget *timeline, int trackId = 0);
	int type() const override {return m_type;}
	void setHeight(qreal height);
	// QSizeF size() const;
	bool isClicked() const {return m_clicked;}

	void recalcPixelPos() override;

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
	void cursorMoved(int ms);
};

} // namespace PS_TL

#endif // TIMELINECURSOR_H

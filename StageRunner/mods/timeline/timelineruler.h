#ifndef TIMELINERULER_H
#define TIMELINERULER_H

#include "timelineitem.h"

#include <QFont>


namespace PS_TL {

class TimeLineRuler : public TimeLineItem
{
	Q_OBJECT
protected:
	QColor m_colorRulerGrid		= QColor(0x999999);
	QFont m_scaleFont;


	int m_gridDistMajor			= 0;
	int m_gridDistSub			= 0;
	bool m_isGridDistValid		= false;

public:
	TimeLineRuler(TimeLineWidget *timeline, int trackId = 0);

	void setTimeLineDuration(int ms);
	static QString msToTimeLineString(int ms, int msPrec = 2);

protected:
	// reimplemented from base
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void recalcPixelPos() override;
	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


protected:
	bool findTicDistance();

signals:
	void timePositionClicked(int ms);

};

} // namespace PS_TL

#endif // TIMELINERULER_H

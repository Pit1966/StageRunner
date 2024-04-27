#ifndef TIMELINERULER_H
#define TIMELINERULER_H

#include "timelinebase.h"

#include <QFont>


namespace PS_TL {

class TimeLineRuler : public TimeLineBase
{
	Q_OBJECT
protected:
	QColor m_colorBG			= QColor(0x333333);
	QColor m_colorRulerGrid		= QColor(0x999999);
	QFont m_scaleFont;


	int m_gridDistMajor			= 0;
	int m_gridDistSub			= 0;
	bool m_isGridDistValid		= false;

public:
	TimeLineRuler(TimeLineWidget *timeline);

	void setTimeLineDuration(int ms);

	// reimplemented from base
	void recalcPixelPos() override;

	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


	static QString msToTimeLineString(int ms, int msPrec = 2);
protected:
	bool findTicDistance();

};

} // namespace PS_TL

#endif // TIMELINERULER_H

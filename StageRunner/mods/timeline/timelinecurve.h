#ifndef TIMELINECURVE_H
#define TIMELINECURVE_H

#include "timelinebase.h"

namespace PS_TL {

class TimeLineCurve : public TimeLineBase
{
	Q_OBJECT
public:
	TimeLineCurve(TimeLineWidget *timeline);

	void setTimeLineDuration(int ms);

protected:
	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	void recalcPixelPos() override;
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

};

} // namespace PS_TL

#endif // TIMELINECURVE_H

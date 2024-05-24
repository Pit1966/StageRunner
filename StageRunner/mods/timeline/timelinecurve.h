#ifndef TIMELINECURVE_H
#define TIMELINECURVE_H

#include "timelineitem.h"

namespace PS_TL {

class TimeLineCurve : public TimeLineItem
{
	Q_OBJECT
public:
	class Node {
	private:
		TimeLineTrack *m_myTrack;
	public:
		int xMs		= 0;
		int yPM		= 0;			///<  this is a per mille value.

	public:
		Node(TimeLineTrack *track, int x = 0, int yPM = 0)
			: m_myTrack(track)
			, xMs(x)
			, yPM(yPM)
		{}
		int scaledY() const;
	};

private:
	QVector<Node> m_nodes;				// node list. Has at least 2 elements: a start node and an end node

public:
	TimeLineCurve(TimeLineWidget *timeline, int trackId);

	void setTimeLineDuration(int ms);

protected:
	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	void recalcPixelPos() override;
	void setTrackDuration(int ms) override;
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

} // namespace PS_TL

#endif // TIMELINECURVE_H

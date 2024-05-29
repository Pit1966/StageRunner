#ifndef TIMELINECURVE_H
#define TIMELINECURVE_H

#include "timelineitem.h"

namespace PS_TL {

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

class Node {
private:
	TimeLineTrack *m_myTrack;
public:
	int xMs			= 0;
	int yPM			= 0;	///<  this is a per mille value.
	mutable qreal x	= 0;	///< real position after first draw
	mutable qreal y	= 0;	///< real position after first draw

public:
	Node(TimeLineTrack *track, int x = 0, int yPM = 0)
		: m_myTrack(track)
		, xMs(x)
		, yPM(yPM)
	{}
	QPointF nodePixel(qreal msPerPix) const;
	int scaledY() const;
	int unScaleY(qreal y) const;
};

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

class TimeLineCurveData
{
protected:
	QVector<Node> m_nodes;				// node list. Has at least 2 elements: a start node and an end node


public:
	void insertNode(const Node &n);
	qreal valAtMs(int ms);

	QString getCurveData() const;
	bool setCurveData(const QString &dat, TimeLineTrack *track);
};

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------


class TimeLineCurve : public TimeLineItem, public TimeLineCurveData
{
	Q_OBJECT
public:

private:
	uint m_colorNode = 0xaa9900;
	uint m_colorNodeHov = 0x00ff00;

	// temp
	int m_curHoveredNode	= -1;
	bool m_nodeClicked		= false;
	QPointF m_clickNodePos;						///< coordinates when node was clicked

public:
	TimeLineCurve(TimeLineWidget *timeline, int trackId);

	void setTimeLineDuration(int ms);

	// reimplemented functions from TimeLineItem
	QString getConfigDat() const override;
	bool setConfigDat(const QString &dat) override;

protected:
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	// reimplemented functions from QGraphicsObject that actualy make the TimeLineItem work
	void recalcPixelPos() override;
	void setTrackDuration(int ms) override;

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

} // namespace PS_TL

#endif // TIMELINECURVE_H

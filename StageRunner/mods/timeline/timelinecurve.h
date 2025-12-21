#ifndef TIMELINECURVE_H
#define TIMELINECURVE_H

#include "timelineitem.h"


namespace PS_TL {

class TimeLineTrack;

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
    Node(PS_TL::TimeLineTrack *track = nullptr, int x = 0, int yPM = 0)
		: m_myTrack(track)
		, xMs(x)
		, yPM(yPM)
	{}
    Node(const Node &o) = default;
    Node & operator=(const Node &o) = default;
    Node & operator=(Node &o) = default;

    // Node(const Node &o)
    // 	: m_myTrack(o.m_myTrack)
    // 	, xMs(o.xMs)
    // 	, yPM(o.yPM)
    // {}
    // Node & operator=(const Node &o) {
    // 	m_myTrack = o.m_myTrack;
    // 	xMs = o.xMs;
    // 	yPM = o.yPM;
    // 	return *this;
    // }
	QPointF nodePixel(qreal msPerPix) const;
	int scaledY() const;
	int unScaleY(qreal y) const;
};

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

class TimeLineCurveData
{
protected:
	int m_curveType		= 0;
	QVector<Node> m_nodes;				// node list. Has at least 2 elements: a start node and an end node

public:
	void insertNode(const Node &n);
	qreal valAtMs(int ms);

	void setCurveType(int type) {m_curveType = type;}
	int curveType() const {return m_curveType;}

	QString getCurveData() const;
	bool setCurveData(const QString &dat, TimeLineTrack *track);
};

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------


class TimeLineCurve : public TimeLineItem, public TimeLineCurveData
{
	Q_OBJECT
public:
	static TimeLineCurve * staticTmpContextObj;

private:
	uint m_colorNode	= 0xaa9900;
	uint m_colorNodeHov = 0x00ff00;

	uint m_altNodeColor = 0x449955;

	// temp
	int m_curHoveredNode	= -1;
	bool m_nodeClicked		= false;
	QPointF m_clickNodePos;						///< coordinates when node was clicked
	QPointF m_lastContextMenuClickPos;			///< click position when context menu was called last time (also when event was ignored)

public:
	TimeLineCurve(TimeLineWidget *timeline, int trackId);
	TimeLineWidget *myTimeLine() const {return m_timeline;}

	void setTimeLineDuration(int ms);
	bool hasOverLayContextMenu() const override;
	QPointF lastContextMenuClickPos() const {return m_lastContextMenuClickPos;}
	QList<TimeLineContextMenuEntry> getOverlayContextMenu(const QPointF &scenePos) override;
	// bool execContextMenuCmd(const TimeLineContextMenuEntry *menuEntry);

	// reimplemented functions from TimeLineItem
	QString getConfigDat() const override;
	bool setConfigDat(const QString &dat) override;

	// node functions
	void clearNodes();
	void appendNode(const Node &node);
	void setNodeColor(uint colorval) {m_colorNode = colorval;}
	void setAltNodeColor();

	void addNodeAtXpos(qreal xpix);

protected:
	// void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	// reimplemented functions from TimeLineItem (or QGraphicsObject) that actualy make the TimeLineItem work
	void recalcPixelPos() override;
	bool mouseHoverEvent(qreal x, qreal y) override;
	void setTrackDuration(int ms) override;

	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

} // namespace PS_TL

#endif // TIMELINECURVE_H

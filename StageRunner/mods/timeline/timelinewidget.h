#ifndef PS_TL_TIMELINEWIDGET_H
#define PS_TL_TIMELINEWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QLabel>
#include <QSlider>
#include <QPointer>

#include "timeline_defines.h"

namespace PS_TL {

class TimeLineItem;
class TimeLineWidget;
class TimeLineCursor;
class TimeLineRuler;


class TimeLineGfxScene : public QGraphicsScene
{
	Q_OBJECT
private:
	TimeLineWidget * m_timeLine;
	QColor m_bg1	= QColor(0x2a2a2a);
	QColor m_bg2	= QColor(0x232424);

public:
	TimeLineGfxScene(TimeLineWidget *timeLineWidget, QWidget *parent = nullptr);

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void drawBackground(QPainter *painter, const QRectF &rect);

signals:
	void mouseHoverPosChanged(const QPointF &pos);


};

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineGfxView : public QGraphicsView
{
	Q_OBJECT
private:
	TimeLineWidget * m_myTimeline;
	QSize m_currentSize;				///< this is current widget size after a resize event. QWidget::size() reports a different size as the resizeEvent !!

public:
	TimeLineGfxView(TimeLineWidget *timeLineWidget, QGraphicsScene *scene, QWidget *parent = nullptr);

	const QSize & currentSize() const;

protected:
	void resizeEvent(QResizeEvent *event);
};

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineTrack
{
public:
	int m_trackID;
	int m_yPos;
	int m_ySize;

public:
	TimeLineTrack(int id, int y = 0, int size = 24)
		: m_trackID(id)
		, m_yPos(y)
		, m_ySize(size)
	{}
	inline int trackId() const {return m_trackID;}
	inline int yPos() const {return m_yPos;}
	inline int yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineWidget : public QWidget
{
	Q_OBJECT

private:
	TimeLineGfxScene *m_scene;
	TimeLineGfxView *m_view;

	TimeLineCursor *m_cursor;
	TimeLineRuler *m_ruler;


protected:
	QLabel *m_navLabel;
	QSlider *m_viewRangeSlider;

	int m_viewPosMs				= 0;		///< current timeline view position (left border of view)
	int m_viewRangeMs			= 60000;	///< length of visible timeline portion (view area length)
	int m_timeLineLenMs			= 60000;	///< virtual length of timeline. This is timeline duration

	int m_defaultTrackHeight	= 34;
	QList<TimeLineTrack> m_tracks;			///< list of y-sizes of the tracks


	QPointer<TimeLineItem> m_rightMostItem;	///< this is the furthest right item in the timeline

	// lists with timeline items. Each track has its own list
	// List index 0 is the ruler track. The editable tracks start with index 1
	QList<TimeLineItem*> m_itemLists[TIMELINE_MAX_TRACKS];

	// helper for transformation of time to pixel pos and vice versa
	qreal m_msPerPixel			= 0.0;		///< how many milliseconds is a pixel
	qreal m_rightMargin			= 1.0;

	// temp
	int m_curMouseXPosMs		= 0;

public:
	explicit TimeLineWidget(QWidget *parent = nullptr);
	virtual ~TimeLineWidget();
	void clear();

	int timeLineHeight() const;
	int timeLineDuration() const {return m_timeLineLenMs;}
	TimeLineItem * addTimeLineItem(int posMs, int durationMs, const QString &label, int trackID = 1);
	TimeLineItem * at(int trackID, int idx);
	bool removeTimeLineItem(TimeLineItem *item, bool deleteLater = false);
	int timeLineSize(int trackID = 1) const;
	QGraphicsView *gfxView() const {return m_view;}
	TimeLineGfxView *timeLineGfxView() const {return m_view;}

	qreal msPerPixel() const;
	qreal pixelToMs(qreal x) const;
	qreal msToPixel(qreal ms) const;
	int leftViewPixel() const;
	int rightViewPixel() const;
	QRectF currentVisibleRect() const;

	bool checkForRightMostItem(TimeLineItem *item);
	TimeLineItem * findRightMostItem() const;

	int cursorPos() const;
	int yPosToTrackId(int y);

public slots:
	void setTimeLineDuration(int ms);
	void setTimeLineViewRangeMs(int ms);
	void setCursorPos(int ms);

protected:
	void resizeEvent(QResizeEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

	virtual TimeLineItem *createNewTimeLineItem(TimeLineWidget *timeline, int trackId = 1);

	void adjustSceneRectToTimelineLength();
	void recalcPixelPosInAllItems();

	void contextMenuEvent(QContextMenuEvent *event);


private slots:
	void onCursorMoved(int ms);
	void onMouseHovered(const QPointF &pos);

signals:
	void furthestRightItemChanged(TimeLineItem *item);
	void cursorPosChanged(int ms);
	void cursorActivated(int ms);
	void mousePosMsChanged(int ms);
	void timeLineDurationChanged(int ms);

	friend class TimeLineGfxScene;
};

} // namespace PS_TL

#endif // PS_TL_TIMELINEWIDGET_H
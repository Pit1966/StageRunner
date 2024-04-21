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



class TimeLineWidget : public QWidget
{
	Q_OBJECT
private:
	QGraphicsScene *m_scene;
	TimeLineGfxView *m_view;

	TimeLineCursor *m_cursor;

protected:
	QLabel *m_navLabel;
	QSlider *m_viewRangeSlider;

	int m_viewPosMs				= 0;		///< current timeline view position (left border of view)
	int m_viewRangeMs			= 60000;	///< length of visible timeline portion (view area length)
	int m_timeLineLenMs			= 60000;	///< virtual length of timeline. This is timeline duration

	int m_defaultTrackHeight	= 34;
	QList<int> m_trackYOffsets;			///< list of y-sizes of the tracks

	QPointer<TimeLineItem> m_rightMostItem;	///< this is the furthest right item in the timeline

	// lists with timeline items. Each track has its own list
	// List index 0 is the ruler track. The editable tracks start with index 1
	QList<TimeLineItem*> m_itemLists[TIMELINE_MAX_TRACKS];

	// helper for transformation of time to pixel pos and vice versa
	qreal m_msPerPixel			= 0.0;		///< how many milliseconds is a pixel
	qreal m_rightMargin			= 1.0;

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

public slots:
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

signals:
	void furthestRightItemChanged(TimeLineItem *item);

};

} // namespace PS_TL

#endif // PS_TL_TIMELINEWIDGET_H

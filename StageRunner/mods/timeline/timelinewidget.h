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
#include "timelinetrack.h"

namespace PS_TL {

class TimeLineBox;
class TimeLineWidget;
class TimeLineCursor;
class TimeLineRuler;


class TimeLineGfxScene : public QGraphicsScene
{
	Q_OBJECT
protected:
	TimeLineWidget * m_timeLine;
	QColor m_bg1	= QColor(0x2a2a2a);
	QColor m_bg2	= QColor(0x232424);

public:
	TimeLineGfxScene(TimeLineWidget *timeLineWidget, QWidget *parent = nullptr);

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void drawBackground(QPainter *painter, const QRectF &rect) override;

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
	void resizeEvent(QResizeEvent *event) override;
};

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineWidget : public QWidget
{
	Q_OBJECT

protected:
	TimeLineGfxScene *m_scene	= nullptr;
	TimeLineGfxView *m_view		= nullptr;

	TimeLineCursor *m_cursor	= nullptr;
	TimeLineRuler *m_ruler		= nullptr;

	QLabel *m_navLabel			= nullptr;
	QSlider *m_viewRangeSlider	= nullptr;

	int m_viewPosMs				= 0;		///< current timeline view position (left border of view)
	int m_viewRangeMs			= 60000;	///< length of visible timeline portion (view area length)
	int m_timeLineLenMs			= 60000;	///< virtual length of timeline. This is timeline duration

	int m_defaultTrackHeight	= 40;

	QPointer<TimeLineBox> m_rightMostItem;	///< this is the furthest right item in the timeline

	// lists with timeline items. Each track has its own list
	// TimeLineTrack with index 0 is the ruler track. The editable tracks start with index 1
	QList<TimeLineTrack*> m_tracks;			///< list of the tracks, containing ySizes of the tracks and item lists

	// helper for transformation of time to pixel pos and vice versa
	qreal m_msPerPixel			= 0.0;		///< how many milliseconds is a pixel
	qreal m_rightMargin			= 1.0;

	// temp
	int m_curMouseXPosMs		= 0;
	QPointer<TimeLineItem> m_hoveredItem[TIMELINE_MAX_TRACKS];

public:
	explicit TimeLineWidget(QWidget *parent = nullptr);
	virtual ~TimeLineWidget();
	void init();
	bool isInitialized() const {return m_scene != nullptr;}
	void clear();

	int defaultTrackHeight() const {return m_defaultTrackHeight;}
	int timeLineHeight() const;
	int timeLineDuration() const {return m_timeLineLenMs;}
	TimeLineTrack *findTrackWithId(int trackId) const;
	TimeLineTrack *findTrackAboveId(int trackId) const;
	TimeLineTrack *findTrackBelowId(int trackId) const;

	bool addTimeLineTrack();
	bool addTimeLineTrack(TimeLineTrack *track);
	bool addAudioEnvelopeTrack();
	bool deleteTimeLineTrack(int trackID);
	TimeLineBox * addTimeLineBox(int posMs, int durationMs, const QString &label, int trackID = 1);
	TimeLineItem * at(int trackID, int idx);
	bool removeTimeLineItem(TimeLineItem *item, bool deleteLater = false);
	int timeLineSize(int trackID = 1) const;
	QGraphicsView *gfxView() const {return m_view;}
	TimeLineGfxView *timeLineGfxView() const {return m_view;}

	bool setTrackHeight(int trackID, int h);
	int trackHeight(int trackID);

	void setTrackOverlay(int trackID, bool enable);
	bool isTrackOverlay(int trackID) const;

	qreal msPerPixel() const;
	qreal pixelToMs(qreal x) const;
	qreal msToPixel(qreal ms) const;
	int leftViewPixel() const;
	int rightViewPixel() const;
	QRectF currentVisibleRect() const;

	bool checkForRightMostItem(TimeLineBox *item);
	TimeLineBox * findRightMostItem() const;

	int cursorPos() const;

	int yPosToTrackId(int y);
	TimeLineTrack *yPosToTrack(int y);
	QList<TimeLineTrack*> yPosToTrackList(int y);
	int trackIdBefore(int trackId);
	int findOverlayIdForTrackId(int trackId);

	void checkMousePos(qreal x, qreal y);

public slots:
	void setTimeLineDuration(int ms);
	void setTimeLineViewRangeMs(int ms);
	void setCursorPos(int ms);
	void updateScene();

protected:
	void resizeEvent(QResizeEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void showEvent(QShowEvent *event);

	virtual TimeLineBox *createNewTimeLineBox(TimeLineWidget *timeline, int trackId = 1);
	virtual TimeLineGfxScene *createTimeLineScene(TimeLineWidget *timeline);

	void appendTrack(TimeLineTrack *track);
	void recalcTrackSizes(int from = 0);
	void relinkTracks();
	void adjustSceneRectToTimelineLength();
	void recalcPixelPosInAllItems();

	void contextMenuEvent(QContextMenuEvent *event);

private slots:
	void onCursorMoved(int ms);
	void onMouseHovered(const QPointF &pos);

signals:
	void furthestRightItemChanged(TimeLineBox *item);
	void cursorPosChanged(int ms);
	void cursorActivated(int ms);
	void mousePosMsChanged(int ms);
	void timeLineDurationChanged(int ms);

	friend class TimeLineGfxScene;
};

} // namespace PS_TL

#endif // PS_TL_TIMELINEWIDGET_H

#ifndef PS_TL_TIMELINEWIDGET_H
#define PS_TL_TIMELINEWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QLabel>

namespace PS_TL {

class TimeLineItem;
class TimeLineWidget;

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
	QLabel *m_navLabel;

	int m_viewPosMs				= 0;		///< current timeline view position (left border of view)
	int m_viewRangeMs			= 60000;	///< length of visible timeline portion (view area length)
	int m_timeLineLenMs			= 60000;	///< virtual length of timeline. This is timeline duration

	int m_defaultTrackHeight	= 34;
	QList<int> m_trackYOffsets;			///< list of y-sizes of the tracks

	TimeLineItem *m_rightMostItem	= nullptr;	///< this is the rightmost item in the timeline

	// helper for transformation of time to pixel pos and vice versa
	qreal m_msPerPixel			= 0.0;		///< how many milliseconds is a pixel

public:
	explicit TimeLineWidget(QWidget *parent = nullptr);
	TimeLineItem * addTimeLineItem(int posMs, int durationMs, const QString &label, int trackID = 1);


	qreal msPerPixel() const;
	bool checkForRightMostItem(TimeLineItem *item);
	TimeLineItem * findRightMostItem() const;

public slots:
	void setTimeLineViewRangeMs(int ms);

protected:
	void resizeEvent(QResizeEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

	void adjustSceneRectToTimelineLength();
	void recalcPixelPosInAllItems();

signals:
	void furthestRightItemChanged(TimeLineItem *item);

};

} // namespace PS_TL

#endif // PS_TL_TIMELINEWIDGET_H

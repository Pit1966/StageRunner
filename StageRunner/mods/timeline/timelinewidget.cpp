#include "timelinewidget.h"

#include "timelineitem.h"

#include <QBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSlider>

namespace PS_TL {

TimeLineGfxView::TimeLineGfxView(TimeLineWidget *timeLineWidget, QGraphicsScene *scene, QWidget *parent)
	: QGraphicsView(scene, parent)
	, m_myTimeline(timeLineWidget)
{
}

const QSize &TimeLineGfxView::currentSize() const
{
	return m_currentSize;
}

void TimeLineGfxView::resizeEvent(QResizeEvent *event)
{
	m_currentSize = event->size();
	qDebug() << "resize view: event size" << event->size() << "widget size" << this->size();

	// qreal xsize = event->size().width();
	// qreal ysize = event->size().height();

	// setSceneRect(0, 0, xsize, ysize);
}

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------


TimeLineWidget::TimeLineWidget(QWidget *parent)
	: QWidget{parent}
{
	m_scene = new QGraphicsScene(this);
	m_view = new TimeLineGfxView(this, m_scene);
	m_navLabel = new QLabel("navigation ...");

	m_trackYOffsets.append(1);

	// QGraphicsRectItem *gitem = new QGraphicsRectItem(50,50, 50, 10);
	// m_scene->addItem(gitem);

	// TimeLineItem *tlItem1 = new TimeLineItem(this);
	// tlItem1->setLabel("item 1");
	// m_scene->addItem(tlItem1);

	// TimeLineItem *tlItem2 = new TimeLineItem(this);
	// tlItem2->setLabel("item 2");
	// tlItem2->setPos(40,40);
	// tlItem2->setBackgroundColor(0x552222);
	// m_scene->addItem(tlItem2);

	QSlider *slider = new QSlider(Qt::Horizontal);
	slider->setMaximum(120000);		// 2minutes
	slider->setValue(60000);
	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(setTimeLineViewRangeMs(int)));

	// widget layout
	QBoxLayout *vLay = new QVBoxLayout;
	vLay->setSpacing(0);
	vLay->setContentsMargins(0,0,0,0);
	vLay->addWidget(m_navLabel);
	vLay->addWidget(m_view);
	vLay->addWidget(slider);
	setLayout(vLay);
}

/**
 * @brief TimeLineWidget::addTimeLineItem
 * @param trackID
 * @param posMs
 * @param durationMs
 * @param label
 * @return
 */
TimeLineItem *TimeLineWidget::addTimeLineItem(int posMs, int durationMs, const QString &label, int trackID)
{
	while (m_trackYOffsets.size() <= trackID) {
		m_trackYOffsets.append(m_trackYOffsets.last() + m_defaultTrackHeight);
	}

	TimeLineItem *item = new TimeLineItem(this, trackID);
	item->setLabel(label);
	item->setDuration(durationMs);
	item->setPosition(posMs);
	item->setYPos(m_trackYOffsets.at(trackID));

	m_scene->addItem(item);
	return item;
}

qreal TimeLineWidget::msPerPixel() const
{
	return m_msPerPixel;
}

/**
 * @brief Check if TimeLineItem position is furthest to the right
 * @param item
 * @return true, if the item is furthest to the right. In this case the item address is stored.
 *
 * This function further checks wether the last item position is beyond the current time line length.
 * The time line will be extended if so.
 */
bool TimeLineWidget::checkForRightMostItem(TimeLineItem *item)
{
	TimeLineItem *newRightItem = m_rightMostItem;

	if (!newRightItem) {	// no item existed before
		newRightItem = item;
	}
	else if (item == newRightItem) {  // item is already the furthest right item, but it could has been moved to the left
		newRightItem = findRightMostItem();
	}
	else if (item->endPosition() > m_rightMostItem->endPosition()) {
		newRightItem = item;
	}

	// check if item is beyond the time line lenght
	if (newRightItem->endPosition() > m_timeLineLenMs) {
		// extend timeline
		m_timeLineLenMs = newRightItem->endPosition();
		adjustSceneRectToTimelineLength();
	}

	if (m_rightMostItem != newRightItem) {
		m_rightMostItem = newRightItem;
		emit furthestRightItemChanged(m_rightMostItem);
		qDebug() << "furthest right item changed" << m_rightMostItem->label() << "endPos" << m_rightMostItem->endPosition() << "timeline len [ms]" << m_timeLineLenMs;
		return true;
	}

	return false;
}

TimeLineItem *TimeLineWidget::findRightMostItem() const
{
	TimeLineItem *rItem = nullptr;
	const QList<QGraphicsItem*> items = m_scene->items();
	for (auto *it : items) {
		TimeLineItem *tlItem = qgraphicsitem_cast<TimeLineItem *>(it);
		if (tlItem) {
			if (rItem) {
				if (tlItem->endPosition() > rItem->endPosition())
					rItem = tlItem;
			}
			else {
				rItem = tlItem;
			}

		}
	}
	return rItem;
}

void TimeLineWidget::setTimeLineViewRangeMs(int ms)
{
	if (m_viewRangeMs == ms)
		return;

	qreal xsize = m_view->width() - 4;

	// adjust view range and transformation factor
	m_viewRangeMs = ms;
	m_msPerPixel = qreal(m_viewRangeMs) / xsize;

	// update pixel positions in all items
	recalcPixelPosInAllItems();

}

/**
 * @brief TimeLineWidget::resizeEvent
 * @param event
 *
 * @note TimeLineWidget::resizeEvent(...) is called after TimeLineGfxView::resizeEvent(...), which is part of this class !!
 */
void TimeLineWidget::resizeEvent(QResizeEvent *event)
{
	qDebug() << "  -> resize widget" << event->size() << "view size" << m_view->size() << "stored size" << m_view->currentSize();

	// get size of child TimeLineGfxView
	// qreal xsize = m_view->currentSize().width();
	// qreal ysize = m_view->currentSize().height();

	qreal xsize = m_view->width() - 4;
	// qreal ysize = m_view->height() - 4;

	qDebug() << "  -> vert scroll" << m_view->verticalScrollBar()->size() << "hori scroll" << m_view->horizontalScrollBar()->size();

	// calculate pixel/time convertion factors
	// if this is the first call, the scale is set. Otherwise the viewRange is adopted.
	if (m_msPerPixel == 0) {
		m_msPerPixel = qreal(m_viewRangeMs) / xsize;
	} else {
		m_viewRangeMs = m_msPerPixel * xsize;
	}

	// setViewRect of screen to the x-size of the complete timeline
	// if timeline is smaller than visible view, the view rect is extended to the view size
	adjustSceneRectToTimelineLength();

	// qDebug() << "ms per pixel" << m_msPerPixel << m_scene->sceneRect();
}

void TimeLineWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	qDebug() << "double click";
	setTimeLineViewRangeMs(30000);
}

/**
 * @brief TimeLineWidget::adjustSceneRectToTimelineLength
 *
 * setViewRect of screen to the x-size of the complete timeline
 * if timeline is smaller than visible view, the view rect is extended to the view size
 */
void TimeLineWidget::adjustSceneRectToTimelineLength()
{
	qreal xsize = m_view->width() - 4;
	qreal ysize = m_view->height() - 4;

	qreal timelineXSize = qreal(m_timeLineLenMs) / m_msPerPixel;
	if (timelineXSize < xsize)
		timelineXSize = xsize;
	m_view->setSceneRect(0, 0, timelineXSize, ysize);
}

void TimeLineWidget::recalcPixelPosInAllItems()
{
	const QList<QGraphicsItem*> items = m_scene->items();
	for (auto *it : items) {
		TimeLineItem *tlItem = qgraphicsitem_cast<TimeLineItem *>(it);
		if (tlItem) {
			tlItem->recalcPixelPos();
			// tlItem->update();
		}
	}
	m_scene->update();
}

} // namespace PS_TL

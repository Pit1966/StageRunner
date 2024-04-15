#include "timelinewidget.h"

#include "timelineitem.h"

#include <QBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>
#include <QAction>
#include <QMenu>

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

	m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	m_viewRangeSlider = new QSlider(Qt::Horizontal);
	m_viewRangeSlider->setMaximum(120000);		// 2minutes
	m_viewRangeSlider->setValue(m_viewRangeMs);
	connect(m_viewRangeSlider, SIGNAL(sliderMoved(int)), this, SLOT(setTimeLineViewRangeMs(int)));

	// widget layout
	QBoxLayout *vLay = new QVBoxLayout;
	vLay->setSpacing(0);
	vLay->setContentsMargins(0,0,0,0);
	vLay->addWidget(m_navLabel);
	vLay->addWidget(m_view);
	vLay->addWidget(m_viewRangeSlider);
	setLayout(vLay);
}

TimeLineWidget::~TimeLineWidget()
{
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		while (!m_itemLists[t].isEmpty())
			delete m_itemLists[t].takeFirst();
	}
}

void TimeLineWidget::clear()
{
	m_rightMostItem = nullptr;
	for (int t=1; t<TIMELINE_MAX_TRACKS; t++) {
		while (!m_itemLists[t].isEmpty())
			delete m_itemLists[t].takeFirst();
	}
}

/**
 * @brief Returns the vertical space needed in order to display all tracks in full height
 *
 * The functions returns at least the height of 2 tracks
 */
int TimeLineWidget::timeLineHeight() const
{
	return qMax(m_trackYOffsets.last(), m_defaultTrackHeight * 2 + 10);
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

	m_itemLists[trackID].append(item);

	// item->setCursor(Qt::SizeHorCursor);
	// QGraphicsRectItem *leftHandle = new QGraphicsRectItem(0,0, 10, item->ho, item);

	m_scene->addItem(item);
	return item;
}

TimeLineItem *TimeLineWidget::at(int trackID, int idx)
{
	if (trackID < 0 || trackID >= TIMELINE_MAX_TRACKS)
		return nullptr;

	if (m_itemLists[trackID].size() <= idx)
		return nullptr;

	return m_itemLists[trackID].at(idx);
}

/**
 * @brief Return the amount of elements in the timeline for the requested track id
 * @param trackID
 * @return
 */
int TimeLineWidget::timeLineSize(int trackID) const
{
	if (trackID < 0 || trackID >= TIMELINE_MAX_TRACKS)
		return 0;

	return m_itemLists[trackID].size();
}

qreal TimeLineWidget::msPerPixel() const
{
	return m_msPerPixel;
}

qreal TimeLineWidget::pixelToMs(qreal x) const
{
	return m_msPerPixel * x;
}

qreal TimeLineWidget::msToPixel(qreal ms) const
{
	return ms / m_msPerPixel;
}

QRectF TimeLineWidget::currentVisibleRect() const
{
	// QRectF rect = m_view->mapToScene(m_view->viewport()->geometry()).boundingRect();

	QPointF tl(m_view->horizontalScrollBar()->value(), m_view->verticalScrollBar()->value());
	QPointF br = tl + m_view->viewport()->rect().bottomRight();
	QRectF rect = QRectF(tl,br);

	// qDebug() << "begin [x]" << rect.x() << "end [x]" << rect.right();
	qDebug() << "begin [ms]" << pixelToMs(rect.x()) << "end [ms]" << pixelToMs(rect.right() + 1);

	return rect;
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
	m_msPerPixel = qreal(m_viewRangeMs) / (xsize - m_rightMargin);

	// readjust sceneRect since pixel transformation has changed
	adjustSceneRectToTimelineLength();
	// update pixel positions in all items
	recalcPixelPosInAllItems();

	m_navLabel->setText(QString("view range: %1s").arg(float(ms)/1000));
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

	qreal xsize = m_view->width() - 4;
	qreal xvsize = m_view->viewport()->geometry().width();
	// qreal ysize = m_view->height() - 4;

	// calculate pixel/time convertion factors
	// if this is the first call, the scale is set. Otherwise the viewRange is adopted.
	if (m_msPerPixel == 0) {
		m_msPerPixel = qreal(m_viewRangeMs) / (xsize - m_rightMargin);
	} else {
		m_viewRangeMs = m_msPerPixel * xsize;
		m_viewRangeSlider->setValue(m_viewRangeMs);
	}

	// setViewRect of screen to the x-size of the complete timeline
	// if timeline is smaller than visible view, the view rect is extended to the view size
	adjustSceneRectToTimelineLength();

	m_navLabel->setText(QString("view range: %1s").arg(float(m_viewRangeMs)/1000));

	// qDebug() << "ms per pixel" << m_msPerPixel << m_scene->sceneRect();
}

void TimeLineWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	// setTimeLineViewRangeMs(30000);
	// clear();
	currentVisibleRect();
}

/**
 * @brief TimeLineWidget::adjustSceneRectToTimelineLength
 *
 * setViewRect of scene to the x-size of the complete timeline
 */
void TimeLineWidget::adjustSceneRectToTimelineLength()
{
	// qreal xsize = m_view->viewport()->geometry().width();
	// qreal xsize = m_view->width() - 4;
	// qreal ysize = m_view->height() - 4;

	qreal yMax = timeLineHeight();

	qreal timelineXSize = qreal(m_timeLineLenMs) / m_msPerPixel;
	m_view->setSceneRect(0, 0, timelineXSize, yMax);
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

void TimeLineWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	QAction *act;
	act = menu.addAction(tr("Add Item in track 1"));
	act->setObjectName("newIn1");
	act = menu.addAction(tr("Add Item in track 2"));
	act->setObjectName("newIn2");

	act = menu.exec(event->globalPos());
	if (!act) return;

	QString cmd = act->objectName();
	if (cmd == "newIn1") {
		int ms = 0;
		if (!m_itemLists[1].isEmpty())
			ms = m_itemLists[1].last()->endPosition();
		/*TimeLineItem *item = */addTimeLineItem(ms, 10000, "item t1", 1);
	}
	else if (cmd == "newIn2") {
		int ms = 50000;
		if (!m_itemLists[2].isEmpty())
			ms = m_itemLists[2].last()->endPosition();
		/*TimeLineItem *item = */addTimeLineItem(ms, 10000, "item t2", 2);
	}
}

} // namespace PS_TL

#include "timelinewidget.h"

#include "timelineitem.h"
#include "timelinecursor.h"
#include "timelineruler.h"
#include "timelinecurve.h"


#include <QBoxLayout>
#include <QDebug>
#include <QResizeEvent>
#include <QScrollBar>
#include <QAction>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

namespace PS_TL {


TimeLineGfxScene::TimeLineGfxScene(TimeLineWidget *timeLineWidget, QWidget *parent)
	: QGraphicsScene(parent)
	, m_timeLine(timeLineWidget)
{
}

void TimeLineGfxScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	emit mouseHoverPosChanged(event->scenePos());
	QGraphicsScene::mouseMoveEvent(event);
}

void TimeLineGfxScene::drawBackground(QPainter *painter, const QRectF &/*rect*/)
{
	int timeLineWidthMs = m_timeLine->timeLineDuration();
	int width = m_timeLine->msToPixel(timeLineWidthMs);

	QList<TimeLineTrack*> &tracks = m_timeLine->m_tracks;
	for (int t=1; t<tracks.size(); t++) {
		TimeLineTrack *track = tracks.at(t);
		QColor bgcol = track->trackBgColor;
		if (track->trackBgColor >= 0) {
			painter->fillRect(0, track->yPos()-1, width, track->ySize()-1, bgcol);
		}
		else if (t & 1) {
			painter->fillRect(0, track->yPos()-1, width, track->ySize()-1, m_bg1);
		}
		else {
			painter->fillRect(0, track->yPos()-1, width, track->ySize()-1, m_bg2);
		}
	}
}

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

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
	// qDebug() << "resize view: event size" << event->size() << "widget size" << this->size();
}

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------


TimeLineWidget::TimeLineWidget(QWidget *parent)
	: QWidget{parent}
{
}

TimeLineWidget::~TimeLineWidget()
{
	while (!m_tracks.isEmpty())
		delete m_tracks.takeFirst();
}

void TimeLineWidget::init()
{
	m_scene = createTimeLineScene(this);
	m_view = new TimeLineGfxView(this, m_scene);
	m_navLabel = new QLabel("navigation ...");

	connect(m_scene, SIGNAL(mouseHoverPosChanged(QPointF)), this, SLOT(onMouseHovered(QPointF)));

	// create ruler track. This is always track with ID 0
	m_tracks.append(new TimeLineTrack(TRACK_RULER, 0, 0, 34));

	// add cursor to scene
	m_cursor = new TimeLineCursor(this);
	m_scene->addItem(m_cursor);
	m_cursor->setPos(30, 0);
	m_cursor->setZValue(100);
	connect(m_cursor, SIGNAL(cursorMoved(int)), this, SLOT(onCursorMoved(int)));


	// add ruler to scene
	m_ruler = new TimeLineRuler(this);
	m_scene->addItem(m_ruler);
	m_ruler->setPos(0, 0);
	m_ruler->setZValue(99);
	m_ruler->setDuration(m_timeLineLenMs);
	connect(m_ruler, SIGNAL(timePositionClicked(int)), this, SLOT(setCursorPos(int)));

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

void TimeLineWidget::clear()
{
	m_rightMostItem = nullptr;
	// delete all tracks except the ruler track
	while (m_tracks.size() > 1)
		delete m_tracks.takeLast();
}

/**
 * @brief Returns the vertical space needed in order to display all tracks in full height
 *
 * The functions returns at least the height of 2 tracks
 */
int TimeLineWidget::timeLineHeight() const
{
	return qMax(m_tracks.last()->yEndPos(), m_defaultTrackHeight * 2 + 10);
}

bool TimeLineWidget::addTimeLineTrack()
{
	int newTrackId = m_tracks.size();
	if (newTrackId >= TIMELINE_MAX_TRACKS)
		return false;

	TimeLineTrack *track = new TimeLineTrack(TRACK_ITEMS, newTrackId, m_tracks.last()->yEndPos(), m_defaultTrackHeight);
	m_tracks.append(track);
	// update scene in order to draw the background
	m_scene->update();

	return true;
}


/**
 * @brief Add given track to timeline
 * @param track TimeLineTrack object to be added
 * @return true on added. false, if track was not added
 */
bool TimeLineWidget::addTimeLineTrack(TimeLineTrack *track)
{
	bool ok = true;
	if (!m_tracks.isEmpty()) {
		for (int t=0; t<m_tracks.size(); t++) {
			if (track->trackId() == m_tracks.at(t)->trackId()) {
				qWarning() << "track with ID" << track->trackId() << "already in timeline";
				track->m_trackID = m_tracks.size();
			}
		}
		track->setYPos(m_tracks.last()->yEndPos());
	}
	else {
		track->setYPos(0);
	}
	m_tracks.append(track);

	// update scene in order to draw the background
	m_scene->update();
	return ok;

}

bool TimeLineWidget::addAudioEnvelopeTrack()
{
	int newTrackId = m_tracks.size();
	if (newTrackId >= TIMELINE_MAX_TRACKS)
		return false;

	TimeLineTrack *track = new TimeLineTrack(TRACK_AUDIO_ENV, newTrackId, m_tracks.last()->yEndPos(), m_defaultTrackHeight);
	track->trackBgColor = 0x3333555;
	m_tracks.append(track);

	// add TimeLineCurve to scene on timeline
	TimeLineCurve *curve = new TimeLineCurve(this, newTrackId);
	curve->setYPos(track->yPos());
	curve->setLabel("Audio Curve");

	track->appendTimeLineItem(curve);

	// update scene in order to draw the background
	m_scene->update();

	return true;
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
	while (m_tracks.size() <= trackID) {
		TimeLineTrack *track = new TimeLineTrack(TRACK_ITEMS, trackID, m_tracks.last()->yEndPos(), m_defaultTrackHeight);
		m_tracks.append(track);
	}

	// this is the track instance
	TimeLineTrack *track = m_tracks.at(trackID);

	// Now create an item and add it to the track
	TimeLineItem *item = createNewTimeLineItem(this, trackID);
	if (!item->isFirstInitReady()) {
		item->setLabel(label);
		item->setDuration(durationMs);
		item->setPosition(posMs);
	}
	item->setYPos(track->yPos());
	track->appendTimeLineItem(item);

	// finaly add the item to the scene
	m_scene->addItem(item);
	return item;
}

TimeLineBase *TimeLineWidget::at(int trackID, int idx)
{
	if (trackID < 0 || trackID >= m_tracks.size())
		return nullptr;

	// this is the track instance
	TimeLineTrack *track = m_tracks.at(trackID);

	return track->itemAt(idx);
}

bool TimeLineWidget::removeTimeLineItem(TimeLineBase *item, bool deleteLater)
{
	for (int t=0; t<m_tracks.size();t++) {
		TimeLineTrack *track = m_tracks.at(t);
		if (track->itemList().contains(item)) {
			// remove item from scene
			m_scene->removeItem(item);
			// remove item from internal list
			track->itemList().removeOne(item);
			// delete instance later, if requested
			if (deleteLater)
				item->deleteLater();
		}
	}

	return false;
}

/**
 * @brief Return the amount of elements in the timeline for the requested track id
 * @param trackID
 * @return
 */
int TimeLineWidget::timeLineSize(int trackID) const
{
	if (trackID < 0 || trackID >= m_tracks.size())
		return 0;

	return m_tracks.at(trackID)->itemCount();
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

int TimeLineWidget::leftViewPixel() const
{
	return gfxView()->horizontalScrollBar()->value();
}

int TimeLineWidget::rightViewPixel() const
{
	return this->width() + gfxView()->horizontalScrollBar()->value();
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
		setTimeLineDuration(newRightItem->endPosition());
	}

	if (m_rightMostItem != newRightItem) {
		m_rightMostItem = newRightItem;
		emit furthestRightItemChanged(m_rightMostItem);
		qDebug() << "furthest right item changed" << m_rightMostItem->label();
		qDebug() << "endPos" << m_rightMostItem->endPosition() << "timeline len [ms]" << m_timeLineLenMs;
		return true;
	}

	return false;
}

TimeLineItem *TimeLineWidget::findRightMostItem() const
{
	TimeLineItem *rItem = nullptr;
	const QList<QGraphicsItem*> items = m_scene->items();
	for (auto *it : items) {
		TimeLineItem *tlItem = dynamic_cast<TimeLineItem *>(it);
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

/**
 * @brief Return current position of time line cursor (indicator)
 * @return int position [ms]
 */
int TimeLineWidget::cursorPos() const
{
	return m_cursor->position();
}


/**
 * @brief Returns trackID for given y position
 * @param y
 * @return trackID or -1, if no track
 *
 * @note the first track has trackID 1 (trackID 0 is the timeline ruler)
 */
int TimeLineWidget::yPosToTrackId(int y)
{
	for (int t=0; t<m_tracks.size(); t++) {
		if (m_tracks.at(t)->isInYRange(y))
			return m_tracks.at(t)->trackId();
	}

	return -1;
}

void TimeLineWidget::setTimeLineDuration(int ms)
{
	if (m_timeLineLenMs != ms) {
		m_timeLineLenMs = ms;

		m_ruler->setTimeLineDuration(ms);
		adjustSceneRectToTimelineLength();

		for (int t=1; t<m_tracks.size(); t++) {
			// if (m_tracks.at(t)->trackType() == TRACK_AUDIO_ENV)

		}

		updateScene();
		emit timeLineDurationChanged(ms);
	}
}

void TimeLineWidget::setTimeLineViewRangeMs(int ms)
{
	if (m_viewRangeMs == ms)
		return;

	qreal xsize = m_view->width() - 4;

	// adjust view range and transformation factor
	m_viewRangeMs = ms;
	m_msPerPixel = qreal(m_viewRangeMs) / (xsize - m_rightMargin);
	qDebug() << "set timelineviewrange msPerPixel" << m_msPerPixel;

	// readjust sceneRect since pixel transformation has changed
	adjustSceneRectToTimelineLength();
	// update pixel positions in all items
	recalcPixelPosInAllItems();

	m_navLabel->setText(QString("view range: %1s").arg(float(ms)/1000));
}

void TimeLineWidget::setCursorPos(int ms)
{
	// qDebug() << "set cursor pos" << ms;
	if (m_cursor->isClicked())
		return;

	if (m_cursor->position() != ms) {
		emit cursorPosChanged(ms);
	}
	m_cursor->setPosition(ms);
	// m_cursor->setPos(QPointF(ms / m_msPerPixel, m_cursor->y()));
}

void TimeLineWidget::updateScene()
{
	// qDebug() << "updateScene called";
	if (m_scene)
		m_scene->update();
}

/**
 * @brief TimeLineWidget::resizeEvent
 * @param event
 *
 * @note TimeLineWidget::resizeEvent(...) is called after TimeLineGfxView::resizeEvent(...), which is part of this class !!
 */
void TimeLineWidget::resizeEvent(QResizeEvent */*event*/)
{
	// qDebug() << "  -> resize widget" << event->size() << "view size" << m_view->size() << "stored size" << m_view->currentSize();

	// get size of child TimeLineGfxView

	qreal xsize = m_view->width() - 4;
	// qreal xvsize = m_view->viewport()->geometry().width();
	// qreal ysize = m_view->height() - 4;

	// calculate pixel/time convertion factors
	// if this is the first call, the scale is set. Otherwise the viewRange is adopted.
	if (m_msPerPixel == 0) {
		m_msPerPixel = qreal(m_viewRangeMs) / (xsize - m_rightMargin);
		// qDebug() << "  -> resize set msPerPixel" << m_msPerPixel << "";
	} else {
		m_viewRangeMs = m_msPerPixel * xsize;
		m_viewRangeSlider->setValue(m_viewRangeMs);
	}

	// setViewRect of screen to the x-size of the complete timeline
	// if timeline is smaller than visible view, the view rect is extended to the view size
	adjustSceneRectToTimelineLength();

	m_navLabel->setText(QString("view range: %1s").arg(float(m_viewRangeMs)/1000));

	// qDebug() << "ms per pixel" << m_msPerPixel << m_scene->sceneRect();

	m_cursor->setHeight(m_view->height());
}

void TimeLineWidget::mouseDoubleClickEvent(QMouseEvent */*event*/)
{
	// setTimeLineViewRangeMs(30000);
	// clear();
	// currentVisibleRect();
	// setTimeLineDuration(100000);
	updateScene();
}

void TimeLineWidget::showEvent(QShowEvent */*event*/)
{
	/// @todo blöder Workaround, um eine Update der Timeline herbeizuführen, nachdem
	/// das Widget sichtbar wird
	QTimer::singleShot(10, this, SLOT(updateScene()));
}


/**
 * @brief Create a standard TimeLineItem and return address.
 * @param timeline
 * @param trackId
 * @return Address of newly created item.
 *
 * Reimplement this function in your derived class in order to automatically generate
 * more complex TimeLineItems.
 */
TimeLineItem *TimeLineWidget::createNewTimeLineItem(TimeLineWidget *timeline, int trackId)
{
	return new TimeLineItem(timeline, trackId);
}

/**
 * @brief Create a standard TimeLineGfxScene and return address.
 * @param timeline
 * @return
 *
 * Reimplement this function for customized derived Scene classes
 */
TimeLineGfxScene *TimeLineWidget::createTimeLineScene(TimeLineWidget *timeline)
{
	return new TimeLineGfxScene(timeline);
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
		TimeLineBase *tlItem = dynamic_cast<TimeLineBase*>(it);
		if (tlItem) {
			qDebug() << "type" << tlItem->type();
			tlItem->recalcPixelPos();
			// tlItem->update();
		}
	}
	m_scene->update();
}

void TimeLineWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QPointF pos1 = m_view->mapFromGlobal(event->globalPos());
	int clickedTrackID = yPosToTrackId(pos1.y());
	int ms = pixelToMs(pos1.x());
	qDebug() << "ms" << ms << "pixel" << pos1.x();

	QMenu menu(this);
	QAction *act;
	if (clickedTrackID > 0) {
		act = menu.addAction(tr("Add new Item"));
		act->setObjectName("newItem");
	}
	else if (clickedTrackID < 0) {
		act = menu.addAction(tr("Add new timeline track"));
		act->setObjectName("newTrack");

		act = menu.addAction(tr("Add audio envelope track"));
		act->setObjectName("newAudioEnvelope");
	}

	act = menu.exec(event->globalPos());
	if (!act) return;

	QString cmd = act->objectName();
	if (cmd == "newItem") {
		// if (!m_itemLists[1].isEmpty())
		// 	ms = m_itemLists[1].last()->endPosition();
		/*TimeLineItem *item = */addTimeLineItem(ms, 10000, QString("item t%1").arg(clickedTrackID), clickedTrackID);
	}
	else if (cmd == "newTrack") {
		/*bool ok = */addTimeLineTrack();
	}
	else if (cmd == "newAudioEnvelope") {
		addAudioEnvelopeTrack();
	}

}

void TimeLineWidget::onCursorMoved(int ms)
{
	emit cursorPosChanged(ms);
	emit cursorActivated(ms);
}

void TimeLineWidget::onMouseHovered(const QPointF &pos)
{
	int xMs = pixelToMs(pos.x());
	if (xMs > 0)
		emit mousePosMsChanged(xMs);
}

} // namespace PS_TL

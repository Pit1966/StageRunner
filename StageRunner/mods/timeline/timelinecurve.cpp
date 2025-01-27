#include "timelinecurve.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QAction>
#include <QMenu>

namespace PS_TL {

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

QPointF Node::nodePixel(qreal msPerPix) const
{
	x = xMs/msPerPix;
	return QPointF(x, scaledY());
}

int Node::scaledY() const
{
	int b = 3;
	int h = m_myTrack->ySize() - 2 * b;		// track height

	y = h - (h * yPM / 1000) + b;
	return y;
}

int Node::unScaleY(qreal y) const
{
	int b = 3;
	int h = m_myTrack->ySize() - 2 * b;

	int uy = (h - y + b) * 1000 / h;
	return uy;
}

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------
/// Static callback functions for context menus

TimeLineCurve * TimeLineCurve::staticTmpContextObj = nullptr;

void staticAddNode()
{
	if (TimeLineCurve::staticTmpContextObj) {
		TimeLineCurve::staticTmpContextObj->addNodeAtXpos(TimeLineCurve::staticTmpContextObj->lastContextMenuClickPos().x());
	}
}

void staticDetachOverlay()
{
	if (TimeLineCurve::staticTmpContextObj) {
		TimeLineCurve::staticTmpContextObj->myTimeLine()->setTrackOverlay(TimeLineCurve::staticTmpContextObj->trackID(), false);
	}
}


// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------


/**
 * @brief Insert node at correct positon in time sorted node list
 * @param n
 */
void TimeLineCurveData::insertNode(const Node &n)
{
    int i = 0;
	while (i < m_nodes.size() && n.xMs > m_nodes.at(i).xMs)
		i++;
    m_nodes.insert(i, n);
}


/**
 * @brief get yPM value at time position
 * @param ms
 * @return int yPM (should always be valid)
 *
 * first the 2 nodes are searched, which are nearest the given time position.
 * Than the final y value is interpolated from these nodes
 */
qreal TimeLineCurveData::valAtMs(int ms)
{
	// find 2 nodes. One to the left of ms and one to the right of ms (or equal)
	int idx1 = 0;

	while (idx1 < m_nodes.size() - 1 && ms >= m_nodes.at(idx1).xMs)
		idx1++;

	idx1--;

	int idx2 = idx1+1;
	if (idx2 >= m_nodes.size()) // should not happen, but for crash prevention
		return 0;

	const Node &n1 = m_nodes.at(idx1);
	const Node &n2 = m_nodes.at(idx2);

	int xd = n2.xMs - n1.xMs;
	if (xd == 0) {			// nodes are exactly at the same time position (can this happen?)
		return n1.yPM;		// then simply return the value
	}
	qreal yd = n2.yPM - n1.yPM;
	qreal xp = ms - n1.xMs;

	qreal yp = xp * yd / xd;
	return (yp + n1.yPM);
}

/**
 * @brief Returns the TimeLineCurve nodes (and maybe more) formatted as a string
 * @return string of format: "nodes:<xMs>,<yPM>;<xMs>,<yPM>"
 */
QString TimeLineCurveData::getCurveData() const
{
	QString dat;

	// endcode all nodes as string and return it
	if (m_nodes.size()) {
		dat += "nodes:";
		for (int t=0; t<m_nodes.size(); t++) {
			const Node &n = m_nodes.at(t);
			dat += QString("%1,%2;").arg(n.xMs).arg(n.yPM);
		}
		dat.chop(1);
	}

	return dat;
}

/**
 * @brief TimeLineCurveData::setCurveData
 * @param dat
 * @param track
 * @return
 *
 * Data format of curve data:
 * 1 - string starts with "nodes:" keyword
 * 2 - than follow comma separated the values of one node.
 * 3 - the node is terminated with a semicolon. goto 2 until all nodes are stored
 *
 * "nodes:<xMs>,<yPM>;<xMs>,<yPM>"
 */
bool TimeLineCurveData::setCurveData(const QString &dat, TimeLineTrack *track)
{
	bool ok = true;
	bool newnodeformat = false;

	m_nodes.clear();

	// first split the groups, seperated by _$_
	const QStringList groups = dat.split("_$_");
	for (const QString &str : groups) {
		// get the keyword vor the group, separated by :
		const QStringList g = str.split(':');
		if (g.size() < 2) // not valid or empty
			continue;
		QString gkey = g.at(0).toLower();
		QString gdat = g.at(1);

		// read data of group type
		if (gkey == "nodes") {
			// nodes are separated by ;
			const QStringList nodes = gdat.split(';');
			// loop over all nodes
			for (const QString &nstr : nodes) {
				// a node consists of a list of data points seperated by a comma
				int expected = 2;
				const QStringList np = nstr.split(',');
				if (np.size() >= expected) {	// we expect 2 values here
					// create a new node
					Node n(track);			// m_myTrack must be valid here
					n.xMs = np.at(0).toInt();
					n.yPM = np.at(1).toInt();

					if (np.size() > expected) {	// more data than wie need (from newer stagerunner version?)
						if (!newnodeformat) {
							newnodeformat = true;
							qDebug() << "TimeLineCurve: Found node with more parameters than expected. Maybe from newer stagerunner version";
						}
					}

					// add node
					m_nodes.append(n);
				}
				else { // missing data (from old stagerunner version or invalid)
				}

			}
		}
		else { // unknown group
			ok = false;
			qWarning () << gkey << "TimeLineCurve: unknown group type in timeline curve config";
		}
	}

	return ok;
}


// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

TimeLineCurve::TimeLineCurve(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
{
	setAcceptHoverEvents(true);

	m_xSize = 500;
	m_xSize = 30;
	m_colorBG = 0x333355;

	m_myTrack = m_timeline->findTrackWithId(trackId);
	m_ySize = m_myTrack->ySize();

	// We must have to nodes!
	m_nodes.append(Node(m_myTrack, 0, 1000));
	m_nodes.append(Node(m_myTrack, 60000, 1000));
}

void TimeLineCurve::setTimeLineDuration(int ms)
{
	setDuration(ms);

	// delete all nodes that lies beyond the duration
	Node last = m_nodes.takeLast();
	last.xMs = ms;

	while (m_nodes.size() > 1 && m_nodes.last().xMs >= ms)
		m_nodes.removeLast();

	// append last node again
	m_nodes.append(last);

	recalcPixelPos();
}

bool TimeLineCurve::hasOverLayContextMenu() const
{
	return true;
}

QList<TimeLineContextMenuEntry> TimeLineCurve::getOverlayContextMenu(const QPointF &/*scenePos*/)
{
	QList<TimeLineContextMenuEntry> list;

	list.append(TimeLineContextMenuEntry(tr("---------------")));
	list.append(TimeLineContextMenuEntry(tr("Add node"), "addNode"));
	list.last().staticCmdFunc = &staticAddNode;
	list.append(TimeLineContextMenuEntry(tr("Show overlay in own track"), "clearOver"));
	list.last().staticCmdFunc = &staticDetachOverlay;

	staticTmpContextObj = this;
	return list;
}

// bool TimeLineCurve::execContextMenuCmd(const TimeLineContextMenuEntry *menuEntry)
// {
// 	if (menuEntry->menuId == "addNode") {
// 		Node n(m_myTrack);
// 		n.xMs = m_timeline->pixelToMs(m_lastContextMenuClickPos.x());
// 		n.yPM = valAtMs(n.xMs);

// 		insertNode(n);
// 		update();
// 	}
// 	else if (menuEntry->menuId == "clearOver") {
// 		m_timeline->setTrackOverlay(m_trackId, false);
// 	}
// 	else {
// 		return false;
// 	}

// 	return true;
// }

QString TimeLineCurve::getConfigDat() const
{
	return getCurveData();
}

bool TimeLineCurve::setConfigDat(const QString &dat)
{
	return setCurveData(dat, m_myTrack);
}

void TimeLineCurve::addNodeAtXpos(qreal xpix)
{
	qDebug() << "add node at" << xpix;
	Node n(m_myTrack);
	n.xMs = m_timeline->pixelToMs(xpix);
	n.yPM = valAtMs(n.xMs);

	insertNode(n);
	update();
}

// void TimeLineCurve::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
// {
// 	qreal cx = event->pos().x();
// 	qreal cy = event->pos().y();

// 	mouseHoverEvent(cx, cy);
// }

void TimeLineCurve::hoverLeaveEvent(QGraphicsSceneHoverEvent */*event*/)
{
	if (m_curHoveredNode >= 0) {
		m_curHoveredNode = -1;
		update();
	}
}

void TimeLineCurve::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_curHoveredNode >= 0) {
		m_clickNodePos = event->pos();
		m_nodeClicked = true;
	}
	else if (m_myTrack->isOverlay()) {
		event->ignore();
		return;
	}
}

void TimeLineCurve::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_nodeClicked && m_curHoveredNode >= 0) {
		int c = m_curHoveredNode;
		QPointF curPos = event->pos();
		qreal xdif = curPos.x() - m_clickNodePos.x();
		qreal ydif = curPos.y() - m_clickNodePos.y();

		qreal newx = m_clickNodePos.x() + xdif;
		qreal newy = m_clickNodePos.y() + ydif;

		Node &n = m_nodes[c];
		int ms = m_timeline->pixelToMs(newx);

		// The first and the last node must not be moved in x direction
		if (c > 0 && c < m_nodes.size() - 1) {
			// its not allowed to move a node past its neighbour. Check this here !!
			if (ms <= m_nodes.at(c-1).xMs) {
				ms = m_nodes.at(c-1).xMs + 1;
			}
			else if (ms >= m_nodes.at(c+1).xMs) {
				ms = m_nodes.at(c+1).xMs - 1;
			}

			n.xMs = ms;
		}

		n.yPM = n.unScaleY(newy);
		if (n.yPM < 0)
			n.yPM = 0;
		if (n.yPM > 1000)
			n.yPM = 1000;
		update();
	}
}

void TimeLineCurve::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
{
	m_nodeClicked = false;
	m_curHoveredNode = -1;
}

void TimeLineCurve::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	// unselect hover point

	QPoint globPos = event->screenPos();
	QPointF pos = event->pos();
	m_lastContextMenuClickPos = pos;

	int m_selNodeWhenClicked = m_curHoveredNode;

	QMenu menu(m_timeline);
	QAction *act;

	// there are different context menus whether a node is selected or not
	if (m_selNodeWhenClicked >= 0) {
		act = menu.addAction(tr("Remove node"));
		act->setObjectName("remClickedNode");
	}
	else if (m_myTrack->isOverlay()) {
		qDebug() << "last context menu pos" << pos;
		event->ignore();
		return;
	}
	else {
		act = menu.addAction(tr("Add node"));
		act->setObjectName("addNode");

		if (m_myTrack->ySize() < 70) {
			act = menu.addAction(tr("Expand track height"));
			act->setObjectName("expandTrack");
		} else {
			act = menu.addAction(tr("Shrink track height"));
			act->setObjectName("shrinkTrack");
		}

		if (m_myTrack->isOverlay()) {
			act = menu.addAction(tr("Show overlay in own track"));
			act->setObjectName("clearOver");
		} else {
			act = menu.addAction(tr("Make overlay"));
			act->setObjectName("makeOver");
		}

		menu.addAction(tr("------------"));

		act = menu.addAction(tr("Delete track"));
		act->setObjectName("delTrack");
	}

	act = menu.exec(globPos);
	m_nodeClicked = false;
	m_curHoveredNode = -1;
	if (!act) return;

	QString cmd = act->objectName();
	if (cmd == "addNode") {
		addNodeAtXpos(pos.x());
	}
	else if (cmd == "delTrack") {
		m_timeline->deleteTimeLineTrack(m_trackId);
	}
	else if (cmd == "remClickedNode") {
		// remember: first and last node should not be deleted
		if (m_selNodeWhenClicked > 0 && m_selNodeWhenClicked < m_nodes.size() -1) {
			m_nodes.removeAt(m_selNodeWhenClicked);
			update();
		}
	}
	else if (cmd == "expandTrack") {
		m_timeline->setTrackHeight(m_trackId, 80);
	}
	else if (cmd == "shrinkTrack") {
		m_timeline->setTrackHeight(m_trackId, m_timeline->defaultTrackHeight());
	}
	else if (cmd == "makeOver") {
		m_timeline->setTrackOverlay(m_trackId, true);
	}
	else if (cmd == "clearOver") {
		m_timeline->setTrackOverlay(m_trackId, false);
	}
}

void TimeLineCurve::recalcPixelPos()
{
	setX(0);
	if (m_timeline->msPerPixel() > 0) {
		m_xSize = m_timeline->msToPixel(duration());
		m_isPixelPosValid = true;
	} else {
		m_isPixelPosValid = false;
	}
}

bool TimeLineCurve::mouseHoverEvent(qreal x, qreal y)
{
	if (m_nodeClicked)
		return false;

	for (int t=0; t<m_nodes.size(); t++) {
		const Node &n = m_nodes.at(t);
		if (qAbs(x - n.x) < 5 && qAbs(y - n.y) < 5) {
			if (t != m_curHoveredNode) {
				m_curHoveredNode = t;
				update();
			}
			return true;
		}
	}

	if (m_curHoveredNode >= 0) {
		m_curHoveredNode = -1;
		update();
	}

	return false;
}

void TimeLineCurve::setTrackDuration(int ms)
{
	setTimeLineDuration(ms);
}

QRectF TimeLineCurve::boundingRect() const
{
	QRectF br(0, 0, m_xSize, m_ySize + 2);
	return br;
}

void TimeLineCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_isTimePosValid) {
		m_xSize = m_timeline->msToPixel(duration());
	}

	// painter->setBrush(m_colorBG);
	// painter->setPen(Qt::NoPen);
	// painter->drawRect(0, 0, m_xSize, m_ySize);

	painter->setRenderHint(QPainter::Antialiasing);

	QPen pen;
	pen.setColor(m_colorBorder);
	painter->setPen(pen);

	if (!m_myTrack)
		m_myTrack = m_timeline->findTrackWithId(m_trackId);
	if (!m_myTrack)
		return;

	if (!m_myTrack->isOverlay()) {
		QRectF textRect = boundingRect();
		textRect.adjust(5,-2,-5,-10);
		painter->drawText(textRect, m_label);
	}


	// draw nodes
	qreal mps = m_timeline->msPerPixel();
	// qreal trackH = m_myTrack->ySize();


	painter->setPen(QColor(m_colorNode));
	for (int t=1; t<m_nodes.size(); t++) {
		const Node &n1 = m_nodes.at(t-1);
		const Node &n2 = m_nodes.at(t);
		painter->drawLine(QPointF(n1.nodePixel(mps)), QPointF(n2.nodePixel(mps)));
	}

	painter->setPen(Qt::NoPen);
	for (int t=0; t<m_nodes.size(); t++) {
		if (t == m_curHoveredNode) {
			painter->setBrush(QColor(m_colorNodeHov));
		} else {
			painter->setBrush(QColor(m_colorNode));
		}

		const Node &n = m_nodes.at(t);
		QPointF p = n.nodePixel(mps);
		painter->drawEllipse(p, 3, 3);
	}
}



} // namespace PS_TL

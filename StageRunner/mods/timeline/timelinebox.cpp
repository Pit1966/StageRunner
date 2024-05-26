#include "timelinebox.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QDebug>

namespace PS_TL {

TimeLineBox::TimeLineBox(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
{
	setAcceptHoverEvents(true);

	m_colorBG		= 0x225522;
	m_colorBorder   = 0x225522;		// 0xeeeeee;

}

void TimeLineBox::moveToEndPosition(int ms)
{
	if (ms != endPosition()) {
		m_isPixelPosValid = false;
		m_isTimePosValid = true;
		setPosition(ms - duration());
	}
}


void TimeLineBox::recalcPixelPos()
{
	if (m_isTimePosValid) {
		m_xSize = qreal(duration()) / m_timeline->msPerPixel();
		qDebug() << label() << "recalc m_xSize" << m_xSize;
		setX(qreal(position()) / m_timeline->msPerPixel());
	}
}

QRectF TimeLineBox::boundingRect() const
{
	return QRectF(0, 0, m_xSize + m_penWidthBorder, m_ySize + m_penWidthBorder);
}

void TimeLineBox::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_timeline->msPerPixel() <= 0)
		return;

	if (m_isTimePosValid) {
		if (!m_isPixelPosValid) {
			setX(qreal(position()) / m_timeline->msPerPixel());
			m_xSize = qreal(duration()) / m_timeline->msPerPixel();
		}
	}
	else {
		m_isTimePosValid = true;
		setPosition(m_timeline->msPerPixel() * x());
		setDuration(m_timeline->msPerPixel() * m_xSize);
	}

	QPen pen;
	pen.setColor(m_colorBorder);
	pen.setWidthF(m_penWidthBorder);
	painter->setPen(pen);
	painter->setBrush(QColor(m_colorBG));
	painter->drawRect(m_penWidthBorder/2, m_penWidthBorder/2, m_xSize, m_ySize);

	painter->setPen(QColor(m_colorText));
	QRectF textRect = boundingRect();
	textRect.adjust(5,-2,-5,-3);
	painter->drawText(textRect, m_label);

	m_wasPainted = true;
}

void TimeLineBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_clickPos = event->scenePos();
		// store current position for move
		m_itemPos = scenePos();
		m_isClicked = true;
		// store some of the current members for calculation of new values by mouse moving
		m_clickXSize = m_xSize;
		m_clickFadeInMs = fadeInTime();
		m_clickFadeOutMs = fadeOutTime();
		qDebug() << "clicked item" << m_label << "at pos" << m_itemPos << "duration [ms]" << duration();
		leftClicked(event);
	}
	else if (event->button() == Qt::RightButton) {
		rightClicked(event);
	}
	else {
		m_isClicked = false;
	}
}

void TimeLineBox::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isClicked) {
		QPointF curPos = event->scenePos();
		qreal xdif = curPos.x() - m_clickPos.x();
		// qreal ydif = curPos.y() - m_clickPos.y();

		if (m_grabMode == GRAB_RIGHT) {
			m_xSize = m_clickXSize + xdif;
			qreal xSizeMs = m_xSize * m_timeline->msPerPixel();
			qreal maxDurMs = maxDuration();
			if (xSizeMs < fadeOutTime() + fadeInTime()) {
				xSizeMs = fadeOutTime() + fadeInTime();
			}
			else if (maxDurMs > 0 && xSizeMs > maxDurMs) {
				xSizeMs = maxDurMs;
				m_xSize = m_timeline->msToPixel(maxDurMs);
			}
			setDuration(xSizeMs);

			// this schedules an graphics update
			setPos(pos() + QPointF(1,0));
			setPos(pos() - QPointF(1,0));
		}
		else if (m_grabMode == GRAB_LEFT) {
			qreal newX = m_itemPos.x() + xdif;
			qreal newY = m_itemPos.y();
			qreal newXSize = m_clickXSize - xdif;

			qreal maxXSize = m_timeline->msToPixel(maxDuration());
			qreal minXSize = m_timeline->msToPixel(fadeInTime() + fadeOutTime());
			if (newXSize < minXSize) {
				newX -=  minXSize - newXSize;
				newXSize = minXSize;
			}
			else if (maxXSize > 0 && newXSize > maxXSize) {
				newX += newXSize - maxXSize;
				newXSize = maxXSize;
			}

			// calc new time position and new duration
			setPosition(m_timeline->msPerPixel() * newX);
			setDuration(m_timeline->msPerPixel() * newXSize);

			// set new item pos
			setPos(newX, newY);
		}
		else if (m_grabMode == GRAB_FADEIN) {
			int fadein_ms = m_timeline->pixelToMs(xdif) + m_clickFadeInMs;
			if (fadein_ms < 0)
				fadein_ms = 0;
			if (fadein_ms > duration() - fadeOutTime())
				fadein_ms = duration() - fadeOutTime();
			setFadeInTime(fadein_ms);
			update();
		}
		else if (m_grabMode == GRAB_FADEOUT) {
			int fadeout_ms = m_clickFadeOutMs - m_timeline->pixelToMs(xdif);
			if (fadeout_ms < 0)
				fadeout_ms = 0;
			if (duration() - fadeout_ms < fadeInTime())
				fadeout_ms = duration() - fadeInTime();
			setFadeOutTime(fadeout_ms);
			update();
		}
		else {
			qreal newX = m_itemPos.x() + xdif;
			qreal newY = m_itemPos.y() /*+ ydif*/;
			if (newX < 0)
				newX = 0;
			if (newY < 0)
				newY = 0;

			// calc new time position
			setPosition(m_timeline->msPerPixel() * newX);
			// set new item pos
			setPos(newX, newY);

			qDebug() << m_label << "current timepos" << position() << "size" << duration();
		}
	}
}

void TimeLineBox::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/)
{
	m_isClicked = false;
	m_grabMode = GRAB_NONE;
	m_timeline->checkForRightMostItem(this);
}

void TimeLineBox::hoverEnterEvent(QGraphicsSceneHoverEvent */*event*/)
{
	// qDebug() << "hover enter" << m_label;
	m_isHover = true;
}

void TimeLineBox::hoverLeaveEvent(QGraphicsSceneHoverEvent */*event*/)
{
	// qDebug() << "hover leave" << m_label;
	m_isHover = true;
}

void TimeLineBox::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	if (m_isClicked)
		return;

	//qDebug() << "hover move" << m_label << event->pos() << "fadeout ms" << fadeOutTime();
	qreal x = event->pos().x();

	int fadein_x = m_timeline->msToPixel(fadeInTime());
	int fadeout_x = 0;
	if (fadeOutTime() > 0)
		fadeout_x = m_timeline->msToPixel(duration() - fadeOutTime());

	if (fadein_x > 0 && qAbs(fadein_x - x) < 4) {
		setCursor(Qt::SplitHCursor);
		m_grabMode = GRAB_FADEIN;
	}
	else if (fadeout_x > 0 && qAbs(fadeout_x - x) < 4) {
		setCursor(Qt::SplitHCursor);
		m_grabMode = GRAB_FADEOUT;
	}
	else if (x < 5) {
		setCursor(Qt::SizeHorCursor);
		m_grabMode = GRAB_LEFT;
	}
	else if (x > m_xSize - 5) {
		setCursor(Qt::SizeHorCursor);
		m_grabMode = GRAB_RIGHT;
	}
	else {
		setCursor(m_timeline->cursor());
		m_grabMode = GRAB_NONE;
	}

}

} // namespace PS_TL

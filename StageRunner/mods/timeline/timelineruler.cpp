#include "timelineruler.h"
#include "timelinewidget.h"

#include <QPainter>
#include <QDebug>
#include <math.h>
#include <QGraphicsSceneMouseEvent>

namespace PS_TL {

TimeLineRuler::TimeLineRuler(TimeLineWidget *timeline)
	: TimeLineBase(timeline)
{
	m_xSize = 500;
	m_ySize = 30;

	m_scaleFont = QFont("DejaVu Sans Mono", 9);
	m_scaleFont.setStyleHint(QFont::Monospace);
}

void TimeLineRuler::setTimeLineDuration(int ms)
{
	setDuration(ms);
	recalcPixelPos();
	// m_timeline->updateScene();
}

QString TimeLineRuler::msToTimeLineString(int ms, int msPrec)
{
	int min = ms / 60000;
	ms = ms % 60000;
	int sec = ms / 1000;
	ms = ms % 1000;

	return QString("%1m%2.%3s")
			.arg(min)
			.arg(sec, 2, 10, QLatin1Char('0'))
			.arg(ms, msPrec, 10, QLatin1Char('0'));
}

void TimeLineRuler::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	int x = event->pos().x();
	emit timePositionClicked(m_timeline->pixelToMs(x));
}

void TimeLineRuler::recalcPixelPos()
{
	setX(0);
	if (m_timeline->msPerPixel() > 0) {
		m_xSize = m_timeline->msToPixel(duration());
		m_isPixelPosValid = true;
	} else {
		m_isPixelPosValid = false;
	}
	findTicDistance();
}

QRectF TimeLineRuler::boundingRect() const
{
	 QRectF br(0, 0, m_xSize, m_ySize);
	 return br;
}

void TimeLineRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
	if (m_isTimePosValid) {
		m_xSize = m_timeline->msToPixel(duration());
	}

	painter->setPen(QPen());
	painter->setBrush(m_colorBG);
	painter->drawRect(0, 0, m_xSize, m_ySize);


	painter->setPen(QPen(m_colorRulerGrid));
	painter->setFont(m_scaleFont);

	if (!m_isGridDistValid)
		m_isGridDistValid = findTicDistance();

	if (m_isGridDistValid) {
		int ms = 0;
		while (ms < duration()) {
			qreal x = m_timeline->msToPixel(ms);
			painter->drawLine(x, m_ySize, x, m_ySize - 12);
			painter->drawText(x, 10, msToTimeLineString(ms));

			if (m_gridDistSub > 0) {
				int subMs = m_gridDistSub;
				while (subMs < m_gridDistMajor) {
					qreal x = m_timeline->msToPixel(ms + subMs);
					painter->drawLine(x, m_ySize, x, m_ySize - 5);
					subMs += m_gridDistSub;
				}
			}

			ms += m_gridDistMajor;
		}
	}
}

bool TimeLineRuler::findTicDistance()
{
	int xres = log10(duration());
	int griddist = pow(10,xres);
	int subdist = 0;

	// calc pixel per step
	int pixelPerTic = m_timeline->msToPixel(griddist);

	int subquot = 10;
	if (pixelPerTic > 10000) {
		griddist /= 100;
		subquot = 10;
	}
	else if (pixelPerTic > 5000) {
		griddist /= 50;
		subquot = 2;		//4;
	}
	else if (pixelPerTic > 2000) {
		griddist /= 20;
		subquot = 5;
	}
	else if (pixelPerTic > 1000) {
		griddist /= 10;
		subquot = 10;
	}
	else if (pixelPerTic > 300) {
		griddist /= 5;
		subquot = 2;		//4;
	}
	else if (pixelPerTic > 120) {
		griddist /= 2;
		subquot = 5;
	}

	subdist = griddist / subquot;
	// int pixelPerSubTic = m_timeline->msToPixel(subdist);
	// qDebug() << "griddist" << griddist << "pixelPerTic" << pixelPerTic << "subpixeldist" << pixelPerSubTic << "subquot" << subquot;


	m_gridDistMajor = griddist;
	m_gridDistSub = subdist;

	if (m_timeline->msToPixel(m_gridDistMajor) < 2)
		return false;

	return true;
}

} // namespace PS_TL

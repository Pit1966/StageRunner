#include "psvumeter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRect>

PsVuMeter::PsVuMeter(QWidget *parent)
	: qsynthMeter(parent)
	, m_volume(0.0f)
	, m_dragBeginVolume(0.0f)
	, m_mouseButton(0)
{
}

void PsVuMeter::setVolume(float vol)
{
	if (vol < 0.0f)
		vol = 0.0f;
	if (vol > 1.0f)
		vol = 1.0f;

	if (vol != m_volume) {
		m_volume = vol;
		update();
		emit valueChanged(m_volume);
		emit valueChanged(int(m_volume * 1000));
	}
}

void PsVuMeter::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	int xs = 14;
	int x = width() / 2 - 7;
	int ys = m_volume * height();
	int y = height() - ys;

	QColor col(0x006000);

	p.fillRect(x,y,xs,ys ,col);

}

void PsVuMeter::mousePressEvent(QMouseEvent *ev)
{

	if (ev->button() == Qt::LeftButton) {
		m_mouseButton = Qt::LeftButton;
		m_dragBeginMousePos = ev->pos();
		m_dragBeginVolume = m_volume;
	}
}

void PsVuMeter::mouseMoveEvent(QMouseEvent *ev)
{
	if (m_mouseButton == Qt::LeftButton) {
		QPoint move_dif = ev->pos() - m_dragBeginMousePos;
		float moved_norm = float(move_dif.y()) / float(height());
		setVolume(m_dragBeginVolume - moved_norm);
		update();
	}
}

void PsVuMeter::mouseReleaseEvent(QMouseEvent *)
{
	m_mouseButton = 0;

}

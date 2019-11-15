#include "psspinbox.h"

PsSpinBox::PsSpinBox()
{

}

PsSpinBox::PsSpinBox(QWidget *parent)
	: QSpinBox(parent)
	, m_pressed(false)
	, m_valueOnPress(0)
{

}

void PsSpinBox::mousePressEvent(QMouseEvent *event)
{
	m_pressed = true;
	m_valueOnPress = QSpinBox::value();
	QSpinBox::mousePressEvent(event);
}

void PsSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
	QSpinBox::mouseReleaseEvent(event);
	if (m_pressed && m_valueOnPress != QSpinBox::value()) {
		m_pressed = false;
		emit clickedAndChanged(QSpinBox::value());
	}
}

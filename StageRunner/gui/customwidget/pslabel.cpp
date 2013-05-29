#include "pslabel.h"

PsLabel::PsLabel(QWidget *parent) :
	QLabel(parent)
{
}

void PsLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit doubleClicked();
}

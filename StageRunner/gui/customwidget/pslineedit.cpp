#include "pslineedit.h"

PsLineEdit::PsLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
}

void PsLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit doubleClicked();
}

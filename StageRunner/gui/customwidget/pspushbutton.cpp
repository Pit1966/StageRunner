#include "pspushbutton.h"

#include <QEvent>
#include <QDebug>

PsPushButton::PsPushButton(QWidget *parent)
	: QPushButton(parent)
{

}

PsPushButton::PsPushButton(const QString &text, QWidget *parent)
	: QPushButton(text, parent)
{

}

bool PsPushButton::event(QEvent *e)
{
	// qDebug() << "event" << e->type();
	if (e->type() == QEvent::InputMethodQuery) {
		lastEventWasInputMethodQuery = true;
	} else {
		lastEventWasInputMethodQuery = false;
	}
	return QPushButton::event(e);
}

#include "pslineedit.h"
#include "qtstatictools.h"


PsLineEdit::PsLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	single_key_mode_f = false;
	current_key = 0;
	current_modifier = 0;
	prop_editable = true;
}

void PsLineEdit::setSingleKeyEditEnabled(bool state)
{
	single_key_mode_f = state;
}

void PsLineEdit::setEditable(bool state)
{
	if (state != prop_editable) {
		prop_editable = state;
		if (state && !isReadOnly()) {
			setStyleSheet("background-color: #ffe0e0");
		} else {
			setStyleSheet("background-color: #e0ffe0");
		}
	}
}

void PsLineEdit::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	// qDebug("PsLineEdit -> click");
	emit clicked();
}

void PsLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit doubleClicked();
}

void PsLineEdit::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	switch(key) {
	case Qt::Key_Control:
		current_modifier |= Qt::CTRL;
		break;
	case Qt::Key_Space:
	default:
		current_key = key;
	}

	// qDebug("Key Event: %s", QtStaticTools::keyToString(current_key, current_modifier).toLatin1().data());

	if (single_key_mode_f) {
		setText(QtStaticTools::keyToString(current_key,current_modifier));
	} else {
		QLineEdit::keyPressEvent(event);
	}
}

void PsLineEdit::keyReleaseEvent(QKeyEvent *event)
{
	bool doemit = false;

	int key = event->key();
	switch(key) {
	case Qt::Key_Control:
		current_modifier &= ~Qt::CTRL;
		break;
	default:
		current_key = 0;
		doemit = true;
	}

	if (single_key_mode_f) {
		if (doemit) emit textEdited(text());
	} else {
		QLineEdit::keyReleaseEvent(event);
	}
}

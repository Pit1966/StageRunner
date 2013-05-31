#include "pslineedit.h"
#include "qtstatictools.h"


PsLineEdit::PsLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	single_key_mode_f = false;
	current_key = 0;
	current_modifier = 0;
	prop_editable = true;
	prop_minimized = false;
	prop_events_shared = false;
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

void PsLineEdit::setMinimized(bool state)
{
	prop_minimized = state;
}

QSize PsLineEdit::sizeHint() const
{
	if (!prop_minimized)
		return QLineEdit::sizeHint();

	ensurePolished();
	QFontMetrics fm(font());
	int h = qMax(fm.height(), 10);
	int w = fm.width(text()) + 8;
	return QSize(w,h);
}

QSize PsLineEdit::minimumSizeHint() const
{
	if (!prop_minimized)
		return QLineEdit::minimumSizeHint();

	ensurePolished();
	QFontMetrics fm(font());
	int h = qMax(fm.height(), 10);
	int w = fm.boundingRect(text()).width();
	return QSize(w,h);
}

void PsLineEdit::mousePressEvent(QMouseEvent *event)
{
	// qDebug("PsLineEdit -> click");
	emit clicked();

	if (prop_events_shared)
		event->ignore();
}

void PsLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
	emit doubleClicked();
	if (prop_events_shared)
		event->ignore();
}

void PsLineEdit::mouseMoveEvent(QMouseEvent *event)
{
	if (prop_events_shared)
		event->ignore();
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

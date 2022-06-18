#include "psoverlaylabel.h"

#include <QMouseEvent>
#include <QDebug>

PsOverlayLabel::PsOverlayLabel(QWidget *parent)
	: QLabel(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	// setAttribute(Qt::WA_TransparentForMouseEvents);
	// setFocusPolicy(Qt::NoFocus);
	// setAttribute(Qt::WA_TranslucentBackground);
	// setText("My Label");
	setAutoFillBackground(true);
	setStyleSheet("background: black");
	setWindowOpacity(1);
	// setScaledContents(true);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	if (!parent) {
		setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	}
}

void PsOverlayLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
	emit doubleClicked();
	event->ignore();
}

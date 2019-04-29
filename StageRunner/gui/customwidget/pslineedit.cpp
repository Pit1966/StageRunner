//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "pslineedit.h"
#include "qtstatictools.h"

#include <QDebug>


PsLineEdit::PsLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	single_key_mode_f = false;
	current_key = 0;
	current_modifier = 0;
	prop_editable = true;
	prop_minimized = false;
	prop_events_shared = false;
	prop_warn_color = false;
	org_palette = palette();
}

void PsLineEdit::setSingleKeyEditEnabled(bool state)
{
	single_key_mode_f = state;
}

void PsLineEdit::setEditable(bool state)
{
	if (state != prop_editable) {
		prop_editable = state;
		if (prop_warn_color) updateEditWarnColor();
	}

}

void PsLineEdit::setDefaultPalette()
{
	setPalette(org_palette);
}

void PsLineEdit::setMinimized(bool state)
{
	prop_minimized = state;
}

void PsLineEdit::setWarnColor(bool state)
{
	if (state != prop_warn_color) {
		prop_warn_color = state;
		if (state) {
			updateEditWarnColor();
		} else {
			setDefaultPalette();
		}
	}
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

void PsLineEdit::updateEditWarnColor()
{
	QPalette pal = palette();

	if (prop_editable && !isReadOnly()) {
		pal.setColor(QPalette::Text,Qt::blue);
		pal.setColor(QPalette::Dark, QColor("#ef8896"));  // light red
		pal.setColor(QPalette::Shadow,QColor("#910e1e"));  // dark red
		pal.setColor(QPalette::Base, QColor("#c48b93"));  // middle red
		pal.setColor(QPalette::Window, QColor("#ef8896"));  // light red
		pal.setColor(QPalette::Light,QColor("#910e1e"));  // dark red
	} else {
		pal.setColor(QPalette::Text,Qt::blue);
		pal.setColor(QPalette::Dark, QColor("#9fdaa2"));  // light green
		pal.setColor(QPalette::Shadow,QColor("#374c38"));  // dark green
		pal.setColor(QPalette::Base, QColor("#8bbf8d"));  // middle green
		pal.setColor(QPalette::Window, QColor("#9fdaa2"));  // light green
		pal.setColor(QPalette::Light,QColor("#374c38"));  // dark green
	}
	setPalette(pal);

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
	if (prop_editable)
		QLineEdit::selectAll();

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
	case Qt::Key_Tab:
		emit tabPressed();
		break;
	case Qt::Key_Return:
		emit enterPressed();
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

bool PsLineEdit::event(QEvent *event)
{
	if (event->type() == 6) {
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		int key = ev->key();
		if (key == Qt::Key_Tab) {
			emit tabPressed();
		}
	}
	return QLineEdit::event(event);
}

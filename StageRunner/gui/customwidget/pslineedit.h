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

#ifndef PSLINEEDIT_H
#define PSLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QPalette>

class PsLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
	Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized)
	Q_PROPERTY(bool eventsShared READ eventsShared WRITE setEventsShared)
	Q_PROPERTY(bool warnColor READ warnColor WRITE setWarnColor)

private:
	bool prop_editable;
	bool prop_minimized;
	bool prop_events_shared;
	bool prop_warn_color;
	bool single_key_mode_f;
	int current_key;
	int current_modifier;
	QPalette org_palette;

public:
	PsLineEdit(QWidget *parent = 0);
	inline int currentKey() {return current_key;}
	inline int currentModifier() {return current_modifier;}
	void setSingleKeyEditEnabled(bool state);
	void setEditable(bool state);
	void setDefaultPalette();
	bool isEditable() const {return prop_editable;}
	void setMinimized(bool state);
	bool isMinimized() const {return prop_minimized;}
	void setEventsShared(bool state) {prop_events_shared = state;}
	bool eventsShared() {return prop_events_shared;}
	void setWarnColor(bool state);
	bool warnColor() const {return prop_warn_color;}

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	void updateEditWarnColor();

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	bool event(QEvent *event);


signals:
	void clicked();
	void doubleClicked();
	void tabPressed();
	void enterPressed();

public slots:

};

#endif // PSLINEEDIT_H

//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef FXSCRIPTWIDGET_H
#define FXSCRIPTWIDGET_H

#include "ui_fxscriptwidget.h"

class FxScriptItem;
class FxScriptWidget;
class FxItem;

class FxScriptWidget : public QWidget, private Ui::FxScriptWidget
{
	Q_OBJECT
private:
	static QList<FxScriptWidget*>m_scriptWidgetList;
	FxScriptItem *m_OriginFxScript;

public:
	static FxScriptWidget * openFxScriptPanel(FxScriptItem *fx, QWidget *parent = 0);
	static void destroyAllScriptPanels();
	static FxScriptWidget * findParentFxScriptWidget(FxItem *fx);


	bool setFxScriptContent(FxScriptItem *fxscr);


protected:
	void changeEvent(QEvent *e);

private:
	FxScriptWidget(FxScriptItem *fxscr, QWidget *parent = 0);
	~FxScriptWidget();

	bool applyChanges();

private slots:
	void on_cancelButton_clicked();
	void on_applyButton_clicked();
	void on_applyCloseButton_clicked();

signals:
	void modified();
};

#endif // FXSCRIPTWIDGET_H

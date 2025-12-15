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

#ifndef SEQUENCECONTROLWIDGET_H
#define SEQUENCECONTROLWIDGET_H

#include "ui_sequencecontrolwidget.h"
#include "system/commandsystem.h"

class AppCentral;
class FxItem;

class SequenceControlWidget : public QGroupBox, private Ui::SequenceControlWidget
{
	Q_OBJECT

private:
	AppCentral *appcentral;

public:
	SequenceControlWidget(QWidget *parent = 0);


private:
	void init();

protected:

public slots:
	void setNextFx(FxItem *fx);
	void setMasterVolume(int val);

private slots:
	void on_ctrlPlayButton_clicked();
	void on_ctrlStopButton_clicked();
	void on_masterVolDial_sliderMoved(int position);
	void on_moveToNextButton_clicked();
	void on_moveToPrevButton_clicked();

	void selectedToggled(bool state);

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);

};

#endif // SEQUENCECONTROLWIDGET_H

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

#include "sequencecontrolwidget.h"

#include "appcentral.h"
#include "fxitem.h"
#include "config.h"
#include "fxcontrol.h"
#include "audiocontrol.h"

#include <QKeyEvent>

SequenceControlWidget::SequenceControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
	masterVolDial->setRange(0,MAX_VOLUME);
	masterVolDial->setValue(MAX_VOLUME);
	nextSequenceGroup->setAutoFillBackground(true);

	appcentral = AppCentral::instance();
}

void SequenceControlWidget::setNextFx(FxItem *fx)
{
	if (fx == 0) {
		nextInSeqLabel->clear();
	} else {
		nextInSeqLabel->setText(fx->name());
	}
}

void SequenceControlWidget::setMasterVolume(int val)
{
	if (val < 0)
		val = 0;
	if (val > MAX_VOLUME)
		val = MAX_VOLUME;

	masterVolDial->setValue(val);
}

void SequenceControlWidget::init()
{
}

void SequenceControlWidget::on_ctrlPlayButton_clicked()
{
	appcentral->executeNextFx(1);
}

void SequenceControlWidget::on_ctrlStopButton_clicked()
{
	appcentral->unitFx->stopAllFxPlaylists();
	appcentral->unitFx->stopAllFxSequences();
	appcentral->unitFx->stopAllFxScripts();
	appcentral->unitAudio->stopAllFxAudio();
	appcentral->clearCurrentFx(1);
}

void SequenceControlWidget::on_masterVolDial_sliderMoved(int position)
{
	appcentral->unitAudio->setMasterVolume(position);
}


void SequenceControlWidget::on_moveToNextButton_clicked()
{
	appcentral->moveToFollowerFx(1);
}

void SequenceControlWidget::on_moveToPrevButton_clicked()
{
	appcentral->moveToForeRunnerFx(1);
}

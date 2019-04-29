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

#ifndef AUDIOCONTROLWIDGET_H
#define AUDIOCONTROLWIDGET_H

#include "../system/commandsystem.h"

#include "ui_audiocontrolwidget.h"

#include <QList>

using namespace AUDIO;

class AudioSlot;
class AppCentral;
class AudioSlotWidget;
class FrqSpectrum;

class AudioControlWidget : public QGroupBox, private Ui::AudioControlWidget
{
	Q_OBJECT
private:
	AppCentral *appcentral;

public:
	QList<AudioSlotWidget*> audioSlotWidgets;

public:
	AudioControlWidget(QWidget *parent = 0);
	void setFFTGraphVisibleFromMask(qint32 mask);
	void setVolumeDialVisibleFromMask(qint32 mask);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	void init();
	void init_gui();

public slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void audioCtrlRepeater(AudioCtrlMsg msg);
	void setVuMeterLevel(int channel, qreal left, qreal right);
	void setFFTSpectrum(int channel, FrqSpectrum *spectrum);

signals:
	void playClicked(int slotNum);
	void stopClicked(int slotNum);
	void volumeChanged(int slotNum, int volume);
	void audioCtrlCmdEmitted(AudioCtrlMsg msg);

};

#endif // AUDIOCONTROLWIDGET_H

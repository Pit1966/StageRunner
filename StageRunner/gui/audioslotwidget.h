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

#ifndef AUDIOSLOTWIDGET_H
#define AUDIOSLOTWIDGET_H

#include "ui_audioslotwidget.h"
#include "commandsystem.h"
#include "frqspectrum.h"

//using namespace AUDIO;

class AudioControlWidget;

class AudioSlotWidget : public QGroupBox, private Ui::AudioSlotWidget
{
	Q_OBJECT
public:
	int slotNumber;
	bool isAbsoluteTime;

private:
	bool m_volumeDialPressed;
	bool m_panDialPressed;
	bool m_currentPlayState;
	bool m_currentPauseState;

public:
	AudioSlotWidget(QWidget *parent = 0);
	AudioSlotWidget(AudioControlWidget *widget);
	void setFFTGraphVisible(bool state);
	void setVolumeDialVisible(bool state);
	bool setGuiSetting(const QString &key, const QString &val);
	QString guiSetting(const QString &key);
	QString allGuiSettings();
	bool setAllGuiSettings(const QString &settings);


protected:
	void resizeEvent(QResizeEvent *event);

private:
	void init_vars();
	void init_gui();

private slots:
	void on_slotPlayButton_clicked();
	void on_slotStopButton_clicked();
	void onVolumeChangedInGUI(int position);
	void onPanningChangedInGUI(int pan);
	void on_slotVolumeDial_doubleClicked();
	void on_slotAbsButton_clicked(bool checked);
	void on_slotPauseButton_clicked();
	void onMeterSliderMoved(float valf);
	void onVolumeDialMoved(int val);
	void onPanDialMoved(int val);
	void on_slotVolumeDial_sliderPressed();
	void on_slotVolumeDial_sliderReleased();
	void on_panDial_sliderPressed();
	void on_panDial_sliderReleased();

public slots:
	void setPlayState(bool state);
	void setPauseState(bool state);
	void updateGuiStatus(AUDIO::AudioCtrlMsg msg);
	void setVuLevel(qreal left, qreal right);
	void setFFTSpectrum(FrqSpectrum *spectrum);

signals:
	void playClicked(int slotNum);
	void stopClicked(int slotNum);
	void pauseClicked(int slotNum);
	void volumeChanged(int slotNum, int vol);
	void panningChanged(int slotNum, int pan);
	void audioCtrlCmdEmitted(AUDIO::AudioCtrlMsg msg);
};

#endif // AUDIOSLOTWIDGET_H

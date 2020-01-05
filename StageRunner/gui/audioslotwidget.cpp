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

#include "audioslotwidget.h"
#include "audiocontrolwidget.h"
#include "config.h"
#include "fxaudioitem.h"
#include "executer.h"

#include <QDebug>
#include <QFontDatabase>

AudioSlotWidget::AudioSlotWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init_vars();

	setupUi(this);
	init_gui();

	QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	setFont(fixedFont);
}

AudioSlotWidget::AudioSlotWidget(AudioControlWidget *widget)
{
	init_vars();
	setupUi(this);
	init_gui();

	// We don't really need this connects (it is done via controlCommand)
	connect(this,SIGNAL(playClicked(int)),widget,SIGNAL(playClicked(int)));
	connect(this,SIGNAL(stopClicked(int)),widget,SIGNAL(stopClicked(int)));
	connect(this,SIGNAL(volumeChanged(int,int)),widget,SIGNAL(volumeChanged(int,int)));

}

void AudioSlotWidget::setFFTGraphVisible(bool state)
{
	fftFrame->setVisible(state);
}

void AudioSlotWidget::setVolumeDialVisible(bool state)
{
	slotVolumeDial->setVisible(state);
}

void AudioSlotWidget::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	// qDebug("Audio Meter width: %d",meterWidget->width());

}

void AudioSlotWidget::init_vars()
{
	slotNumber = -1;
	isAbsoluteTime = false;
	m_volumeDialPressed = false;
	m_currentPauseState = false;
	m_currentPlayState = false;
}

void AudioSlotWidget::init_gui()
{
	mainLayout->setAlignment(Qt::AlignCenter);
	slotVolumeDial->setRange(0,MAX_VOLUME);
	slotVolumeDial->setNotchesVisible(true);
	slotVolumeDial->setNotchTarget(18);

	connect(meterWidget,SIGNAL(valueChanged(float)),this,SLOT(if_meter_volume_changed(float)));
	connect(slotVolumeDial,SIGNAL(valueChanged(int)),this,SLOT(if_volume_knob_changed(int)));
	connect(meterWidget,SIGNAL(sliderPressed()),this,SLOT(on_slotVolumeDial_sliderPressed()));
	connect(meterWidget,SIGNAL(sliderReleased()),this,SLOT(on_slotVolumeDial_sliderReleased()));
}

void AudioSlotWidget::on_slotPlayButton_clicked()
{
	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_AUDIO_START;
	msg.slotNumber = slotNumber;
	emit audioCtrlCmdEmitted(msg);
	emit playClicked(slotNumber);
}

void AudioSlotWidget::on_slotStopButton_clicked()
{
	AudioCtrlMsg msg;

	msg.ctrlCmd = CMD_AUDIO_STOP;
	msg.slotNumber = slotNumber;
	emit audioCtrlCmdEmitted(msg);
	emit stopClicked(slotNumber);

}

void AudioSlotWidget::on_slotAbsButton_clicked(bool checked)
{
	isAbsoluteTime = checked;

}

void AudioSlotWidget::on_slotPauseButton_clicked()
{
	AudioCtrlMsg msg;

	msg.ctrlCmd = CMD_AUDIO_PAUSE;
	msg.slotNumber = slotNumber;
	emit audioCtrlCmdEmitted(msg);
	emit pauseClicked(slotNumber);
}

void AudioSlotWidget::if_meter_volume_changed(float valf)
{
	slotVolumeDial->setValue(valf * slotVolumeDial->maximum());
	on_slotVolumeDial_sliderMoved(slotVolumeDial->sliderPosition());
}

void AudioSlotWidget::if_volume_knob_changed(int val)
{
	meterWidget->setVolume(float(val) / float(slotVolumeDial->maximum()));
}

void AudioSlotWidget::on_slotVolumeDial_sliderMoved(int position)
{
	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_AUDIO_CHANGE_VOL;
	msg.slotNumber = slotNumber;
	msg.volume = position;
	msg.isActive = m_volumeDialPressed;
	emit audioCtrlCmdEmitted(msg);
	emit volumeChanged(slotNumber,position);
}

void AudioSlotWidget::on_slotVolumeDial_doubleClicked()
{
	AudioCtrlMsg msg;

	msg.ctrlCmd = CMD_AUDIO_FADEOUT;
	msg.slotNumber = slotNumber;
	msg.fadetime = 5000;
	emit audioCtrlCmdEmitted(msg);
}

void AudioSlotWidget::setPlayState(bool state)
{
	if (state == m_currentPlayState)
		return;

	if (state) {
		slotPlayButton->setIcon(QIcon(":/gfx/icons/audio_play_green.png"));
	} else {
		slotPlayButton->setIcon(QIcon(":/gfx/icons/audio_play.png"));
	}

	m_currentPlayState = state;
}

void AudioSlotWidget::setPauseState(bool state)
{
	if (state == m_currentPauseState)
		return;

	if (state) {
		slotPauseButton->setIcon(QIcon(":/gfx/icons/audio_pause_red.png"));
	} else {
		slotPauseButton->setIcon(QIcon(":/gfx/icons/audio_pause.png"));
	}

	m_currentPauseState = state;
}

void AudioSlotWidget::updateGuiStatus(AudioCtrlMsg msg)
{
	// qDebug() << "AudioSlotWidget: msg:" << msg.ctrlCmd << msg.currentAudioStatus;
	if (msg.ctrlCmd == CMD_STATUS_REPORT || msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED) {
		// Set Play-Status Buttons in Audio Slot Panel
		switch (msg.currentAudioStatus) {
		case AUDIO_NO_FREE_SLOT:
		case AUDIO_NO_STATE:
			break;
		case AUDIO_IDLE:
		case AUDIO_ERROR:
			setPlayState(false);
			setPauseState(false);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				setTitle(msg.fxAudio->name().left(7) + "...");
			}
			break;
		case AUDIO_PAUSED:
			setPlayState(false);
			setPauseState(true);
			break;
		default:
			setPlayState(true);
			setPauseState(false);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				// setTitle(msg.fxAudio->name());
				slotPlayButton->setToolTip(msg.fxAudio->name());
				slotStopButton->setToolTip(msg.fxAudio->name());
			}
			if (msg.progress >= 0) {
				QString time;
				int min = msg.progressTime / 60000;
				int sec = ( msg.progressTime - (min * 60000) ) / 1000;
				int ms = (( msg.progressTime - (min * 60000)) % 1000) / 100;
				time = QString("%1m%2.%3s")
						.arg(min, 1, 10, QLatin1Char('0'))
						.arg(sec, 2, 10, QLatin1Char('0'))
						.arg(ms, 1, 10, QLatin1Char('0'));
				slotAbsButton->setToolTip(time);
				if (isAbsoluteTime) {
					setTitle(time);
				} else {
					time = QString("%1.%2")
							.arg(msg.progress/10, 3, 10, QLatin1Char('0'))
							.arg(msg.progress%10, 1, 10, QLatin1Char('0'));
					if (msg.maxloop > 0) {
						time += QString(" L%1/%2").arg(msg.loop).arg(msg.maxloop);
					}
					setTitle(time);
				}
			}
			break;
		}
		if (msg.volume >= 0)
			slotVolumeDial->setValue(msg.volume);
	}
	else if (msg.ctrlCmd == CMD_VIDEO_STATUS_CHANGED) {
		// Set Play-Status Buttons in Audio Slot Panel
		switch (msg.currentAudioStatus) {
		case VIDEO_IDLE:
			setPlayState(false);
			setPauseState(false);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				setTitle(msg.fxAudio->name().left(7) + "...");
			}
			break;
		case VIDEO_RUNNING:
			setPlayState(true);
			setPauseState(false);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				// setTitle(msg.fxAudio->name());
				slotPlayButton->setToolTip(msg.fxAudio->name());
				slotStopButton->setToolTip(msg.fxAudio->name());
			}
			if (msg.progress >= 0) {
				QString time;
				int min = msg.progressTime / 60000;
				int sec = ( msg.progressTime - (min * 60000) ) / 1000;
				int ms = (( msg.progressTime - (min * 60000)) % 1000) / 100;
				time = QString("%1m%2.%3s")
						.arg(min, 1, 10, QLatin1Char('0'))
						.arg(sec, 2, 10, QLatin1Char('0'))
						.arg(ms, 1, 10, QLatin1Char('0'));
				slotAbsButton->setToolTip(time);
				if (isAbsoluteTime) {
					setTitle(time);
				} else {
					time = QString("%1.%2")
							.arg(msg.progress/10, 3, 10, QLatin1Char('0'))
							.arg(msg.progress%10, 1, 10, QLatin1Char('0'));
					if (msg.maxloop > 0) {
						time += QString(" L%1/%2").arg(msg.loop).arg(msg.maxloop);
					}
					setTitle(time);
				}
			}
			else if (msg.fxAudio) {
				setTitle(msg.fxAudio->name().left(7) + "...");
			}
			break;
		default:
			break;
		}
		if (msg.volume >= 0)
			slotVolumeDial->setValue(msg.volume);

	}
}

void AudioSlotWidget::setVuLevel(qreal left, qreal right)
{
	meterWidget->setValue(0,left);
	meterWidget->setValue(1,right);
	if (left == 0 && right == 0) {
		for (int t=0;t<5;t++) {
			meterWidget->setValue(0,0.001f);
			meterWidget->setValue(1,0.001f);
			meterWidget->peakReset();
		}
	}
}

void AudioSlotWidget::setFFTSpectrum(FrqSpectrum *spectrum)
{
	fftWidget->setFrqSpectrum(spectrum);
}



void AudioSlotWidget::on_slotVolumeDial_sliderPressed()
{
	m_volumeDialPressed = true;
}

void AudioSlotWidget::on_slotVolumeDial_sliderReleased()
{
	m_volumeDialPressed = false;
}

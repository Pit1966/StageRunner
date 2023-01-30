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

#include "audiocontrolwidget.h"
#include "system/log.h"
#include "config.h"
#include "gui/audioslotwidget.h"
#include "appcontrol/appcentral.h"
#include "system/qt_versions.h"


#include <QHBoxLayout>
#include <QDebug>
#include <QFontDatabase>

AudioControlWidget::AudioControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
	init_gui();

	statusLabel->setText("");
}

void AudioControlWidget::setFFTGraphVisibleFromMask(qint32 mask)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (mask & (1<<t)) {
			audioSlotWidgets.at(t)->setFFTGraphVisible(true);
		} else {
			audioSlotWidgets.at(t)->setFFTGraphVisible(false);
		}
	}
}

void AudioControlWidget::setVolumeDialVisibleFromMask(qint32 mask)
{
	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		if (mask & (1<<t)) {
			audioSlotWidgets.at(t)->setVolumeDialVisible(true);
		} else {
			audioSlotWidgets.at(t)->setVolumeDialVisible(false);
		}
	}
}

void AudioControlWidget::setGuiSettingForSlot(int slotNum, const QString &key, const QString &val)
{
	if (slotNum < audioSlotWidgets.size() && slotNum >= 0) {
		audioSlotWidgets.at(slotNum)->setGuiSetting(key, val);
	}
}

QString AudioControlWidget::guiSettingForSlot(int slotNum, const QString &key)
{
	if (slotNum < audioSlotWidgets.size() && slotNum >= 0) {
		return audioSlotWidgets.at(slotNum)->guiSetting(key);
	}
	return QString();
}

QString AudioControlWidget::completeGuiSettings()
{
	QString set;
	for (int s=0; s<audioSlotWidgets.size(); s++) {
		set += "[" + QString::number(s+1) + ":" + audioSlotWidgets.at(s)->allGuiSettings() + "]";
	}
	return set;
}

bool AudioControlWidget::setCompleteGuiSettings(const QString &settings)
{
	bool ok = true;
	const QStringList slotsets = settings.split(']', QT_SKIP_EMPTY_PARTS);
	for (QString slotset : slotsets) {
		slotset.remove('[');
		int slot = slotset.section(':',0,0).toInt() - 1;
		QString set = slotset.section(':', 1);
		if (slot >= 0 && slot < audioSlotWidgets.size()) {
			ok &= audioSlotWidgets.at(slot)->setAllGuiSettings(set);
		}
	}

	return ok;
}

void AudioControlWidget::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
}

void AudioControlWidget::init()
{
	appcentral = AppCentral::instance();
}

void AudioControlWidget::init_gui()
{

	QHBoxLayout *layout = new QHBoxLayout;

	for (int t=0; t<MAX_AUDIO_SLOTS; t++) {
		AudioSlotWidget *slot = new AudioSlotWidget;
		slot->slotNumber = t;
		slot->setTitle(tr("Slot %1").arg(t+1));

		connect(slot,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));

		audioSlotWidgets.append(slot);
		layout->addWidget(slot);
	}

	mainLayout->insertLayout(0,layout);
}

void AudioControlWidget::audioCtrlReceiver(AudioCtrlMsg msg)
{
	if (debug > 5) DEBUGTEXT("%s: Cmd:%d, AudioStatus:%d, Volume:%d, Progress:%d ProgressTime: %d"
							 ,Q_FUNC_INFO,msg.ctrlCmd,msg.currentAudioStatus,msg.volume,msg.progress,msg.progressTime);
	if (msg.slotNumber < 0 || msg.slotNumber >= MAX_AUDIO_SLOTS) {
		DEBUGERROR("AudioControlWidget::audioCtrlReceiver: illegal slot number received: %d", msg.slotNumber);
		return;
	}

	switch (msg.ctrlCmd) {
	case CMD_AUDIO_STATUS_CHANGED:
	case CMD_STATUS_REPORT:
	case CMD_VIDEO_STATUS_CHANGED:
		audioSlotWidgets[msg.slotNumber]->updateGuiStatus(msg);
		break;
	default:
		break;

	}
}

void AudioControlWidget::audioCtrlRepeater(AudioCtrlMsg msg)
{
	emit audioCtrlCmdEmitted(msg);
}

void AudioControlWidget::setVuMeterLevel(int channel, qreal left, qreal right)
{
	if (channel >= 0 && channel < audioSlotWidgets.size()) {
		audioSlotWidgets[channel]->setVuLevel(left,right);
	}
}

void AudioControlWidget::setFFTSpectrum(int channel, FrqSpectrum *spectrum)
{
	if (channel >= 0 && channel < audioSlotWidgets.size()) {
		audioSlotWidgets[channel]->setFFTSpectrum(spectrum);
	}
}

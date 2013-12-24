#include "audioslotwidget.h"
#include "audiocontrolwidget.h"
#include "config.h"
#include "fxaudioitem.h"
#include "executer.h"

#include <QDebug>

AudioSlotWidget::AudioSlotWidget(QWidget *parent) :
	QGroupBox(parent)
{
	slotNumber = -1;

	setupUi(this);
	init_gui();
}

AudioSlotWidget::AudioSlotWidget(AudioControlWidget *widget)
{
	slotNumber = -1;

	setupUi(this);
	init_gui();

	// We don't really need this connects (it is done via controlCommand)
	connect(this,SIGNAL(playClicked(int)),widget,SIGNAL(playClicked(int)));
	connect(this,SIGNAL(stopClicked(int)),widget,SIGNAL(stopClicked(int)));
	connect(this,SIGNAL(volumeChanged(int,int)),widget,SIGNAL(volumeChanged(int,int)));

}

void AudioSlotWidget::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	// qDebug("Audio Meter width: %d",meterWidget->width());

}

void AudioSlotWidget::init_gui()
{
	mainLayout->setAlignment(Qt::AlignCenter);
	slotVolumeDial->setRange(0,MAX_VOLUME);
	slotVolumeDial->setNotchesVisible(true);
	slotVolumeDial->setNotchTarget(18);

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

void AudioSlotWidget::on_slotVolumeDial_sliderMoved(int position)
{
	AudioCtrlMsg msg;
	msg.ctrlCmd = CMD_AUDIO_CHANGE_VOL;
	msg.slotNumber = slotNumber;
	msg.volume = position;
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
	if (state) {
		slotPlayButton->setIcon(QIcon(":/gfx/icons/audio_play_green.png"));
	} else {
		slotPlayButton->setIcon(QIcon(":/gfx/icons/audio_play.png"));
	}
}

void AudioSlotWidget::updateGuiStatus(AudioCtrlMsg msg)
{
	// qDebug() << "AudioSlotWidget: msg:" << msg.ctrlCmd << msg.currentAudioStatus;
	if (msg.ctrlCmd == CMD_STATUS_REPORT || msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED) {
		// Set Play-Status Buttons in Audio Slot Panel
		switch (msg.currentAudioStatus) {
		case AUDIO_IDLE:
		case AUDIO_ERROR:
			setPlayState(false);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				setTitle(msg.fxAudio->name().left(5) + "...");
			}
			break;
		default:
			setPlayState(true);
			if (msg.fxAudio && FxItem::exists(msg.fxAudio)) {
				// setTitle(msg.fxAudio->name());
				slotPlayButton->setToolTip(msg.fxAudio->name());
				slotStopButton->setToolTip(msg.fxAudio->name());
			}
			if (msg.progress >= 0) {
				QString time = QString("%1.%2\%")
						.arg(msg.progress/10, 3, 10, QLatin1Char('0'))
						.arg(msg.progress%10, 1, 10, QLatin1Char('0'));
				if (msg.maxloop > 0) {
					time += QString(" L%1/%2").arg(msg.loop+1).arg(msg.maxloop);
				}
				setTitle(time);
			}
			break;
		}
		if (msg.volume >= 0) slotVolumeDial->setValue(msg.volume);
	}
}

void AudioSlotWidget::setVuLevel(int left, int right)
{
	meterWidget->setValue(0,(float)left/10000);
	meterWidget->setValue(1,(float)right/10000);
	if (left == 0 && right == 0) {
		for (int t=0;t<5;t++) {
			meterWidget->setValue(0,0.001f);
			meterWidget->setValue(1,0.001f);
			meterWidget->peakReset();
		}
	}
}

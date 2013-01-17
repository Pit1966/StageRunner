#include "audioslotwidget.h"
#include "audiocontrolwidget.h"

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
	slotVolumeDial->setRange(0,160);
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
	switch (msg.currentAudioStatus) {
	case AUDIO_IDLE:
	case AUDIO_ERROR:
		setPlayState(false);
		break;
	default:
		setPlayState(true);
		break;
	}
}

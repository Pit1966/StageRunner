#include "audiocontrolwidget.h"
#include "system/log.h"
#include "config.h"
#include "gui/audioslotwidget.h"
#include "appcontrol/appcentral.h"

#include <QHBoxLayout>
#include <QDebug>

AudioControlWidget::AudioControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
	init_gui();
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

		connect(slot,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),this,SLOT(audioCtrlRepeater(AudioCtrlMsg)));

		audioSlotWidgets.append(slot);
		layout->addWidget(slot);
	}

	mainLayout->insertLayout(0,layout);
}

void AudioControlWidget::audioCtrlReceiver(AudioCtrlMsg msg)
{
	if (debug > 5) DEBUGTEXT("%s: Cmd:%d, AudioStatus:%d, Volume:%d, Progress:%d"
							 ,Q_FUNC_INFO,msg.ctrlCmd,msg.currentAudioStatus,msg.volume,msg.progress);
	if (msg.slotNumber < 0 || msg.slotNumber >= MAX_AUDIO_SLOTS) {
		DEBUGERROR("AudioControlWidget::audioCtrlReceiver: illegal slot number received: %d", msg.slotNumber);
		return;
	}

	switch (msg.ctrlCmd) {
	case CMD_STATUS_REPORT:
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

void AudioControlWidget::setVuMeterLevel(int channel, int left, int right)
{
	if (channel >= 0 && channel < audioSlotWidgets.size()) {
		audioSlotWidgets[channel]->setVuLevel(left,right);
	}
}

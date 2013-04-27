#include "sequencecontrolwidget.h"

#include "appcontrol/appcentral.h"
#include "fx/fxitem.h"
#include "config.h"
#include "system/audiocontrol.h"

#include <QKeyEvent>

SequenceControlWidget::SequenceControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
	masterVolDial->setRange(0,MAX_VOLUME);
	masterVolDial->setValue(MAX_VOLUME);

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

void SequenceControlWidget::init()
{
}

void SequenceControlWidget::on_ctrlPlayButton_clicked()
{
	appcentral->executeNextFx(1);
}

void SequenceControlWidget::on_ctrlStopButton_clicked()
{
	appcentral->stopAllFxAudio();
}

void SequenceControlWidget::on_masterVolDial_sliderMoved(int position)
{
	appcentral->unitAudio->setMasterVolume(position);
}

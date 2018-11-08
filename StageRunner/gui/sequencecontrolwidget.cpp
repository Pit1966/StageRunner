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

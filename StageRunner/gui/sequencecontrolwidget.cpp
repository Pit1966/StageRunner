#include "sequencecontrolwidget.h"

#include "main/appcentral.h"

SequenceControlWidget::SequenceControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	setupUi(this);

	appcentral = AppCentral::instance();
}

void SequenceControlWidget::on_ctrlPlayButton_clicked()
{

}

void SequenceControlWidget::on_ctrlStopButton_clicked()
{
	appcentral->stopAllFxAudio();
}

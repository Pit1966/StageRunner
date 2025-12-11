#include "dmxrangewidget.h"
#include "ui_dmxrangewidget.h"

DmxRangeWidget::DmxRangeWidget(int first, int last, int universe, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DmxRangeWidget)
{
	ui->setupUi(this);
	ui->firstDmxChanSpin->setValue(first + 1);
	ui->lastDmxChanSpin->setValue(last + 1);
	ui->universeSpin->setValue(universe + 1);
}

DmxRangeWidget::~DmxRangeWidget()
{
	delete ui;
}

int DmxRangeWidget::first() const
{
	return ui->firstDmxChanSpin->value()-1;
}

int DmxRangeWidget::last() const
{
	return ui->lastDmxChanSpin->value()-1;
}

int DmxRangeWidget::universe() const
{
	return ui->universeSpin->value()-1;
}

void DmxRangeWidget::on_firstDmxChanSpin_valueChanged(int arg1)
{
	int to = ui->lastDmxChanSpin->value();
	if (arg1 > to) {
		ui->lastDmxChanSpin->setValue(arg1);
		return;
	}

	ui->channelsSpin->setValue(to - arg1 + 1);
}

void DmxRangeWidget::on_lastDmxChanSpin_valueChanged(int arg1)
{
	int from = ui->firstDmxChanSpin->value();
	if (arg1 < from) {
		ui->lastDmxChanSpin->setValue(from);
		return;
	}
	ui->channelsSpin->setValue(arg1 - from + 1);
}

void DmxRangeWidget::on_channelsSpin_valueChanged(int arg1)
{
	int from = ui->firstDmxChanSpin->value();
	ui->lastDmxChanSpin->setValue(from + arg1 - 1);
}






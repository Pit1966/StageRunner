#include "../system/dmx/dmxrangewidget.h"
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

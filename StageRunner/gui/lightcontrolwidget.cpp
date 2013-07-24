#include "lightcontrolwidget.h"
#include "appcentral.h"

LightControlWidget::LightControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
}

void LightControlWidget::init()
{
	appcentral = AppCentral::instance();
}

void LightControlWidget::on_blackButton_clicked()
{
	appcentral->lightBlack(0);

}

void LightControlWidget::on_fadeToBlackButton_clicked()
{
	appcentral->lightBlack(fadeSecondsSpin->value() * 1000);
}

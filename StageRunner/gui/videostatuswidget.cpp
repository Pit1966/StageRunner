#include "videostatuswidget.h"
#include "ui_videostatuswidget.h"

VideoStatusWidget::VideoStatusWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VideoStatusWidget)
{
	ui->setupUi(this);
}

VideoStatusWidget::~VideoStatusWidget()
{
	delete ui;
}

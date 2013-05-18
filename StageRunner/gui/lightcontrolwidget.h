#ifndef LIGHTCONTROLWIDGET_H
#define LIGHTCONTROLWIDGET_H

#include "ui_lightcontrolwidget.h"

class AppCentral;

class LightControlWidget : public QGroupBox, private Ui::LightControlWidget
{
	Q_OBJECT
private:
	AppCentral *appcentral;

public:
	LightControlWidget(QWidget *parent = 0);

private slots:
	void on_blackButton_clicked();

private:
	void init();

};

#endif // LIGHTCONTROLWIDGET_H

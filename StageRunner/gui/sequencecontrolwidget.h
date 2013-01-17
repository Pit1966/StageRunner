#ifndef SEQUENCECONTROLWIDGET_H
#define SEQUENCECONTROLWIDGET_H

#include "ui_sequencecontrolwidget.h"

class AppCentral;

class SequenceControlWidget : public QGroupBox, private Ui::SequenceControlWidget
{
	Q_OBJECT

private:
	AppCentral *appcentral;

public:
	explicit SequenceControlWidget(QWidget *parent = 0);
private slots:
	void on_ctrlPlayButton_clicked();
	void on_ctrlStopButton_clicked();
};

#endif // SEQUENCECONTROLWIDGET_H

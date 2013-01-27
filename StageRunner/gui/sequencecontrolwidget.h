#ifndef SEQUENCECONTROLWIDGET_H
#define SEQUENCECONTROLWIDGET_H

#include "ui_sequencecontrolwidget.h"
#include "system/commandsystem.h"

class AppCentral;
class FxItem;

class SequenceControlWidget : public QGroupBox, private Ui::SequenceControlWidget
{
	Q_OBJECT

private:
	AppCentral *appcentral;

public:
	SequenceControlWidget(QWidget *parent = 0);


private:
	void init();

public slots:
	void setNextFx(FxItem *fx);

private slots:
	void on_ctrlPlayButton_clicked();
	void on_ctrlStopButton_clicked();

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);

};

#endif // SEQUENCECONTROLWIDGET_H

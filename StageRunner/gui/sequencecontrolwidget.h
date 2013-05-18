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
	void on_masterVolDial_sliderMoved(int position);
	void on_moveToNextButton_clicked();
	void on_moveToPrevButton_clicked();

signals:
	void fxCmdActivated(FxItem *, CtrlCmd);

};

#endif // SEQUENCECONTROLWIDGET_H

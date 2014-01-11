#ifndef AUDIOSLOTWIDGET_H
#define AUDIOSLOTWIDGET_H

#include "ui_audioslotwidget.h"
#include "commandsystem.h"

using namespace AUDIO;

class AudioControlWidget;

class AudioSlotWidget : public QGroupBox, private Ui::AudioSlotWidget
{
	Q_OBJECT
public:
	int slotNumber;
	bool isAbsoluteTime;

public:
	AudioSlotWidget(QWidget *parent = 0);
	AudioSlotWidget(AudioControlWidget *widget);


protected:
	void resizeEvent(QResizeEvent *event);

private:
	void init_gui();

private slots:
	void on_slotPlayButton_clicked();
	void on_slotStopButton_clicked();
	void on_slotVolumeDial_sliderMoved(int position);
	void on_slotVolumeDial_doubleClicked();

	void on_slotAbsButton_clicked(bool checked);

	void on_slotPauseButton_clicked();

public slots:
	void setPlayState(bool state);
	void updateGuiStatus(AudioCtrlMsg msg);
	void setVuLevel(int left, int right);

signals:
	void playClicked(int slotNum);
	void stopClicked(int slotNum);
	void volumeChanged(int slotNum, int vol);
	void audioCtrlCmdEmitted(AudioCtrlMsg msg);
};

#endif // AUDIOSLOTWIDGET_H

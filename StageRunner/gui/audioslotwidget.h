#ifndef AUDIOSLOTWIDGET_H
#define AUDIOSLOTWIDGET_H

#include "ui_audioslotwidget.h"
#include "commandsystem.h"
#include "frqspectrum.h"

using namespace AUDIO;

class AudioControlWidget;

class AudioSlotWidget : public QGroupBox, private Ui::AudioSlotWidget
{
	Q_OBJECT
public:
	int slotNumber;
	bool isAbsoluteTime;

private:
	bool m_volumeDialPressed;

public:
	AudioSlotWidget(QWidget *parent = 0);
	AudioSlotWidget(AudioControlWidget *widget);
	void setFFTGraphVisible(bool state);
	void setVolumeDialVisible(bool state);


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
	void if_meter_volume_changed(float valf);
	void if_volume_knob_changed(int val);
	void on_slotVolumeDial_sliderPressed();
	void on_slotVolumeDial_sliderReleased();

public slots:
	void setPlayState(bool state);
	void updateGuiStatus(AudioCtrlMsg msg);
	void setVuLevel(qreal left, qreal right);
	void setFFTSpectrum(FrqSpectrum *spectrum);

signals:
	void playClicked(int slotNum);
	void stopClicked(int slotNum);
	void volumeChanged(int slotNum, int vol);
	void audioCtrlCmdEmitted(AudioCtrlMsg msg);
};

#endif // AUDIOSLOTWIDGET_H

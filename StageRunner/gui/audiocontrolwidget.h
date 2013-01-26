#ifndef AUDIOCONTROLWIDGET_H
#define AUDIOCONTROLWIDGET_H

#include "../system/commandsystem.h"

#include "ui_audiocontrolwidget.h"

#include <QList>

using namespace AUDIO;

class AudioSlot;
class AppCentral;
class AudioSlotWidget;

class AudioControlWidget : public QGroupBox, private Ui::AudioControlWidget
{
	Q_OBJECT
private:
	AppCentral *appcentral;

public:
	QList<AudioSlotWidget*> audioSlotWidgets;

public:
	explicit AudioControlWidget(QWidget *parent = 0);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	void init();
	void init_gui();

public slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void audioCtrlRepeater(AudioCtrlMsg msg);
	void setVuMeterLevel(int channel, int left, int right);

signals:
	void playClicked(int slotNum);
	void stopClicked(int slotNum);
	void volumeChanged(int slotNum, int volume);
	void audioCtrlCmdEmitted(AudioCtrlMsg msg);

};

#endif // AUDIOCONTROLWIDGET_H

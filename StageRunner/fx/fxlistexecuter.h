#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"

using namespace AUDIO;

#include <QObject>

class FxList;
class AppCentral;
class AudioControl;
class LightControl;
class FxItem;

class FxListExecuter : public QObject
{
	Q_OBJECT
protected:
	AppCentral * myApp;
	FxList *fxList;
	AudioControl *unitAudio;
	LightControl *unitLight;

	FxItem *curFx;

private:
	AudioStatus currentAudioStatus;

public:
	FxListExecuter(AppCentral * app_central, FxList *fx_list = 0);
	void setFxList(FxList *fx_list);
	bool runExecuter(int idx = -1);
	bool runFxItem(FxItem *fx);

protected slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);

private:
	void init();



signals:

};

#endif // FXLISTEXECUTER_H

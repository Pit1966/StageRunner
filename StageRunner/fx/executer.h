#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"

using namespace AUDIO;

#include <QObject>
#include <QString>

class FxList;
class AppCentral;
class AudioControl;
class LightControl;
class FxItem;

class Executer : public QObject
{
	Q_OBJECT
protected:
	AppCentral * myApp;
	QString idString;

protected:
	Executer(AppCentral * app_central);

public:
	void setIdString(const QString & str);
	QString getIdString() const;
};



class FxListExecuter : public Executer
{
	Q_OBJECT
protected:
	FxList *fxList;
	AudioControl *unitAudio;
	LightControl *unitLight;

	FxItem *curFx;

private:
	AudioStatus currentAudioStatus;

	int playlist_initial_vol;

public:
	void setFxList(FxList *fx_list);
	void setPlayListInitialVolume(int vol);
	bool runExecuter(int idx = -1);
	bool runFxItem(FxItem *fx);

protected:
	FxListExecuter(AppCentral * app_central, FxList *fx_list);


protected slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);

private:
	void init();



signals:
	void fxItemExecuted(FxItem *fx, Executer *exec);
	void deleteMe();

	friend class ExecCenter;

};

#endif // FXLISTEXECUTER_H

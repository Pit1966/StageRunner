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
	FxItem *originFxItem;						///<  This should be the FxItem that has initiated the executer or 0
	FxItem *curFx;
	FxItem *nxtFx;
	QString idString;

protected:
	Executer(AppCentral * app_central);
	~Executer();

public:
	void setIdString(const QString & str);
	QString getIdString() const;
	inline void setOriginFx(FxItem *fx) {originFxItem = fx;}
	inline FxItem * originFx() const {return originFxItem;}
	inline FxItem * currentFx() const {return curFx;}
	inline FxItem * nextFx() const {return nxtFx;}

	friend class ExecCenter;
};



class FxListExecuter : public Executer
{
	Q_OBJECT
protected:
	FxList *fxList;
	AudioControl *unitAudio;
	LightControl *unitLight;


private:
	AudioStatus currentAudioStatus;

	int playlist_initial_vol;

public:
	void setFxList(FxList *fx_list);
	void setPlayListInitialVolume(int vol);
	bool runExecuter(int idx = -1);
	bool runFxItem(FxItem *fx);
	void fadeEndExecuter();

protected:
	FxListExecuter(AppCentral * app_central, FxList *fx_list);


protected slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);

private:
	void init();



signals:
	void fxItemExecuted(FxItem *fx, Executer *exec);
	void deleteMe(Executer *exec);
	void currentFxChanged(FxItem *fx);
	void nextFxChanged(FxItem *fx);

	friend class ExecCenter;

};

#endif // FXLISTEXECUTER_H

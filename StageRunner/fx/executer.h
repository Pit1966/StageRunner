#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"

using namespace AUDIO;

#include <QObject>
#include <QString>
#include <QElapsedTimer>

class FxList;
class AppCentral;
class AudioControl;
class LightControl;
class FxControl;
class FxItem;
class FxSceneItem;

class Executer : public QObject
{
	Q_OBJECT
public:
	enum TYPE {
		EXEC_BASE,
		EXEC_FXLIST
	};

protected:
	AppCentral * myApp;
	FxItem *originFxItem;						///<  This should be the FxItem that has initiated the executer or 0
	FxItem *curFx;
	FxItem *nxtFx;
	QString idString;
	QElapsedTimer runTime;						///< This timer holds the overall running time of the executer
	qint64 eventTargetTimeMs;					///< This is the target time for the next event that has to be executed by the executer (if runTime < targetTimeMs nothing is todo)
	bool isInExecLoopFlag;						///< This indicates that the executer belongs to the FxExecLoop


protected:
	Executer(AppCentral * app_central);
	~Executer();

public:
	void setIdString(const QString & str);
	inline virtual TYPE type() {return EXEC_BASE;}
	virtual bool processExecuter();
	void destroyLater();
	inline void setExecLoopFlag(bool state) {isInExecLoopFlag = state;}
	inline qint64 currentTargetTimeMs() {return eventTargetTimeMs;}
	inline qint64 currentRunTimeMs() {return runTime.elapsed();}
	inline bool processTimeReached() {return (runTime.elapsed() >= eventTargetTimeMs);}
	QString getIdString() const;
	inline void setOriginFx(FxItem *fx) {originFxItem = fx;}
	inline FxItem * originFx() const {return originFxItem;}
	inline FxItem * currentFx() const {return curFx;}
	inline FxItem * nextFx() const {return nxtFx;}

signals:
	void deleteMe(Executer *exec);

	friend class ExecCenter;
};



class FxListExecuter : public Executer
{
	Q_OBJECT
protected:
	FxList *fxList;
	AudioControl *unitAudio;
	LightControl *unitLight;
	FxControl *unitFx;

private:
	AudioStatus currentAudioStatus;

	int playlist_initial_vol;

public:
	inline TYPE type() {return EXEC_FXLIST;}
	bool processExecuter();

	void setFxList(FxList *fx_list);
	void setPlayListInitialVolume(int vol);
	bool runExecuter(int idx = -1);
	void fadeEndExecuter();
	void setNextFx(FxItem *fx);

protected:
	FxListExecuter(AppCentral * app_central, FxList *fx_list);
	void setCurrentFx(FxItem *fx);

	bool runFxItem(FxItem *fx);
	bool runFxAudioItem(FxAudioItem *fxa);
	bool runFxSceneItem(FxSceneItem *fxs);


protected slots:
	void audioCtrlReceiver(AudioCtrlMsg msg);
	void lightCtrlReceiver(FxSceneItem *fxs);
	void sceneCueReceiver(FxSceneItem *fxs);

private:
	void init();
	FxItem * move_to_next_fx();
	qint64 cue_fx(FxItem *fx);
	qint64 cue_fx_scene(FxSceneItem *scene);
	qint64 cue_fx_audio(FxAudioItem *audio);


signals:
	void fxItemExecuted(FxItem *fx, Executer *exec);
	void currentFxChanged(FxItem *fx);
	void nextFxChanged(FxItem *fx);

	friend class ExecCenter;

};

#endif // FXLISTEXECUTER_H

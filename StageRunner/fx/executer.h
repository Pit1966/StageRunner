#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"

#include <QObject>
#include <QString>
#include <QElapsedTimer>

using namespace AUDIO;

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
		EXEC_FXLIST,
		EXEC_SCENE
	};
	enum STATE {
		EXEC_IDLE,
		EXEC_RUNNING,
		EXEC_PAUSED,
		EXEC_DELETED
	};

protected:
	AppCentral &myApp;

	FxItem *originFxItem;						///<  This should be the FxItem that has initiated the executer or 0
	QString idString;
	QElapsedTimer runTime;						///< This timer holds the overall running time of the executer
	qint64 eventTargetTimeMs;					///< This is the target time for the next event that has to be executed by the executer (if runTime < targetTimeMs nothing is todo)
	bool isWaitingForAudio;
	STATE myState;

private:
	int use_cnt;								///< if this usage counter is > 0 the object should not been deleted

protected:
	Executer(AppCentral &app_central);
	~Executer();

public:
	inline int useCount() const {return use_cnt;}
	void setIdString(const QString & str);
	inline virtual TYPE type() {return EXEC_BASE;}
	inline STATE state() const {return myState;}
	virtual bool processExecuter();

	void destroyLater();
	bool activateProcessing();
	bool deactivateProcessing();
	bool setPaused(bool state);
	inline bool isRunning() const {return (myState == EXEC_RUNNING);}
	inline qint64 currentTargetTimeMs() {return eventTargetTimeMs;}
	inline qint64 currentRunTimeMs() {return runTime.elapsed();}
	inline bool processTimeReached() {return (runTime.elapsed() >= eventTargetTimeMs);}
	inline void setTargetTimeToCurrent() {eventTargetTimeMs = runTime.elapsed();}
	QString getIdString() const;
	inline void setOriginFx(FxItem *fx) {originFxItem = fx;}
	inline FxItem * originFx() const {return originFxItem;}

signals:
	void deleteMe(Executer *exec);
	void changed(Executer *exec);

	friend class ExecCenter;
};



class FxListExecuter : public Executer
{
	Q_OBJECT
protected:
	FxList *fxList;
	FxItem *curFx;
	FxItem *nxtFx;

private:
	int playbackProgress;

public:
	inline TYPE type() {return EXEC_FXLIST;}
	bool processExecuter();

	void setFxList(FxList *fx_list);
	void setNextFx(FxItem *fx);
	inline FxItem * nextFx() const {return nxtFx;}
	void setCurrentFx(FxItem *fx);
	inline FxItem * currentFx() const {return curFx;}
	FxAudioItem * currentFxAudio();

protected:
	FxListExecuter(AppCentral & app_central, FxList *fx_list);
	~FxListExecuter();

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

public slots:
	void selectNextFx(FxItem *fx);

signals:
	void currentFxChanged(FxItem *fx);
	void nextFxChanged(FxItem *fx);
	void playListProgressChanged(int audioProgressMille, int playlistProgressMille);

	friend class ExecCenter;

};


class SceneExecuter : public Executer
{
protected:
	FxSceneItem *curScene;
	LIGHT::SceneSeqState sceneState;

public:
	inline TYPE type() {return EXEC_SCENE;}
	bool processExecuter();

protected:
	SceneExecuter(AppCentral &app_central, FxSceneItem *scene);

public:


	friend class ExecCenter;
};

#endif // FXLISTEXECUTER_H

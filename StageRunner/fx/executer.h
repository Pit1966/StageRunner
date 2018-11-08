#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"
#include "fx/fxscripttools.h"

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
class FxScriptItem;
class FxScriptList;

typedef QList<FxItem*> FxItemList;

class Executer : public QObject
{
	Q_OBJECT
public:
	enum TYPE {
		EXEC_BASE,
		EXEC_FXLIST,
		EXEC_SCENE,
		EXEC_SCRIPT
	};
	enum STATE {
		EXEC_IDLE,
		EXEC_RUNNING,
		EXEC_PAUSED,
		EXEC_DELETED,
		EXEC_FINISH
	};

protected:
	AppCentral &myApp;

	FxItem *originFxItem;						///< This should be the FxItem that has initiated the executer or NULL
	FxItem *parentFxItem;						///< This is the parent FxItem, that contains the originFxItem or NULL
	QString idString;
	QElapsedTimer runTime;						///< This timer holds the overall running time of the executer
	qint64 eventTargetTimeMs;					///< This is the target time for the next event that has to be executed by the executer (if runTime < targetTimeMs nothing is todo)
	bool isWaitingForAudio;
	volatile STATE myState;

private:
	int use_cnt;								///< if this usage counter is > 0 the object should not been deleted

protected:
	Executer(AppCentral &app_central, FxItem *originFx = 0);
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
	bool setFinish();
	inline bool isRunning() const {return (myState == EXEC_RUNNING || myState == EXEC_FINISH);}
	inline qint64 currentTargetTimeMs() {return eventTargetTimeMs;}
	inline qint64 currentRunTimeMs() {return runTime.elapsed();}
	inline bool processTimeReached() {return (runTime.elapsed() >= eventTargetTimeMs);}
	inline void setTargetTimeToCurrent() {eventTargetTimeMs = runTime.elapsed();}
	inline void setEventTargetTime(qint64 ms) {eventTargetTimeMs = runTime.elapsed() + ms;}
	QString getIdString() const;
	inline void setOriginFx(FxItem *fx) {originFxItem = fx;}
	inline FxItem * originFx() const {return originFxItem;}
	inline FxItem * parentFx() const {return parentFxItem;}


signals:
	void deleteMe(Executer *exec);
	void changed(Executer *exec);
	void sceneExecuterStartSignal(FxSceneItem *scene);
	void wantedDeleteFxScene(FxSceneItem *scene);

	friend class ExecCenter;
};

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

class FxListExecuter : public Executer
{
	Q_OBJECT
protected:
	FxList *fxList;
	FxItem *curFx;
	FxItem *nxtFx;

private:
	int m_playbackProgress;
	int m_currentInitialVolume;
	int m_loopCount;

public:
	inline TYPE type() {return EXEC_FXLIST;}
	bool processExecuter();

	void setFxList(FxList *fx_list);
	void setNextFx(FxItem *fx);
	inline FxItem * nextFx() const {return nxtFx;}
	void setCurrentFx(FxItem *fx);
	inline FxItem * currentFx() const {return curFx;}
	FxAudioItem * currentFxAudio();
	inline void setCurrentInitialVol(int vol) {m_currentInitialVolume = vol;}

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
	void listProgressStepChanged(int step1, int step2);

	friend class ExecCenter;

};

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

class SceneExecuter : public Executer
{
protected:
	FxSceneItem *curScene;
	LIGHT::SceneSeqState sceneState;

public:
	inline TYPE type() {return EXEC_SCENE;}
	bool processExecuter();

protected:
	SceneExecuter(AppCentral &app_central, FxSceneItem *scene, FxItem *parentFx);


	friend class ExecCenter;
};


//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
class ScriptExecuter : public Executer
{
protected:
	FxScriptItem *m_fxScriptItem;			///< pointer to FxScriptItem, which is the parent
	FxScriptList m_script;
	int m_currentLineNum;
	QList<FxSceneItem*> m_clonedSceneList;
	QString m_lastScriptError;

public:
	inline TYPE type() {return EXEC_SCRIPT;}
	bool processExecuter();

protected:
	ScriptExecuter(AppCentral &app_central, FxScriptItem *script, FxItem *parentFx);
	virtual ~ScriptExecuter();

    QString getTargetFxItemFromPara(FxScriptLine *line, const QString &paras, FxItemList &fxList);
	FxItemList getExecuterTempCopiesOfFx(FxItem *fx) const;

	bool executeLine(FxScriptLine *line);
	bool executeCmdStart(FxScriptLine *line);
	bool executeCmdStop(FxScriptLine *line);
	bool executeFadeIn(FxScriptLine * line);
	bool executeFadeOut(FxScriptLine *line);
	bool executeYadiDMXMergeMode(FxScriptLine *line);
	bool executeLoopExt(FxScriptLine *line);

	friend class ExecCenter;
};

#endif // FXLISTEXECUTER_H

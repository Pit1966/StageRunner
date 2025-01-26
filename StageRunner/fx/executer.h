//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef FXLISTEXECUTER_H
#define FXLISTEXECUTER_H

#include "commandsystem.h"
#include "fx/fxscripttools.h"
#include "system/extelapsedtimer.h"

#include <QObject>
#include <QString>
#include <QElapsedTimer>

// using namespace AUDIO;

class FxList;
class AppCentral;
class AudioControl;
class LightControl;
class FxControl;
class FxItem;
class FxSceneItem;
class FxScriptItem;
class FxScriptList;
class FxTimeLineItem;

typedef QList<FxItem*> FxItemList;

class Executer : public QObject
{
	Q_OBJECT
public:
	enum TYPE {
		EXEC_BASE,
		EXEC_FXLIST,
		EXEC_SCENE,
		EXEC_SCRIPT,
		EXEC_TIMELINE
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
	ExtElapsedTimer runTime;					///< This timer holds the overall running time of the executer
	ExtElapsedTimer runTimeOne;					///< Timer holds runtime for current loop (only used for progress!!)
	qint64 eventTargetTimeMs	= 0;			///< This is the target time for the next event that has to be executed by the executer (if runTime < targetTimeMs nothing is todo)
	qint64 lastProgressTimeMs	= 0;			///< May get used by derived classes

	int m_pausedAtMs			= -1;			///< run time when pause started

	bool isWaitingForAudio		= false;
	volatile STATE myState		= EXEC_IDLE;

private:
	int use_cnt					= 0;			///< if this usage counter is > 0 the object should not been deleted

protected:
	Executer(AppCentral &app_central, FxItem *originFx = 0);
	~Executer();

public:
	inline int useCount() const {return use_cnt;}
	void setIdString(const QString & str);
	inline virtual TYPE type() const {return EXEC_BASE;}
	inline STATE state() const {return myState;}
	virtual bool processExecuter();
	virtual void processProgress();
	virtual void onPauseEvent(bool active);

	void destroyLater();
	bool activateProcessing(bool continueProcessing = false);
	bool deactivateProcessing();
	bool setPaused(bool state);
	bool setFinish();
	inline bool isRunning() const {return (myState == EXEC_RUNNING || myState == EXEC_FINISH);}
	inline bool isPaused() const {return myState == EXEC_PAUSED;}
	inline qint64 currentTargetTimeMs() {return eventTargetTimeMs;}
	inline qint64 currentRunTimeMs() {return runTime.elapsed();}
	inline bool processTimeReached() {return (runTime.elapsed() >= eventTargetTimeMs);}
	inline void setTargetTimeToCurrent() {eventTargetTimeMs = runTime.elapsed();}
	inline void setEventTargetTimeRelative(qint64 ms) {eventTargetTimeMs = runTime.elapsed() + ms;}
	inline void setEventTargetTimeAbsolute(qint64 ms) {eventTargetTimeMs = ms;}
	QString getIdString() const;
	inline void setOriginFx(FxItem *fx) {originFxItem = fx;}
	inline FxItem * originFx() const {return originFxItem;}
	inline FxItem * parentFx() const {return parentFxItem;}


signals:
	void deleteMe(Executer *exec);
	void changed(Executer *exec);
	void sceneExecuterStartSignal(FxSceneItem *scene);
	void wantedDeleteFxScene(FxSceneItem *scene);
	void executerStatusChanged(FxItem *fx, const QString &msg);

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
	mutable int m_calculatedExecutionTime;				// this is calculated execution time as a sum of all FxItems in the sequence (-1: is uncalculated)

public:
	inline TYPE type() const override {return EXEC_FXLIST;}
	bool processExecuter() override;
	void processProgress() override;

	void setFxList(FxList *fx_list);
	void setNextFx(FxItem *fx);
	inline FxItem * nextFx() const {return nxtFx;}
	void setCurrentFx(FxItem *fx);
	inline FxItem * currentFx() const {return curFx;}
	FxAudioItem * currentFxAudio();
	inline void setCurrentInitialVol(int vol) {m_currentInitialVolume = vol;}
	qint64 calculateSeqExecutionTime() const;


protected:
	FxListExecuter(AppCentral & app_central, FxList *fx_list);
	~FxListExecuter();

protected slots:
	void audioCtrlReceiver(AUDIO::AudioCtrlMsg msg);
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
	inline TYPE type() const override {return EXEC_SCENE;}
	bool processExecuter() override;

protected:
	SceneExecuter(AppCentral &app_central, FxSceneItem *scene, FxItem *parentFx);


	friend class ExecCenter;
};


//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
class ScriptExecuter : public Executer
{
	Q_OBJECT
protected:
	FxScriptItem *m_fxScriptItem;			///< pointer to FxScriptItem, which is the parent
	FxScriptList m_script;
	int m_currentLineNum;
	volatile STATE m_lastState	= EXEC_IDLE;

	QList<FxSceneItem*> m_clonedSceneList;
	QString m_lastScriptError;

	bool m_breakOnCancel;					///< should executer immediately break, if canceled
	bool m_disableMultiStart;
	bool m_startedByDMX;

public:
	inline TYPE type() const override {return EXEC_SCRIPT;}
	bool processExecuter() override;
	void processProgress() override;
	void onPauseEvent(bool active) override;
	bool isMultiStartDisabled() const {return m_disableMultiStart;}
	void setDmxStarted(bool state) {m_startedByDMX = state;}
	bool isStartedByDmx() const {return m_startedByDMX;}
	const QString & lastScriptError() const {return m_lastScriptError;}

protected:
	ScriptExecuter(AppCentral &app_central, FxScriptItem *script, FxItem *parentFx);
	virtual ~ScriptExecuter();

    QString getTargetFxItemFromPara(FxScriptLine *line, const QString &paras, FxItemList &fxList);
	QString getFirstParaOfString(QString &parastr);
	int getPos(QString &restPara);

	FxItemList getExecuterTempCopiesOfFx(FxItem *fx) const;

	bool executeLine(FxScriptLine *line, bool & reExecDelayed);
	bool executeCmdStart(FxScriptLine *line);
	bool executeCmdStop(FxScriptLine *line);
	bool executeFadeIn(FxScriptLine * line);
	bool executeFadeOut(FxScriptLine *line);
	bool executeYadiDMXMergeMode(FxScriptLine *line);
	bool executeLoopExt(FxScriptLine *line);
	bool executeGrabScene(FxScriptLine *line);
	bool executeBlack(FxScriptLine *line);
	bool executeRemote(FxScriptLine *line);
	bool executeFadeVolume(FxScriptLine *line);
	bool executeMode(FxScriptLine *line);
	bool executePause(FxScriptLine *line);


	static bool executeSingleCmd(const QString &linestr);

signals:
	void listProgressStepChanged(int step1, int step2);

	friend class ExecCenter;
};

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

#endif // FXLISTEXECUTER_H

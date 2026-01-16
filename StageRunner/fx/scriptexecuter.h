#ifndef SCRIPTEXECUTER_H
#define SCRIPTEXECUTER_H

#include "executer.h"

class ScriptExecuter : public Executer
{
	Q_OBJECT
protected:
	FxScriptItem *m_fxScriptItem;			///< pointer to FxScriptItem, which is the parent
	FxScriptList m_script;
	int m_currentLineNum;
	int m_defUniverse		= 0;			///< default option for some script commands (intern 0:MAX_UNIVERSE-1, user [1:MAX-UNIVERSE])
	int m_defStaticSceneNum	= 0;			///< default option for some script commands
	int m_defFadeTimeMs		= 0;			///< default fadeout/in time in milliseconds for some commands
	volatile STATE m_lastState	= EXEC_IDLE;

	QList<FxSceneItem*> m_clonedSceneList;
	QString m_lastScriptError;
	QString m_defFixShortId;				///< default shortID string for FIX command

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
	QString getFirstParaOfString(QString &parastr, bool allowCommaSep = false);
	int getFrontIntOfString(QString &parastr, bool *ok = nullptr, const QString &errHint = {});
	int getPos(QString &restPara);
	int convertToDmxVal(const QString &valstr);

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
	bool executeDMX(FxScriptLine *line);
	bool executeDefault(FxScriptLine *line);
	bool executeFix(FxScriptLine *line);


	bool executeCommandBlock(const QString &cmds);

	static bool executeSingleCmd(const QString &linestr);

signals:
	void listProgressStepChanged(int step1, int step2);

	friend class ExecCenter;
	friend class TimeLineExecuter;
};

#endif // SCRIPTEXECUTER_H

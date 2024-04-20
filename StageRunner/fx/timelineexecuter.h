#ifndef TIMELINEEXECUTER_H
#define TIMELINEEXECUTER_H

#include "executer.h"

class FxTimeLineObj;

class TimeLineExecuter : public Executer
{
	Q_OBJECT

protected:
	FxTimeLineItem *m_fxTimeLine;			///< the parent FxTimeLineItem
	bool m_disableMultiStart	= true;
	bool m_timelineValid		= false;
	int m_curObjPos				= 0;

	QList<FxTimeLineObj*> m_sortedTimeLineObjList;

public:
	inline TYPE type() const override {return EXEC_TIMELINE;}
	bool processExecuter() override;
	void processProgress() override;
	bool isMultiStartDisabled() const {return m_disableMultiStart;}

protected:
	TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx);
	virtual ~TimeLineExecuter();

	FxTimeLineObj *findNextObj();
	bool getTimeLineObjs(FxTimeLineItem *fx);

	friend class ExecCenter;
};


#endif // TIMELINEEXECUTER_H

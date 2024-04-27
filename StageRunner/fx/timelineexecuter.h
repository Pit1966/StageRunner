#ifndef TIMELINEEXECUTER_H
#define TIMELINEEXECUTER_H

#include "executer.h"

class FxTimeLineObj;

class TimeLineExecuter : public Executer
{
	Q_OBJECT
public:
	enum EV_TYPE {
		EV_UNKNOWN,
		EV_BEGIN,
		EV_END
	};

	class Event {
	public:
		EV_TYPE evType		= EV_UNKNOWN;
		int evNum			= 0;
		int timeMs			= 0;
		int trackID			= 0;
		FxTimeLineObj * obj	= nullptr;

		Event(EV_TYPE type = EV_UNKNOWN)
			: evType(type)
		{}
	};

protected:
	FxTimeLineItem *m_fxTimeLine;			///< the parent FxTimeLineItem
	bool m_disableMultiStart	= true;
	bool m_timelineValid		= false;
	int m_curEventPos			= 0;		///< this is current pending obj number. Next to be executed
	volatile STATE m_lastState	= EXEC_IDLE;

	QList<Event> m_sortedObjEventList;

public:
	inline TYPE type() const override {return EXEC_TIMELINE;}
	bool processExecuter() override;
	void processProgress() override;
	bool isMultiStartDisabled() const {return m_disableMultiStart;}
	bool setReplayPosition(int ms);

protected:
	TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx);
	virtual ~TimeLineExecuter();

	Event findNextObj();
	bool getTimeLineObjs(FxTimeLineItem *fx);


	bool execObjBeginPosForFx(FxItem *fx, Event &ev);
	bool execObjEndPosForFx(FxItem *fx, Event &ev);

signals:
	void playPosChanged(int ms);
	void timeLineStatusChanged(int executerStatus);

	friend class ExecCenter;
};


#endif // TIMELINEEXECUTER_H

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
		EV_STOP,
		EV_TIMELINE_END
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
	int m_finalEventAtMs		= 0;		///< time position of last cue executed in this timeline (run duration)
	volatile STATE m_lastState	= EXEC_IDLE;

	QList<Event> m_sortedObjEventList;
	QHash<int,FxSceneItem*> m_idToClonedSceneHash;	///< this is a hash Original fxID -> cloned FxItem of FxSceneItems that were started in the timeline as copy from main list

public:
	inline TYPE type() const override {return EXEC_TIMELINE;}
	bool processExecuter() override;
	void processProgress() override;
	bool isMultiStartDisabled() const {return m_disableMultiStart;}
	bool setReplayPosition(int ms);

protected:
	TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx);
	virtual ~TimeLineExecuter();

	FxSceneItem *getScene(int fxID);

	Event findNextObj();
	bool getTimeLineObjs(FxTimeLineItem *fx);


	bool execObjBeginPosForFx(int fxID, Event &ev);
	bool execObjEndPosForFx(int fxID, Event &ev);

signals:
	void playPosChanged(int ms);
	void timeLineStatusChanged(int executerStatus);
	void listProgressChanged(int step1, int step2);

	friend class ExecCenter;
};


#endif // TIMELINEEXECUTER_H

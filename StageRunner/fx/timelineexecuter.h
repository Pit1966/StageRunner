#ifndef TIMELINEEXECUTER_H
#define TIMELINEEXECUTER_H

#include "executer.h"

class FxTimeLineObj;
namespace PS_TL {
	class TimeLineCurve;
	class TimeLineCurveData;
}

class TimeLineExecuter : public Executer
{
	Q_OBJECT
public:
	enum EV_TYPE {
		EV_UNKNOWN,
		EV_BEGIN,
		EV_STOP,
		EV_TIMELINE_END,
		EV_ENVELOPE_START
	};

	class Event {
	public:
		EV_TYPE evType		= EV_UNKNOWN;
		int evNum			= 0;			///< not used ... only for debugging
		int timeMs			= 0;
		int trackID			= 0;
		FxTimeLineObj * obj	= nullptr;

		Event(EV_TYPE type = EV_UNKNOWN)
			: evType(type)
		{}
	};

	class Envelope {
	public:
		int trackID			= 0;
		int curveTrack		= 0;
		int targetTrack		= 0;
		QList<int> usedAudioSlots;	// a list of audioslots with aktiv FxAudioItems or FxClipItems running

		/// Attention! this is only a temporary storage for the pointer.
		/// the TimeLineCurveData object will not be deleted
		PS_TL::TimeLineCurveData *curveDat = nullptr;
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
	QList<int> m_activeFxAudioList;					///< this is a list of FxItems that were started from the scene (mainly FxAudioItems ...)
	QList<int> m_activeFxScriptList;				///< list of FxScriptItems started in the timeline
	QList<Envelope*> m_curveTracks;					///< this is a list of running envelopes aka curves

	int m_curveIntervalMs		= 40;

	// temp
	int m_pausedAtMs				= -1;			///< run time when pause started
	int m_nextCurveTrackEventAtMs	= -1;
	bool m_triggerCurveEvent		= false;		///< override next timeline ITEMS event with a regulary called envelope/curve event
	bool m_stopAllFxAtFinish		= false;		///< if set, all Fx started by this timeline are stopped when timeline has finished (stop audio, script, scene)

public:
	inline TYPE type() const override {return EXEC_TIMELINE;}
	void onPauseEvent(bool active) override;
	bool processExecuter() override;
	void processProgress() override;
	bool isMultiStartDisabled() const {return m_disableMultiStart;}
	bool setReplayPosition(int ms);
	void setStopAllAtFinishFlag() {m_stopAllFxAtFinish = true;}
	void stopAllChildFx();

protected:
	TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx);
	virtual ~TimeLineExecuter();

	FxSceneItem *getScene(int fxID);
	Envelope *getEnvelopeForTrackId(int trackID);

	Event findNextObj();
	bool getTimeLineObjs(FxTimeLineItem *fx);

	bool processEnvelopes(int estTimeMs);

	bool execObjBeginPosForFx(int fxID, Event &ev);
	bool execObjEndPosForFx(int fxID, Event &ev);

	void addFxToActiveAudioList(FxItem *fx);
	void addFxToActiveScriptList(FxItem *fx);

private:
	void _finishMe();

protected slots:
	void onAudioStatusChanged(AUDIO::AudioCtrlMsg msg);


signals:
	void playPosChanged(int ms);
	void timeLineStatusChanged(int executerStatus);
	void timeLineStatusMsgChanged(FxItem *fx, const QString &msg);
	void listProgressChanged(int step1, int step2);

	friend class ExecCenter;
};


#endif // TIMELINEEXECUTER_H

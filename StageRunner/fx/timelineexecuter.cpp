#include "timelineexecuter.h"

#include "log.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/audiocontrol.h"
#include "system/lightcontrol.h"
#include "system/videocontrol.h"
#include "fx/fxitem.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/fxaudioitem.h"
#include "fx/fxclipitem.h"
#include "fx/fxsceneitem.h"
#include "mods/timeline/timelineitem.h"

bool TimeLineExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) {
		return false;
	}

	if (m_sortedObjEventList.isEmpty() || m_curEventPos >= m_sortedObjEventList.size()) {
		LOGERROR(tr("Event list not valid in %1").arg(m_fxTimeLine->name()));
		return false;
	}

	Event &ev = m_sortedObjEventList[m_curEventPos];
	FxTimeLineObj *obj = ev.obj;
	if (ev.evType == EV_BEGIN) {
		qDebug() << "EV" << ev.evNum +1 << "begin time:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		if (obj->type() == LINKED_FXITEM) {
			FxItem *fx = FxItem::findFxById(obj->fxID());
			execObjBeginPosForFx(fx, ev);
		}
	}
	else if (ev.evType == EV_END) {
		qDebug() << "EV" << ev.evNum +1 << "end time:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		if (obj->type() == LINKED_FXITEM) {
			FxItem *fx = FxItem::findFxById(obj->fxID());
			execObjEndPosForFx(fx, ev);
		}

	}

	Event nextEv = findNextObj();
	if (nextEv.evType == EV_UNKNOWN) {
		emit timeLineStatusChanged(EXEC_FINISH);
		return false;			// no more active. We are ready
	}

	setEventTargetTimeAbsolute(nextEv.timeMs);
	return true;
}

void TimeLineExecuter::processProgress()
{
	if (myState == EXEC_FINISH) {
		qDebug() << "finish timeline" << m_fxTimeLine->name();
		emit timeLineStatusChanged(EXEC_FINISH);
		destroyLater();
	}
	else if (myState == EXEC_RUNNING) {
		if (m_lastState != EXEC_RUNNING)
			emit timeLineStatusChanged(EXEC_RUNNING);
		emit playPosChanged(runTimeOne.elapsed());		///< todo timeline
	}

	m_lastState = myState;
}

bool TimeLineExecuter::setReplayPosition(int ms)
{
	if (m_sortedObjEventList.isEmpty())
		return false;

	int n = 0;

	while (n < m_sortedObjEventList.size() && m_sortedObjEventList.at(n).timeMs < ms)
		n++;

	if (n >= m_sortedObjEventList.size()) {
		LOGERROR(tr("Start position %1ms not valid for timeline: %2").arg(ms).arg(m_fxTimeLine->name()));
		return false;
	}

	m_curEventPos = n;
	setEventTargetTimeAbsolute(m_sortedObjEventList.at(n).timeMs);

	runTime.setMs(ms);			/// @todo not correct for loop
	runTimeOne.setMs(ms);
	return true;
}

TimeLineExecuter::TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx)
	: Executer(appCentral, timeline)
	, m_fxTimeLine(timeline)
{
	parentFxItem = parentFx;
	m_timelineValid = getTimeLineObjs(timeline);
	if (!m_timelineValid) {
		destroyLater();
	}
	else {
		setEventTargetTimeAbsolute(m_sortedObjEventList.at(m_curEventPos).timeMs);
	}
}

TimeLineExecuter::~TimeLineExecuter()
{
	qDebug() << "destroyed timeline executer";
}

TimeLineExecuter::Event TimeLineExecuter::findNextObj()
{
	m_curEventPos++;
	if (m_curEventPos < m_sortedObjEventList.size())
		return m_sortedObjEventList.at(m_curEventPos);

	return {};
}

/**
 * @brief Build a time sorted list of references to the timeline objects in the timeline
 * @param fx
 */
bool TimeLineExecuter::getTimeLineObjs(FxTimeLineItem *fx)
{
	if (!FxItem::exists(fx))
		return false;

	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		for (int i=0; i<fx->timeLineObjCount(t); i++) {
			FxTimeLineObj *o = fx->timeLineObjAt(t, i);
			if (!o) break;			// should never happen

			// insert BEGIN event of object in list searching the correct time position
			int n = 0;
			while (n < m_sortedObjEventList.size() && o->beginMs() > m_sortedObjEventList.at(n).timeMs)
				n++;
			Event ev(EV_BEGIN);
			ev.evNum = n;
			ev.timeMs = o->beginMs();
			ev.trackID = t;
			ev.obj = o;
			m_sortedObjEventList.insert(n, ev);

			// insert END event of object in list searching the correct time position
			n = 0;
			while (n < m_sortedObjEventList.size() && o->endMs() > m_sortedObjEventList.at(n).timeMs)
				n++;
			ev.evType = EV_END;
			ev.evNum = n;
			ev.timeMs = o->endMs();
			m_sortedObjEventList.insert(n, ev);
		}
	}

	if (m_sortedObjEventList.isEmpty())
		return false; // nothing to execute

	return true;	// at least one timeline obj in list
}

bool TimeLineExecuter::execObjBeginPosForFx(FxItem *fx, Event &ev)
{
	if (!fx) return false;

	bool ok = false;

	switch (fx->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
			int pos = -1;

			if (fxa->isFxClip) {
				ok = myApp.unitVideo->startFxClip(static_cast<FxClipItem*>(fx));
			}
			else if (pos >= -1) {
				ok = myApp.unitAudio->startFxAudio(fxa, this, pos);
			}
			else {
				ok = myApp.unitAudio->startFxAudio(fxa, this);
			}
		}
		break;
	case FX_SCENE:
		/// @todo it would be better to copy the scene and actually fade in the new instance here
		/// Further the executer is not handed over to scene
		ok = myApp.unitLight->startFxScene(static_cast<FxSceneItem*>(fx));
		break;
	default:
		LOGERROR(tr("Timeline '%1': Executing of target is not supported! Time pos: %2 seconds")
				 .arg(m_fxTimeLine->name()));
	}

	return ok;
}

bool TimeLineExecuter::execObjEndPosForFx(FxItem *fx, Event &ev)
{
	if (!fx) return false;

	bool ok = false;

	switch (fx->fxType()) {
	case FX_AUDIO:
		{
			FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
			int fade_time = 3000;

			if (fxa->isFxClip) {
				myApp.unitVideo->videoBlack(fade_time);
			}
			else if (fade_time > 0) {
				myApp.unitAudio->fadeoutFxAudio(fxa, fade_time);
			}
			else {
				myApp.unitAudio->stopFxAudio(fxa);
			}

			ok = true;
		}
		break;
	case FX_SCENE:
		ok = myApp.unitLight->stopFxScene(static_cast<FxSceneItem*>(fx));
		break;
	default:
		LOGERROR(tr("Timeline '%1': Executing of target is not supported! Time pos: %2 seconds")
				 .arg(m_fxTimeLine->name()));
	}

	return ok;
}

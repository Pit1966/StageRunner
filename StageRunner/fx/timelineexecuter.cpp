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

		if (obj->type() >= LINKED_FX_SCENE && obj->type() <= LINKED_FX_LAST) {
			execObjBeginPosForFx(obj->fxID(), ev);
		}
	}
	else if (ev.evType == EV_STOP) {
		qDebug() << "EV" << ev.evNum +1 << "end time:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		if (obj->type() >= LINKED_FX_SCENE && obj->type() <= LINKED_FX_LAST) {
			execObjEndPosForFx(obj->fxID(), ev);
		}
	}
	else if (ev.evType == EV_TIMELINE_END) {
		emit timeLineStatusChanged(EXEC_FINISH);
		emit listProgressChanged(0, 0);
		return false;			// no more active. We are ready
	}

	Event nextEv = findNextObj();
	if (nextEv.evType == EV_UNKNOWN) {
		LOGERROR(tr("Found unknown timeline item -> timeline finished"));
		emit timeLineStatusChanged(EXEC_FINISH);
		emit listProgressChanged(0, 0);
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
		emit listProgressChanged(0,0);
		destroyLater();
	}
	else if (myState == EXEC_RUNNING) {
		if (m_lastState != EXEC_RUNNING)
			emit timeLineStatusChanged(EXEC_RUNNING);
		int ranMs = runTimeOne.elapsed();
		emit playPosChanged(ranMs);
		int perMille = ranMs * 1000 / m_finalEventAtMs;
		emit listProgressChanged(perMille, 0);
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
	for (FxSceneItem *fx : qAsConst(m_idToClonedSceneHash)) {
		if (!fx->isOnStageIntern() && !fx->isActive()) {
			emit wantedDeleteFxScene(fx);
		} else {
			fx->setDeleteOnFinished();
		}
	}
	qDebug() << "destroyed timeline executer";
}

/**
 * @brief Get cloned temporary copy of scene with original fxID
 * @param fxID fx id of FxItem original used in play
 * @return Address of cloned scene
 *
 * if the fxID is not in the internal hash. The scene will be searched in FxItem pool and a
 * copy will be made first
 */
FxSceneItem *TimeLineExecuter::getScene(int fxID)
{
	if (m_idToClonedSceneHash.contains(fxID))
		return m_idToClonedSceneHash[fxID];

	FxSceneItem *orgfx = dynamic_cast<FxSceneItem*>(FxItem::findFxById(fxID));
	if (!orgfx) {
		LOGERROR(tr("Could not find FX scene with ID %1, or it is no scene!").arg(fxID));
		return nullptr;
	}
	FxSceneItem *cfx = new FxSceneItem(*orgfx);
	cfx->setName(tr("%1:%2_tmp").arg(originFxItem->name(), orgfx->name()));
	cfx->setIsTempCopyOf(orgfx);
	m_idToClonedSceneHash.insert(fxID, cfx);
	return cfx;
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

	// helper for timeline end
	int lastEventMs = 0;

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

			if (o->stopAtMs() > 0) {
				// insert END event of object in list searching the correct time position
				n = 0;
				while (n < m_sortedObjEventList.size() && o->stopAtMs() > m_sortedObjEventList.at(n).timeMs)
					n++;
				ev.evType = EV_STOP;
				ev.evNum = n;
				ev.timeMs = o->stopAtMs();
				m_sortedObjEventList.insert(n, ev);
			}

			if (o->endMs() > lastEventMs)
				lastEventMs = o->endMs();
		}
	}

	if (m_sortedObjEventList.isEmpty()) {
		m_finalEventAtMs = 0;
		return false; // nothing to execute
	}

	// add timeline end event
	if (lastEventMs > 0) {
		Event ev(EV_TIMELINE_END);
		ev.timeMs = lastEventMs;
		m_sortedObjEventList.append(ev);
	}

	m_finalEventAtMs = m_sortedObjEventList.last().timeMs;

	return true;	// at least one timeline obj in list
}

bool TimeLineExecuter::execObjBeginPosForFx(int fxID, Event &ev)
{
	FxItem *fx = FxItem::findFxById(fxID);
	if (!fx) {
		LOGERROR(tr("FX with id #%1 not found in timeline executer").arg(fxID));
		return false;
	}

	bool ok = false;

	int fxtype = fx->fxType();
	if (fxtype == FX_AUDIO)	{
		FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
		int pos = -1;

		if (fxa->isFxClip) {
			ok = myApp.unitVideo->startFxClip(static_cast<FxClipItem*>(fx));
		}
		else if (pos > -1) {
			ok = myApp.unitAudio->startFxAudio(fxa, this, pos);
		}
		else {
			int startAtMs = -1;
			int initVol = -1;
			int fadeinms = -1;
			if (ev.obj->fadeInDurationMs())
				fadeinms = ev.obj->fadeInDurationMs();

				// ok = myApp.unitAudio->startFxAudio(fxa, this, 0, 0);
				// ExtElapsedTimer wait(true);
				// int slot = -1;
				// while (ok && slot < 0 && wait.elapsed() < FX_AUDIO_START_WAIT_DELAY) {
				// 	slot = myApp.unitAudio->getSlotForFxAudio(fxa);
				// }
				// // qDebug() << "fadein timeline audio in slot" << slot;
				// if (ok && slot >= 0) {
				// 	myApp.unitAudio->fadeinFxAudio(0, fxa->initialVolume, fadeinms);
				// }

			ok = myApp.unitAudio->startFxAudio(fxa, this, startAtMs, initVol, fadeinms);
		}
	}
	else if (fxtype == FX_SCENE) {
		/// Further the executer is not handed over to scene
		FxSceneItem *fxs = getScene(fxID);
		if (fxs) {
			fxs->setFadeInTime(ev.obj->fadeInDurationMs());
			fxs->setFadeOutTime(ev.obj->fadeOutDurationMs());
			ok = myApp.unitLight->startFxScene(fxs);
		}
	}
	else {
		LOGERROR(tr("Timeline '%1': Executing of target is not supported! Time pos: %2 seconds")
				 .arg(m_fxTimeLine->name()));
	}

	return ok;
}

bool TimeLineExecuter::execObjEndPosForFx(int fxID, Event &ev)
{
	FxItem *fx = FxItem::findFxById(fxID);
	if (!fx) {
		LOGERROR(tr("FX with id #%1 not found in timeline executer").arg(fxID));
		return false;
	}

	bool ok = false;

	int fxtype = fx->fxType();
	if (fxtype == FX_AUDIO)	{
		FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
		int fade_ms = ev.obj->fadeOutDurationMs();
		if (fxa->isFxClip) {
			myApp.unitVideo->videoBlack(fade_ms);
		}
		else if (fade_ms > 0) {
			myApp.unitAudio->fadeoutFxAudio(fxa, fade_ms);
		}
		else {
			myApp.unitAudio->stopFxAudio(fxa);
		}

		ok = true;
	}
	else if (fxtype == FX_SCENE) {
		FxSceneItem *fxs = getScene(fxID);
		if (fxs) {
			fxs->setFadeOutTime(ev.obj->fadeOutDurationMs());
			ok = myApp.unitLight->stopFxScene(fxs);
		}
	}
	else {
		LOGERROR(tr("Timeline '%1': Executing of target is not supported! Time pos: %2 seconds")
				 .arg(m_fxTimeLine->name()));
	}

	return ok;
}

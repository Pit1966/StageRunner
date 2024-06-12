#include "timelineexecuter.h"

#include "log.h"
#include "appcontrol/appcentral.h"
#include "appcontrol/audiocontrol.h"
#include "system/fxcontrol.h"
#include "system/lightcontrol.h"
#include "system/videocontrol.h"
#include "fx/fxitem.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/fxaudioitem.h"
#include "fx/fxclipitem.h"
#include "fx/fxsceneitem.h"
#include "fx/fxtimelinetrack.h"
#include "fx/fxscriptitem.h"
#include "mods/timeline/timelinebox.h"
#include "mods/timeline/timelinecurve.h"

using namespace PS_TL;
using namespace AUDIO;

// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------

bool TimeLineExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) {
		return false;
	}

	if (m_sortedObjEventList.isEmpty() || m_curEventPos >= m_sortedObjEventList.size()) {
		LOGERROR(tr("Event list not valid in %1").arg(m_fxTimeLine->name()));
		return false;
	}

	if (m_triggerCurveEvent) {
		// qDebug() << "  override curve event at: " << runTime.elapsed();
		// do envelope things here
		if (!processEnvelopes(m_nextCurveTrackEventAtMs)) {
			LOGERROR(tr("Envelope processing failed for %1").arg(m_fxTimeLine->name()));
			return false;
		}

		m_nextCurveTrackEventAtMs += m_curveIntervalMs;		// next curve event in 40ms
		if (m_nextCurveTrackEventAtMs <= m_sortedObjEventList.at(m_curEventPos).timeMs) {
			setEventTargetTimeAbsolute(m_nextCurveTrackEventAtMs);
			m_triggerCurveEvent = true;
		}
		else {
			setEventTargetTimeAbsolute(m_sortedObjEventList.at(m_curEventPos).timeMs);
			m_triggerCurveEvent = false;
		}
		return true;
	}

	Event &ev = m_sortedObjEventList[m_curEventPos];
	FxTimeLineObj *obj = ev.obj;
	if (ev.evType == EV_BEGIN) {
		qDebug() << "EV" << ev.evNum  << "begin time:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		if (obj->type() >= LINKED_FX_SCENE && obj->type() <= LINKED_FX_LAST) {
			execObjBeginPosForFx(obj->fxID(), ev);
		}
	}
	else if (ev.evType == EV_STOP) {
		qDebug() << "EV" << ev.evNum << "end time:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		if (obj->type() >= LINKED_FX_SCENE && obj->type() <= LINKED_FX_LAST) {
			execObjEndPosForFx(obj->fxID(), ev);
		}
	}
	else if (ev.evType == EV_TIMELINE_END) {
		_finishMe();
		return false;			// no more active. We are ready
	}
	else if (ev.evType == EV_ENVELOPE_START) {
		qDebug() << "EV" << ev.evNum << "envelope start:" << runTime.elapsed() << ev.timeMs << ev.obj->label;

		// this actualy keeps the envelope running
		m_nextCurveTrackEventAtMs = m_curveIntervalMs;

		// do envelope things here
		if (!processEnvelopes(m_nextCurveTrackEventAtMs)) {
			LOGERROR(tr("Initial envelope processing failed for %1").arg(m_fxTimeLine->name()));
			return false;
		}
	}

	Event nextEv = findNextObj();
	if (nextEv.evType == EV_UNKNOWN) {
		LOGERROR(tr("Found unknown timeline item -> timeline finished"));
		_finishMe();
		return false;			// no more active. We are ready
	}

	if (m_nextCurveTrackEventAtMs > 0 && m_nextCurveTrackEventAtMs <= nextEv.timeMs) {
		setEventTargetTimeAbsolute(m_nextCurveTrackEventAtMs);
		m_triggerCurveEvent = true;
	}
	else {
		setEventTargetTimeAbsolute(nextEv.timeMs);
	}
	return true;
}

void TimeLineExecuter::processProgress()
{
	if (myState == EXEC_FINISH) {
		qDebug() << "finish timeline" << m_fxTimeLine->name();
		_finishMe();
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

	if (ms > 0) {
		// check if first item is an envelope item that we would miss, when setting the playback
		// position to a later time
		if (m_sortedObjEventList.at(0).evType == EV_ENVELOPE_START) {
			m_nextCurveTrackEventAtMs = ms;
			m_triggerCurveEvent = true;
			// override event target time
			setEventTargetTimeAbsolute(ms);
		}
	}

	runTime.setMs(ms);			/// @todo not correct for loop
	runTimeOne.setMs(ms);
	return true;
}

/**
 * @brief Stop all FX items that were started from this timeline and are still active
 */
void TimeLineExecuter::stopAllChildFx()
{
	// Stop audio
	while (!m_activeFxAudioList.isEmpty()) {
		int id = m_activeFxAudioList.takeFirst();
		myApp.unitAudio->stopFxAudioWithID(id);
	}

	// Stop scripts
	while (!m_activeFxScriptList.isEmpty()) {
		int id = m_activeFxScriptList.takeFirst();
		FxItem *fx = FxItem::findFxById(id);
		if (fx) {
			myApp.unitFx->stopAllFxScripts(fx);
		}
	}

	// Stop scenes
	for (FxSceneItem *fxs : std::as_const(m_idToClonedSceneHash)) {
		fxs->setFadeOutTime(100);
		myApp.unitLight->stopFxScene(fxs);
	}
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
	// free temporary objects in Envelopes / m_curveTracks

	for (int i=0; i<m_curveTracks.size(); i++) {
		Envelope *enve = m_curveTracks.at(i);
		delete enve->curveDat;
		delete enve;
	}

	for (FxSceneItem *fx : std::as_const(m_idToClonedSceneHash)) {
		if (!fx->isOnStageIntern() && !fx->isActive()) {
			emit wantedDeleteFxScene(fx);
		} else {
			fx->setDeleteOnFinished();
			fx->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT, 500);
		}
	}
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

TimeLineExecuter::Envelope *TimeLineExecuter::getEnvelopeForTrackId(int trackID)
{
	for (int i=0; i<m_curveTracks.size(); i++) {
		if (m_curveTracks.at(i)->targetTrack == trackID)
			return m_curveTracks.at(i);
	}
	return nullptr;
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

	for (int t=0; t<fx->trackCount(); t++) {
		FxTimeLineTrack *fxtrack = fx->trackAt(t);
		int trackid = fxtrack->trackId();
		if (fxtrack->trackType() == TRACK_ITEMS) {
			for (int i=0; i<fx->timeLineObjCount(trackid); i++) {
				FxTimeLineObj *o = fx->timeLineObjAt(trackid, i);
				if (!o) break;			// should never happen

				// insert BEGIN event of object in list searching the correct time position
				int n = 0;
				while (n < m_sortedObjEventList.size() && o->beginMs() > m_sortedObjEventList.at(n).timeMs)
					n++;
				Event ev(EV_BEGIN);
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
					ev.timeMs = o->stopAtMs();
					m_sortedObjEventList.insert(n, ev);
				}

				if (o->endMs() > lastEventMs)
					lastEventMs = o->endMs();
			}
		}
		else if (fxtrack->trackType() == TRACK_AUDIO_VOL) {
			// get first Item in TimeLineCurve
			FxTimeLineObj *o = fx->timeLineObjAt(trackid, 0);
			if (!o) {
				LOGERROR(tr("Missing Curve item in timeline track %1").arg(t+1));
				continue;
			}

			// Create a temporary TimeLineCurveData instance
			TimeLineCurveData *cd = new TimeLineCurveData();
			cd->setCurveData(o->configDat, fxtrack);

			// Audio envelope is always attached to the track directly above
			Envelope *envel = new Envelope();
			envel->trackID = trackid;
			envel->curveTrack = t;
			envel->targetTrack = t-1;
			envel->curveDat = cd;			// remember, we have to delete this later
			m_curveTracks.append(envel);

			// generate an initial envelope event for the beginning of the timeline
			Event ev(EV_ENVELOPE_START);
			ev.timeMs = 0;
			ev.trackID = trackid;
			ev.obj = o;

			m_sortedObjEventList.prepend(ev);
		}
	}

	if (m_sortedObjEventList.isEmpty()) {
		m_finalEventAtMs = 0;
		return false; // nothing to execute
	}

	// renumber event items
	for (int i=0; i<m_sortedObjEventList.size(); i++) {
		m_sortedObjEventList[i].evNum = i+1;
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

bool TimeLineExecuter::processEnvelopes(int estTimeMs)
{
	for (int i=0; i<m_curveTracks.size(); i++) {
		Envelope *enve = m_curveTracks.at(i);
		TimeLineCurveData *dat = enve->curveDat;
		int vol = dat->valAtMs(estTimeMs) / 10;
		for (int slot : std::as_const(enve->usedAudioSlots)) {
			myApp.unitAudio->setVolumeFromTimeLine(slot, vol);
		}
		// qDebug() << "yMP at" << estTimeMs << "=" << vol;
	}

	return true;
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
		int startAtMs = -1;
		int initVol = -1;
		int fadeinms = -1;
		if (ev.obj->fadeInDurationMs())
			fadeinms = ev.obj->fadeInDurationMs();

		// check if timeline of audio is target of an audio envelope
		Envelope *enve = getEnvelopeForTrackId(ev.trackID);
		if (enve) {
			initVol = enve->curveDat->valAtMs(ev.timeMs) / 10;	// start audio with current envelope volum
			fadeinms = -1;										// start audio without fadein time
		}

		FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
		if (fxa->isFxClip) {
			ok = myApp.unitVideo->startFxClip(static_cast<FxClipItem*>(fx));
		}
		else {
			ok = myApp.unitAudio->startFxAudio(fxa, this, startAtMs, initVol, fadeinms);
		}

		if (ok) {
			addFxToActiveAudioList(fx);

			if (enve) {
				int slot = fxa->startSlot;
				if (!enve->usedAudioSlots.contains(slot))
					enve->usedAudioSlots.append(slot);
			}
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
	else if (fxtype == FX_SCRIPT) {
		FxScriptItem *fxscript = dynamic_cast<FxScriptItem*>(fx);
		if (fxscript) {
			/*ScriptExecuter *scriptexec = */myApp.unitFx->startFxScript(fxscript);
			addFxToActiveScriptList(fx);
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
		int fade_ms = ev.obj->fadeOutDurationMs();

		// check if timeline of audio is target of an audio envelope
		Envelope *enve = getEnvelopeForTrackId(ev.trackID);
		if (enve) {
			fade_ms = 0;
		}

		FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
		if (fxa->isFxClip) {
			myApp.unitVideo->videoBlack(fade_ms);
		}
		else if (fade_ms > 0) {
			myApp.unitAudio->fadeoutFxAudio(fxa, fade_ms);
		}
		else {
			myApp.unitAudio->stopFxAudio(fxa);
		}

		if (enve) {
			if (enve->usedAudioSlots.contains(fxa->startSlot))
				enve->usedAudioSlots.removeOne(fxa->startSlot);
			if (!myApp.unitAudio->isAnyFxAudioActive())
				enve->usedAudioSlots.clear();
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

void TimeLineExecuter::addFxToActiveAudioList(FxItem *fx)
{
	if (fx == nullptr)
		return;
	if (!m_activeFxAudioList.contains(fx->id()))
		m_activeFxAudioList.append(fx->id());
}

void TimeLineExecuter::addFxToActiveScriptList(FxItem *fx)
{
	if (fx == nullptr)
		return;
	if (!m_activeFxScriptList.contains(fx->id()))
		m_activeFxScriptList.append(fx->id());
}

void TimeLineExecuter::_finishMe()
{
	if (m_stopAllFxAtFinish) {
		m_stopAllFxAtFinish = false;
		stopAllChildFx();
	}
	emit timeLineStatusChanged(EXEC_FINISH);
	emit listProgressChanged(0, 0);
}

void TimeLineExecuter::onAudioStatusChanged(AudioCtrlMsg msg)
{
	// qDebug() << "TimeLineExecuter audio status received " << msg.ctrlCmd << "Status" << msg.currentAudioStatus << "fx" << msg.fxAudio << "executer" << msg.executer;

	if (msg.executer != this) // not for me
		return;

	if (msg.currentAudioStatus == AUDIO_IDLE) {
		int fxid = msg.fxAudio->id();
		if (m_activeFxAudioList.contains(fxid)) {
			qDebug() << this << "remove audio with id" << fxid << "from active list";
			m_activeFxAudioList.removeOne(fxid);
		}
	}
}


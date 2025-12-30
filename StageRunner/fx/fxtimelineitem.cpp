#include "fxtimelineitem.h"

#include "fxtimelineobj.h"
#include "fxtimelinetrack.h"
#include "fx/exttimelineitem.h"

#include "fx/fxaudioitem.h"

FxTimeLineItem::FxTimeLineItem(FxList *fxList)
	: FxItem(fxList)
	, m_tracks{}
	, m_timelines{}
	, m_timeLineDurationMs(0)
{
	init();
}

FxTimeLineItem::FxTimeLineItem(const FxTimeLineItem &o)
	: FxItem(o.myParentFxList)
	, m_tracks{}
	, m_timelines{}
	, m_timeLineDurationMs(0)
{
	init();
	cloneFrom(o);

	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		for (int i=0; i<o.m_timelines[t].size(); i++) {
			FxTimeLineObj *obj = new FxTimeLineObj(*o.m_timelines[t].at(i));
			m_timelines[t].append(obj);
		}
	}

	for (int i=0; i<o.m_tracks.size(); i++) {
		FxTimeLineTrack *track = new FxTimeLineTrack(*o.m_tracks.at(i));
		m_tracks.append(track);
	}
}

FxTimeLineItem::~FxTimeLineItem()
{
	clear();

}

void FxTimeLineItem::clear()
{
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		m_timelines[t].clear();
	}
	while (!m_tracks.isEmpty())
		delete m_tracks.takeFirst();
}

FxTimeLineTrack *FxTimeLineItem::trackAt(int t) const
{
	if (m_tracks.size() <= t)
		return nullptr;

	return m_tracks.at(t);
}

void FxTimeLineItem::setTimeLineDuration(int ms)
{
	if (ms < 1000)
		ms = 1000;
	m_timeLineDurationMs = ms;
}

int FxTimeLineItem::timeLineObjCount(uint trackID) const
{
	if (trackID < TIMELINE_MAX_TRACKS)
		return m_timelines[trackID].size();

	return 0;
}

FxTimeLineObj *FxTimeLineItem::timeLineObjAt(uint trackID, int idx) const
{
	if (trackID < TIMELINE_MAX_TRACKS)
		return m_timelines[trackID].at(idx);

	return nullptr;
}

qint32 FxTimeLineItem::loopValue() const
{
	return 0;
}

void FxTimeLineItem::setLoopValue(qint32 val)
{
	Q_UNUSED(val)
}

void FxTimeLineItem::resetFx()
{
}

/**
 * @brief FxTimeLineItem::addFxAudioAndEnvelope
 * @param fxa
 * @param atMs
 * @return
 *
 * @note FxTimeLineItem must have no tracks or only one track
 */
bool FxTimeLineItem::addFxAudioAndEnvelope(FxAudioItem *fxa, int atMs)
{
	// create a ruler track, if not exist
	FxTimeLineTrack *track = trackAt(0);		// trackID 0 is ruler per default
	if (!track) {
		track = new FxTimeLineTrack(nullptr, PS_TL::TRACK_RULER, 0, 0, 34);
		m_tracks.append(track);
	}

	// check if there is one track. Create it, if not.
	int trackId = 1;
	track = trackAt(trackId);
	if (!track) {
		track = new FxTimeLineTrack(nullptr, PS_TL::TRACK_ITEMS, trackId, 0, 40);
		track->trackBgColor = -1;		// QColor(0x2a2a2a).rgba();
		m_tracks.append(track);
	}

	track->setTrackDuration(fxa->audioDuration);

	// Create TimelineItem linked to FxAudio for track
	FxTimeLineObj *obj = new FxTimeLineObj(atMs, fxa->audioDuration, fxa->name(), trackId);
	// not used for normal item: obj->configDat;
	obj->linkObjToFxItem(fxa);
	m_timelines[trackId].append(obj);

	// Add envelope track
	int envTrackId = m_tracks.size();
	FxTimeLineTrack *envTrack = new FxTimeLineTrack(nullptr, PS_TL::TRACK_AUDIO_VOL_CURVE, envTrackId, m_tracks.last()->yEndPos(), 42);
	envTrack->trackBgColor = 0x333355;		//QColor(0x232424).rgba();
	m_tracks.append(envTrack);
	// Create Envelope/Curve item
	obj = new FxTimeLineObj(0, fxa->audioDuration, "Audio Vol Envelope", envTrackId);
	obj->configDat = QString("nodes:0,1000;%1,1000").arg(fxa->audioDuration);
	m_timelines[envTrackId].append(obj);

	// set this FxTimeLineItem duration and Name
	setTimeLineDuration(fxa->audioDuration);
	setName(fxa->name() + " (envelope)");

	return true;
}

void FxTimeLineItem::init()
{
	myFxType = FX_TIMELINE;
	myclass = PrefVarCore::FX_TIMELINE_ITEM;

	addExistingVar(m_timeLineDurationMs, "DurationMs");

	m_tracks.parentVoid = this;
	addExistingVarSetList(m_tracks, QString("TimeLineTracks"), PrefVarCore::FX_TIMELINE_TRACK);

	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		m_timelines[t].parentVoid = this;
		addExistingVarSetList(m_timelines[t], QString("Timeline%1").arg(t), PrefVarCore::TIMELINE_OBJ);
	}
}

#ifndef FXTIMELINEITEM_H
#define FXTIMELINEITEM_H

#include "fxitem.h"
#include "tool/varset.h"
#include "tool/varsetlist.h"
#include "mods/timeline/timeline_defines.h"
#include "fx_defines.h"

#include <QCoreApplication>

class FxTimeLineObj;
class FxTimeLineTrack;
class FxAudioItem;

class FxTimeLineItem : public FxItem
{
	Q_OBJECT

private:
	VarSetList<FxTimeLineTrack*>m_tracks;
	VarSetList<FxTimeLineObj*>m_timelines[TIMELINE_MAX_TRACKS];		// m_timelines index must be trackID
	qint32 m_timeLineDurationMs;

public:
	FxTimeLineItem(FxList *fxList);
	FxTimeLineItem(const FxTimeLineItem &o);
	virtual ~FxTimeLineItem();

	void clear();
	inline int trackCount() const {return m_tracks.size();}
	FxTimeLineTrack *trackAt(int t) const;

	void setTimeLineDuration(int ms);
	int timeLineDurationMs() const {return m_timeLineDurationMs;}

	int timeLineObjCount(uint trackID) const;
	FxTimeLineObj *timeLineObjAt(uint trackID, int idx) const;

	qint32 loopValue() const override;
	void setLoopValue(qint32 val) override;
	void resetFx() override;

	// Convenience editing functions
	bool addFxAudioAndEnvelope(FxAudioItem *fxa, int atMs);

private:
	void init();

friend class FxTimeLineEditWidget;
friend class ExtTimeLineWidget;

};

#endif // FXTIMELINEITEM_H

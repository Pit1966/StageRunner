#ifndef TIMELINETRACK_H
#define TIMELINETRACK_H

#include "timeline_classes.h"

#include <QColor>
#include <QPointF>

namespace PS_TL {

enum TRACK_TYPE {
	TRACK_UNDEF,
	TRACK_ITEMS,
	TRACK_RULER,
	TRACK_AUDIO_VOL_CURVE,
	TRACK_AUDIO_PAN_CURVE,

	TRACK_CURVES = TRACK_AUDIO_VOL_CURVE
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineBox;
class TimeLineItem;
class TimeLineWidget;

class TimeLineTrack
{
public:
	qint64 trackBgColor	= -1;				///< invalid color
	int yAlignOffset	= 1;				///< means, a new track item is located at track y pos + this offset value (here 1)
	int yAlignSize		= 3;				///< this is the size alignment for a track item. Item height is yAlignSize smaller than track height

protected:
	TimeLineWidget *m_timeline;
	TRACK_TYPE m_type;
	int m_trackID;
	int m_yPos;
	int m_ySize;
	QList<TimeLineItem*> m_itemList;		///< this list contains the timeline items in this timeline
	bool m_isOverlay	= false;			///< Is a curve track and is overlay over other track, not displayed under

	TimeLineTrack *m_prev	= nullptr;		///< this is the track before
	TimeLineTrack *m_next	= nullptr;		///< this is the track behind

public:
	TimeLineTrack(TimeLineWidget *timeline, TRACK_TYPE type, int id, int y = 0, int ySize = 24);
	~TimeLineTrack();
	inline TRACK_TYPE trackType() const {return m_type;}
	inline void setTrackType(TRACK_TYPE type) {m_type = type;}
	inline QString trackTypeString() const {return trackTypeToString(m_type);}
	inline int trackId() const {return m_trackID;}
	inline void setTrackId(int id) {m_trackID = id;}
	inline bool isOverlay() const {return m_isOverlay;}
	void setOverlay(bool state);
	inline int yPos() const {return m_yPos;}
	inline void setYPos(int y) {m_yPos = y;}
	inline int  yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	inline void setYSize(int ySize) {m_ySize = ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}

	inline const QList<TimeLineItem*> & itemList() const {return m_itemList;}
	inline QList<TimeLineItem*> & itemList() {return m_itemList;}
	void appendTimeLineItem(TimeLineItem *item);
	bool removeTimeLineItem(TimeLineItem *item);
	int itemCount() const {return m_itemList.size();}
	TimeLineItem * itemAt(int idx) const;
	void deleteAllItems();
	void setTrackIdOfEachItem(int trackId, bool adjustYPosAlso = false);
	void setTrackDuration(int ms);

	void alignItemPositionsToTrack();

	bool hasOverlayContextMenu() const;
	QList<TimeLineContextMenuEntry> getOverlayContextMenu(const QPointF &scenePos);
	// bool execContextMenuCmd(const TimeLineContextMenuEntry *menuEntry);

	TimeLineTrack * trackAbove();
	TimeLineTrack * nextTrack();
	TimeLineTrack * prevTrack();

	static QString trackTypeToString(TRACK_TYPE type);

	friend class TimeLineWidget;
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

}

#endif // TIMELINETRACK_H

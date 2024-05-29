#ifndef TIMELINECLASSES_H
#define TIMELINECLASSES_H

#include <QColor>

namespace PS_TL {

enum TRACK_TYPE {
	TRACK_UNDEF,
	TRACK_ITEMS,
	TRACK_RULER,
	TRACK_AUDIO_ENV
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
	bool m_isOverlay;						///< Curve track is overlay over other track, not displayed under

public:
	TimeLineTrack(TimeLineWidget *timeline, TRACK_TYPE type, int id, int y = 0, int ySize = 24);
	~TimeLineTrack();
	inline TRACK_TYPE trackType() const {return m_type;}
	inline void setTrackType(TRACK_TYPE type) {m_type = type;}
	inline int trackId() const {return m_trackID;}
	inline void setTrackId(int id) {m_trackID = id;}
	inline bool isOverlay() const {return m_isOverlay;}
	void setOverlay(bool state);
	inline int yPos() const {return m_yPos;}
	inline void setYPos(int y) {m_yPos = y;}
	inline int yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	inline void setYSize(int ySize) {m_ySize = ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}

	inline const QList<TimeLineItem*> & itemList() const {return m_itemList;}
	inline QList<TimeLineItem*> & itemList() {return m_itemList;}
	void appendTimeLineItem(TimeLineItem *item);
	int itemCount() const {return m_itemList.size();}
	TimeLineItem * itemAt(int idx) const;
	void deleteAllItems();
	void setTrackIdOfEachItem(int trackId, bool adjustYPosAlso = false);
	void setTrackDuration(int ms);

	void alignItemPositionsToTrack();

	friend class TimeLineWidget;
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

}

#endif // TIMELINECLASSES_H

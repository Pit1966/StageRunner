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

class TimeLineItem;
class TimeLineBase;

class TimeLineTrack
{
public:
	qint64 trackBgColor	= -1;				///< invalid color

protected:
	TRACK_TYPE m_type;
	int m_trackID;
	int m_yPos;
	int m_ySize;
	QList<TimeLineBase*> m_itemList;		///< this list contains the timeline items in this timeline

public:
	TimeLineTrack(TRACK_TYPE type, int id, int y = 0, int ySize = 24);
	~TimeLineTrack();
	inline TRACK_TYPE trackType() const {return m_type;}
	inline void setTrackType(TRACK_TYPE type) {m_type = type;}
	inline int trackId() const {return m_trackID;}
	inline void setTrackId(int id) {m_trackID = id;}
	inline int yPos() const {return m_yPos;}
	inline void setYPos(int y) {m_yPos = y;}
	inline int yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	inline void setYSize(int ySize) {m_ySize = ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}

	inline const QList<TimeLineBase*> & itemList() const {return m_itemList;}
	inline QList<TimeLineBase*> & itemList() {return m_itemList;}
	void appendTimeLineItem(TimeLineBase *item);
	int itemCount() const {return m_itemList.size();}
	TimeLineBase * itemAt(int idx) const;

	friend class TimeLineWidget;
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

}

#endif // TIMELINECLASSES_H

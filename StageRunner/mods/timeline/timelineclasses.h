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

class TimeLineTrack
{
public:
	QColor trackBgColor;

private:
	TRACK_TYPE m_type;
	int m_trackID;
	int m_yPos;
	int m_ySize;
	QList<TimeLineItem*> m_itemList;		///< this list contains the timeline items in this timeline

public:
	TimeLineTrack(TRACK_TYPE type, int id, int y = 0, int size = 24);
	~TimeLineTrack();
	inline TRACK_TYPE trackType() const {return m_type;}
	inline int trackId() const {return m_trackID;}
	inline int yPos() const {return m_yPos;}
	inline int yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}

	inline const QList<TimeLineItem*> & itemList() const {return m_itemList;}
	inline QList<TimeLineItem*> & itemList() {return m_itemList;}
	void appendTimeLineItem(TimeLineItem *item);
	int itemCount() const {return m_itemList.size();}
	TimeLineItem * itemAt(int idx) const;

};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

}

#endif // TIMELINECLASSES_H

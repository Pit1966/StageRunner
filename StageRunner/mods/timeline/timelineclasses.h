#ifndef TIMELINECLASSES_H
#define TIMELINECLASSES_H

namespace PS_TL {

// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

class TimeLineTrack
{
public:
	int m_trackID;
	int m_yPos;
	int m_ySize;

public:
	TimeLineTrack(int id, int y = 0, int size = 24)
		: m_trackID(id)
		, m_yPos(y)
		, m_ySize(size)
	{}
	inline int trackId() const {return m_trackID;}
	inline int yPos() const {return m_yPos;}
	inline int yEndPos() const {return m_yPos + m_ySize;}
	inline int ySize() const {return m_ySize;}
	bool isInYRange(int y) const {return y >= m_yPos && y < yEndPos();}
};


// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

}

#endif // TIMELINECLASSES_H

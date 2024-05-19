#ifndef TIMELINEBASE_H
#define TIMELINEBASE_H

#include "timeline_defines.h"

#include <QGraphicsObject>
#include <QObject>

namespace PS_TL {

class TimeLineWidget;

class TimeLineBase : public QGraphicsObject
{
	Q_OBJECT

private:
	int m_positionMs			= 0;		///< position of item in milliseconds
	int m_durationMs			= 0;		///< length of item in milliseconds

protected:
	TimeLineWidget *m_timeline;

	QColor m_colorBG			= QColor(0x333333);

	// current item dimensions in pixels. Depends on the timeline scaling
	qreal m_xSize				= 200.0;
	qreal m_ySize				= 30.0;
	bool m_isTimePosValid		= false;	///< this is true, if m_timePosMs and m_timeLenMs are valid.
	bool m_isPixelPosValid		= true;		///< this is true, if pixel position information is correct
	bool m_wasPainted			= false;	///< becomes true, after first paint event

public:
	TimeLineBase(TimeLineWidget *timeline);

	int position() const {return m_positionMs;}
	void setPosition(int ms);
	int duration() const {return m_durationMs;}
	void setDuration(int ms);

	virtual void recalcPixelPos()	= 0;

signals:
	void timePositionChanged(int ms);
	void durationChanged(int ms);

};

} // namespace PS_TL

#endif // TIMELINEBASE_H

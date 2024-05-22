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
	// parent timeline
	TimeLineWidget *m_timeline;
	// base information
	int m_id					= 0;		///< not used yet
	int m_itemType				= TL_ITEM;
	int m_trackId				= 0;

	// user editable data
	QString m_label;
	QColor m_colorBG			= QColor(0x333333);
	QColor m_colorBorder		= QColor(0xeeeeee);

	// current item dimensions in pixels. Depends on the timeline scaling
	qreal m_xSize				= 200.0;
	qreal m_ySize				= 30.0;
	bool m_isTimePosValid		= false;	///< this is true, if m_timePosMs and m_timeLenMs are valid.
	bool m_isPixelPosValid		= true;		///< this is true, if pixel position information is correct
	bool m_wasPainted			= false;	///< becomes true, after first paint event
	bool m_isFirstInit			= false;

public:
	TimeLineBase(TimeLineWidget *timeline, int trackId);
	int type() const override {return m_itemType;}
	int trackID() const {return m_trackId;}
	const QString & label() const {return m_label;}
	void setLabel(const QString &label);
	void setBackgroundColor(const QColor &col);

	bool isFirstInitReady() const {return m_isFirstInit;}

	int position() const {return m_positionMs;}
	void setPosition(int ms);
	int duration() const {return m_durationMs;}
	void setDuration(int ms);

	qreal yPos() const;
	void setYPos(qreal yPixelPos);

	virtual void recalcPixelPos()	= 0;

signals:
	void timePositionChanged(int ms);
	void durationChanged(int ms);
	void labelChanged(const QString &txt);
};

} // namespace PS_TL

#endif // TIMELINEBASE_H

#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include "timeline_defines.h"
#include "timeline_classes.h"

#include <QGraphicsObject>
#include <QObject>

namespace PS_TL {

class TimeLineWidget;
class TimeLineTrack;


class TimeLineItem : public QGraphicsObject
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
	// temp track, found by trackID
	TimeLineTrack *m_myTrack	= nullptr;

	// user editable data
	QString m_label;
	uint m_colorBG				= 0x333333;
	uint m_colorBorder			= 0x3333ff;
	uint m_colorText			= 0xeeeeee;

	// current item dimensions in pixels. Depends on the timeline scaling
	qreal m_xSize				= 200.0;
	qreal m_ySize				= 30.0;
	bool m_isTimePosValid		= false;	///< this is true, if m_timePosMs and m_timeLenMs are valid.
	bool m_isPixelPosValid		= true;		///< this is true, if pixel position information is correct
	bool m_wasPainted			= false;	///< becomes true, after first paint event
	bool m_isFirstInitReady		= false;

public:
	TimeLineItem(TimeLineWidget *timeline, int trackId);
	int type() const override {return m_itemType;}
	int trackID() const {return m_trackId;}
	void setTrackID(int id) {m_trackId = id;}
	TimeLineTrack * timeLineTrack() const;
	const QString & label() const {return m_label;}
	void setLabel(const QString &label);
	void setBackgroundColor(const QColor &col);
	void setBorderColor(const QColor &col);

	bool isFirstInitReady() const {return m_isFirstInitReady;}

	int position() const {return m_positionMs;}
	void setPosition(int ms);
	int duration() const {return m_durationMs;}
	void setDuration(int ms);

	qreal yPos() const;
	void setYPos(qreal yPixelPos);
	qreal ySize() const;
	void setYSize(qreal ySize);

	bool isInsideScenePos(qreal sceneX, qreal sceneY);

	// functions must be implemented in derived classes
	virtual void recalcPixelPos()	= 0;
	// functions maybe implemented in derived classes in order to tweak controls
	/**
	 * @brief Implement in subclasses to react on hovering cursor.
	 * @param x qreal x-position in realation to item
	 * @param y qreal y-position in realation to item
	 * @return bool true, if the object accepted the event.
	 */
	virtual bool mouseHoverEvent(qreal x, qreal y);
	virtual int maxDuration() const {return 0;}
	virtual int fadeInTime() const {return 0;}
	virtual void setFadeInTime(int ms) {Q_UNUSED(ms);}
	virtual void setFadeOutTime(int ms) {Q_UNUSED(ms);}
	virtual int fadeOutTime() const {return 0;}
	virtual void setTrackDuration(int ms) {Q_UNUSED(ms);}
	virtual QString getConfigDat() const {return QString();}
	virtual bool setConfigDat(const QString &dat) {Q_UNUSED(dat); return true;}

	virtual bool hasOverLayContextMenu() const;
	virtual QList<TimeLineContextMenuEntry> getOverlayContextMenu(const QPointF &scenePos);
	// bool execContextMenuCmd(const TimeLineContextMenuEntry *menuEntry);

signals:
	void timePositionChanged(int ms);
	void durationChanged(int ms);
	void labelChanged(const QString &txt);
};

} // namespace PS_TL

#endif // TIMELINEITEM_H

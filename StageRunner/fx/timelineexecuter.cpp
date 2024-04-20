#include "timelineexecuter.h"

#include "appcontrol/appcentral.h"
#include "fx/fxitem.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "mods/timeline/timelineitem.h"

bool TimeLineExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) {
		return false;
	}

	qDebug() << "time" << runTime.elapsed() << "ms. execute obj no" << m_curObjPos + 1;

	FxTimeLineObj *next = findNextObj();
	if (!next)
		return false;			// no more active. We are ready

	setEventTargetTimeAbsolute(next->posMs);
	return true;
}

void TimeLineExecuter::processProgress()
{
	if (myState == EXEC_FINISH) {
		qDebug() << "finish timeline" << m_fxTimeLine->name();
		destroyLater();
	}
}

TimeLineExecuter::TimeLineExecuter(AppCentral &appCentral, FxTimeLineItem *timeline, FxItem *parentFx)
	: Executer(appCentral, timeline)
	, m_fxTimeLine(timeline)
{
	parentFxItem = parentFx;
	m_timelineValid = getTimeLineObjs(timeline);
	if (!m_timelineValid) {
		destroyLater();
	}
	else {
		setEventTargetTimeAbsolute(m_sortedTimeLineObjList.at(m_curObjPos)->posMs);
	}
}

TimeLineExecuter::~TimeLineExecuter()
{
	qDebug() << "destroyed timeline executer";
}

FxTimeLineObj *TimeLineExecuter::findNextObj()
{
	m_curObjPos++;
	if (m_curObjPos < m_sortedTimeLineObjList.size())
		return m_sortedTimeLineObjList.at(m_curObjPos);

	return nullptr;
}

/**
 * @brief Build a time sorted list of references to the timeline objects in the timeline
 * @param fx
 */
bool TimeLineExecuter::getTimeLineObjs(FxTimeLineItem *fx)
{
	if (!FxItem::exists(fx))
		return false;

	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		for (int i=0; i<fx->timeLineObjCount(t); i++) {
			FxTimeLineObj *o = fx->timeLineObjAt(t, i);
			if (!o) break;

			// insert object in list searching the correct time position
			int n = 0;
			while (n < m_sortedTimeLineObjList.size() && o->posMs > m_sortedTimeLineObjList.at(n)->posMs)
				n++;
			m_sortedTimeLineObjList.insert(n, o);
		}
	}

	if (m_sortedTimeLineObjList.isEmpty())
		return false; // nothing to execute

	return true;	// at least one timeline obj in list
}

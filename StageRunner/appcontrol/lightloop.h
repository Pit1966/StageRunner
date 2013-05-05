#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include "config.h"

#include <QElapsedTimer>
#include <QTimer>

class LightControl;
class FxList;
class FxSceneItem;
template <class T> class MutexQList;

class LightLoop : public QObject
{
	Q_OBJECT
public:
	enum STATUS {
		STAT_IDLE,
		STAT_RUNNING
	};

private:
	volatile STATUS loop_status;
	QElapsedTimer loop_time;
	QTimer loop_timer;
	qint32 loop_exec_target_time_ms;
	bool first_process_event_f;

	QByteArray dmxtout[MAX_DMX_UNIVERSE];

protected:
	MutexQList<const FxList*>*fxListsRef;
	LightControl *unitLightRef;


public:
	LightLoop(LightControl *unit_light);
	~LightLoop();
	void setFxListsRef(MutexQList<const FxList*>*listref);

private:
	void init();

signals:


public slots:
	void startProcessTimer();
	void stopProcessTimer();
	void processPendingEvents();
	void processFxSceneItem(const FxSceneItem * scene);

};

#endif // CONTROLLOOP_H

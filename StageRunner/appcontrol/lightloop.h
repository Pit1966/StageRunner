#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include "config.h"

#include <QElapsedTimer>
#include <QTimer>

class LightControl;
class FxList;
class FxSceneItem;

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
	qint64 loop_exec_target_time_ms;
	bool first_process_event_f;

	QByteArray dmxtout[MAX_DMX_UNIVERSE];

protected:
	LightControl & lightCtrlRef;

public:
	LightLoop(LightControl & unit_light);
	~LightLoop();

private:
	void init();

signals:
	void sceneStatusChanged(FxSceneItem *scene, quint32 stat);
	void sceneFadeProgressChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void sceneCueReady(FxSceneItem *scene);

public slots:
	void startProcessTimer();
	void stopProcessTimer();
	void processPendingEvents();
	bool processFxSceneItem(FxSceneItem *scene);

};

#endif // CONTROLLOOP_H

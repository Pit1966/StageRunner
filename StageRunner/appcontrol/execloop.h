#ifndef EXECLOOP_H
#define EXECLOOP_H

#include "config.h"

#include <QObject>
#include <QTimer>

class ExecCenter;
class FxControl;


class FxExecLoop : public QObject
{
	Q_OBJECT
public:
	enum STATUS {
		STAT_IDLE,
		STAT_RUNNING
	};

private:
	FxControl &fxContrlRef;

	volatile STATUS loop_status;
	QTimer loop_timer;

public:
	FxExecLoop(FxControl &unit_fx);

	void startLoopTimer();
	void stopLoopTimer();

private:
	void init();

signals:

private slots:
	void processPendingEvents();

};

#endif // EXECLOOP_H

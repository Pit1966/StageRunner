#include "execloop.h"
#include "log.h"
#include "toolclasses.h"
#include "execcenter.h"
#include "fxcontrol.h"
#include "executer.h"

#include <QObject>
#include <QEventLoop>

FxExecLoop::FxExecLoop(FxControl &unit_fx)
	: QObject()
	, fxContrlRef(unit_fx)
{
	init();
}

void FxExecLoop::startLoopTimer()
{
	loop_timer.setInterval(10);
	loop_timer.start();
	loop_status = STAT_RUNNING;
}

void FxExecLoop::stopLoopTimer()
{
	loop_status = STAT_IDLE;
	loop_timer.stop();
}

void FxExecLoop::init()
{
	loop_status = STAT_IDLE;

	connect(&loop_timer,SIGNAL(timeout()),this,SLOT(processPendingEvents()));
}

void FxExecLoop::processPendingEvents()
{
	static int cnt = 0;

	fxContrlRef.activeFxExecuters.lock();

	QMutableListIterator<Executer*> it(fxContrlRef.activeFxExecuters);
	while (it.hasNext()) {
		Executer *exec = it.next();
		switch (exec->state()) {
		case Executer::EXEC_PAUSED:
			exec->setTargetTimeToCurrent();
			break;
		case Executer::EXEC_RUNNING:
			if (exec->processTimeReached()) {
				bool active = exec->processExecuter();
				if (!active) {
					it.remove();
					exec->destroyLater();
				}
			}
		default:
			break;
		}
	}

	fxContrlRef.activeFxExecuters.unlock();
}

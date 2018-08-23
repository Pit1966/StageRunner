#include "execloopthreadinterface.h"
#include "execcenter.h"
#include "execloop.h"
#include "log.h"

ExecLoopThreadInterface::ExecLoopThreadInterface(FxControl &unit_fx)
	: QThread()
	, fxControlRef(unit_fx)
	, execLoop(nullptr)
{
	setObjectName("Exec Loop");
}

bool ExecLoopThreadInterface::startThread()
{
	if (!isRunning()) {
		if (!execLoop) {
			start();
			while (!execLoop) {;}
		}
	}

	return isRunning();
}

bool ExecLoopThreadInterface::stopThread()
{
	if (isRunning()) {
		quit();
		return wait(500);
	}

	return true;
}

void ExecLoopThreadInterface::run()
{
	execLoop = new FxExecLoop(fxControlRef);
	execLoop->startLoopTimer();
	exec();
	execLoop->stopLoopTimer();
	delete execLoop;
	execLoop = 0;
}


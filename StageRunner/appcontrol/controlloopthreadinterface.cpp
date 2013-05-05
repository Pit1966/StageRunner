#include "controlloopthreadinterface.h"
#include "controlloop.h"
#include "log.h"

#include <QDebug>

LightLoopThreadInterface::LightLoopThreadInterface(LightControl *unit_light)
	: QThread()
	, unitLightRef(unit_light)
{
	lightLoop = 0;
}

LightLoopThreadInterface::~LightLoopThreadInterface()
{
}

bool LightLoopThreadInterface::startThread(MutexQList<const FxList *> *listref)
{
	if (!isRunning()) {
		if (!lightLoop) {
			start();
			while (!lightLoop) ;;
			lightLoop->setFxListsRef(listref);
		}
	}

	return isRunning();
}

bool LightLoopThreadInterface::stopThread()
{
	if (isRunning()) {
		quit();
		return wait(500);
	}

	return true;
}

void LightLoopThreadInterface::run()
{
	lightLoop = new LightLoop(unitLightRef);
	lightLoop->startProcessTimer();
	exec();
	lightLoop->stopProcessTimer();
	delete lightLoop;
	lightLoop = 0;
}

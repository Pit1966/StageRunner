#include "lightloopthreadinterface.h"
#include "lightloop.h"
#include "log.h"

#include <QDebug>

LightLoopThreadInterface::LightLoopThreadInterface(LightControl &unit_light)
	: QThread()
	, lightControlRef(unit_light)
	, lightLoop(0)
{
	setObjectName("Light Loop");
}

LightLoopThreadInterface::~LightLoopThreadInterface()
{
}

bool LightLoopThreadInterface::startThread()
{
	if (!isRunning()) {
		if (!lightLoop) {
			start();
			while (!lightLoop)
				;;
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
	lightLoop = new LightLoop(lightControlRef);
	connect(lightLoop,SIGNAL(wantedDeleteFxScene(FxSceneItem*)),this,SIGNAL(wantedDeleteFxScene(FxSceneItem*)));
	lightLoop->startProcessTimer();
	exec();
	lightLoop->stopProcessTimer();
	delete lightLoop;
	lightLoop = 0;
}

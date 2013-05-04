#include "controlloopthreadinterface.h"
#include "controlloop.h"
#include "log.h"

#include <QDebug>

ControlLoopThreadInterface::ControlLoopThreadInterface(QObject *parent) :
	QThread(parent)
{
	worker = 0;
}

ControlLoopThreadInterface::~ControlLoopThreadInterface()
{
}

bool ControlLoopThreadInterface::startThread()
{
	if (!isRunning()) {
		if (!worker) {
			start();
		}
	}

	return isRunning();
}

bool ControlLoopThreadInterface::stopThread()
{
	if (isRunning()) {
		quit();
		return wait(500);
	}

	return true;
}

void ControlLoopThreadInterface::run()
{
	worker = new ControlLoop;
	worker->startProcessTimer();
	exec();
	worker->stopProcessTimer();
	worker->deleteLater();
	worker = 0;
}

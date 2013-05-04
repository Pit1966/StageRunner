#include "controlloop.h"
#include "config.h"
#include "log.h"

#include <QDebug>
#include <QEventLoop>
#include <QThread>

ControlLoop::ControlLoop(QObject *parent) :
	QObject(parent)
{
	init();
}

ControlLoop::~ControlLoop()
{
}


void ControlLoop::init()
{
	loop_status = STAT_IDLE;
	first_process_event_f = true;

	connect(&loop_timer,SIGNAL(timeout()),this,SLOT(processPendingEvents()));
}

void ControlLoop::startProcessTimer()
{
	loop_timer.setInterval(10);
	loop_timer.start();
}

void ControlLoop::stopProcessTimer()
{
	loop_timer.stop();
}

void ControlLoop::processPendingEvents()
{
	if (first_process_event_f) {
		loop_time.start();
		loop_exec_target_time_ms = LIGHT_LOOP_INTERVAL_MS;
		first_process_event_f = false;
	}

	if (loop_time.elapsed() < loop_exec_target_time_ms) return;
	loop_exec_target_time_ms += LIGHT_LOOP_INTERVAL_MS;

	qDebug() << "ControlLoop: Time elapsed:" << loop_time.elapsed();
}


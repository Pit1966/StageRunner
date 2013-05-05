#include "controlloop.h"
#include "log.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "toolclasses.h"
#include "lightcontrol.h"
#include "dmxchannel.h"


#include <QDebug>
#include <QEventLoop>
#include <QThread>

LightLoop::LightLoop(LightControl *unit_light)
	: QObject()
	, unitLightRef(unit_light)
{
	init();
}

LightLoop::~LightLoop()
{
}

void LightLoop::setFxListsRef(MutexQList<const FxList *> *listref)
{
	fxListsRef = listref;
}

void LightLoop::init()
{
	fxListsRef = 0;
	loop_status = STAT_IDLE;
	first_process_event_f = true;
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		dmxtout[t].resize(512);
	}
	connect(&loop_timer,SIGNAL(timeout()),this,SLOT(processPendingEvents()));
}

void LightLoop::startProcessTimer()
{
	loop_timer.setInterval(10);
	loop_timer.start();
}

void LightLoop::stopProcessTimer()
{
	loop_timer.stop();
}

void LightLoop::processPendingEvents()
{
	if (first_process_event_f) {
		loop_time.start();
		loop_exec_target_time_ms = LIGHT_LOOP_INTERVAL_MS;
		first_process_event_f = false;
	}

	if (loop_time.elapsed() < loop_exec_target_time_ms) return;
	loop_exec_target_time_ms += LIGHT_LOOP_INTERVAL_MS;

	// qDebug() << "ControlLoop: Time elapsed:" << loop_time.elapsed();

	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		memset(dmxtout[t].data(),0,512);
	}

	if (fxListsRef) {
		fxListsRef->lock();
		for (int list=0; list<fxListsRef->size(); list++) {
			const FxList *fxlist = fxListsRef->at(list);
			for (int item=0; item<fxlist->size(); item++) {
				FxItem *fxitem = fxlist->at(item);
				if (fxitem->fxType() == FX_SCENE) {
					processFxSceneItem(static_cast<const FxSceneItem*>(fxitem));

				}
			}
		}
		fxListsRef->unlock();
	}

	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		for (int chan=0; chan<512; chan++) {
			if (dmxtout[t].at(chan) != unitLightRef->dmxOutputValues[t].at(chan)) {
				unitLightRef->dmxOutputChanged[t] = true;
			}
			unitLightRef->dmxOutputValues[t][chan] = dmxtout[t].at(chan);
		}
	}

	unitLightRef->sendChangedDmxData();
}

void LightLoop::processFxSceneItem(const FxSceneItem *scene)
{
	for (int t=0; t<scene->tubeCount(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		int channel = tube->dmxChannel;
		int universe = tube->dmxUniverse;
		tube->dmxValue = tube->curValue * 255 / tube->targetFullValue;
		if (tube->dmxValue > quint16(dmxtout[universe].at(channel)) ) {
			if (t==2) qDebug() << tube->dmxValue;
			dmxtout[universe][channel] = tube->dmxValue;
		}
	}
}


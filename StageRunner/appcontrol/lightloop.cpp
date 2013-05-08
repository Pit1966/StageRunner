#include "lightloop.h"
#include "log.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "toolclasses.h"
#include "lightcontrol.h"
#include "dmxchannel.h"
#include "toolclasses.h"


#include <QDebug>
#include <QEventLoop>
#include <QThread>

LightLoop::LightLoop(LightControl &unit_light)
	: QObject()
	, lightCtrlRef(unit_light)
{
	init();
}

LightLoop::~LightLoop()
{
}

void LightLoop::init()
{
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
	// It seems to be the time for an update of DMX output and other light (scene) tasks
	// First set the target time the next update should be done
	loop_exec_target_time_ms += LIGHT_LOOP_INTERVAL_MS;

	// qDebug() << "ControlLoop: Time elapsed:" << loop_time.elapsed();

	// Clear the temporary dmx output channel values. We will search in every scene
	// for active channels later and we will fill in the channel values here by highest takes precidence (HTP)
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		memset(dmxtout[t].data(),0,512);
	}

	QList<FxSceneItem*>inactive_scenes;

	// Lets have a look onto the list with active marked scenes
	MutexQHash<int, FxSceneItem*> & scenes = lightCtrlRef.activeScenes;
	scenes.readLock();
	// Get dmx channel output for every scene in the list
	foreach (FxSceneItem * sceneitem, scenes) {
		// Fill channel data into temp dmx data and determine if scene is still active
		if (!processFxSceneItem(sceneitem)) {
			inactive_scenes.append(sceneitem);
		}
		if (sceneitem->statusHasChanged()) {
			emit sceneStatusChanged(sceneitem,sceneitem->status());
		}
	}
//	QHashIterator<int,const FxSceneItem*> it(scenes);
//	while (it.hasNext()) {
//		it.next();
//		processFxSceneItem(it.value());
//	}
	scenes.unlock();

	// For the dmx channel data array should contain valid dmx data for stage output
	// Checked if the output has changed and give it to the output modules if it has changed
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		for (int chan=0; chan<512; chan++) {
			if (dmxtout[t].at(chan) != lightCtrlRef.dmxOutputValues[t].at(chan)) {
				lightCtrlRef.dmxOutputChanged[t] = true;
			}
			lightCtrlRef.dmxOutputValues[t][chan] = dmxtout[t].at(chan);
		}
	}

	lightCtrlRef.sendChangedDmxData();

	foreach (FxSceneItem *sceneitem, inactive_scenes) {
		lightCtrlRef.setSceneIdle(sceneitem);
	}
}

bool LightLoop::processFxSceneItem(FxSceneItem *scene)
{
	// Now call the function that processes all active fades. If this functions returns
	// false no channel is active anymore
	scene->loopFunction();

	for (int t=0; t<scene->tubeCount(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		int channel = tube->dmxChannel;
		int universe = tube->dmxUniverse;
		tube->dmxValue = tube->curValue * 255 / tube->targetFullValue;
		if (tube->dmxValue > quint16(dmxtout[universe].at(channel)) ) {
			dmxtout[universe][channel] = tube->dmxValue;
		}
	}

	return scene->status() & (SCENE_LIVE | SCENE_STAGE | SCENE_ACTIVE);
}


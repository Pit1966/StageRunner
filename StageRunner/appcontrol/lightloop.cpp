//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "lightloop.h"
#include "system/log.h"
#include "system/lightcontrol.h"
#include "system/dmxchannel.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"
#include "fx/fxsceneitem.h"
#include "tool/toolclasses.h"

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
	loop_status = STAT_RUNNING;
}

void LightLoop::stopProcessTimer()
{
	loop_status = STAT_IDLE;
	loop_timer.stop();
}

void LightLoop::processPendingEvents()
{
	if (first_process_event_f) {
		loop_time.start();
		loop_exec_target_time_ms = LIGHT_LOOP_INTERVAL_MS;
		first_process_event_f = false;
	}

	if (loop_time.elapsed() < loop_exec_target_time_ms)
		return;
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
	MutexQHash<int, QPointer<FxSceneItem>> &scenes = lightCtrlRef.activeScenes;
	scenes.readLock();
	bool removeNulls = false;		// if we found nullptr scenes or scenes which look faulty, we will remove it after loop
	// add scanner scenes
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (!scenes.contains(lightCtrlRef.hiddenScannerScenes[t]->id())) {
			scenes.insert(lightCtrlRef.hiddenScannerScenes[t]->id(),lightCtrlRef.hiddenScannerScenes[t]);
		}
	}
	// add static scenes
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		for (int i=0; i<MAX_STATIC_SCENES; i++) {
			if (lightCtrlRef.staticScenes[t][i]) {
				if (!scenes.contains(lightCtrlRef.staticScenes[t][i]->id())) {
					scenes.insert(lightCtrlRef.staticScenes[t][i]->id(),lightCtrlRef.staticScenes[t][i]);
				}
			}
		}
	}

	// Get dmx channel output for every scene in the list
	for (FxSceneItem *sceneitem : std::as_const(scenes)) {
		if (sceneitem == nullptr) {
			LOGTEXT(tr("<font color=red>Light control warning!</font> found deleted scene in active scene list"));
			removeNulls = true;
			continue;
		}

		// Fill channel data into temp dmx data and determine if scene is still active
		if (!processFxSceneItem(sceneitem)) {
			if (debug > 1 && !sceneitem->name().startsWith("ScanMove"))
				DEBUGTEXT("Scene %s is idle now",sceneitem->name().toLocal8Bit().data());
			inactive_scenes.append(sceneitem);
		}
		if (sceneitem->getClearStatusHasChanged()) {
			emit sceneStatusChanged(sceneitem,sceneitem->status());
		}
		if (sceneitem->getClearActiveHasChanged()) {
			if (!sceneitem->isActive())
				emit sceneCueReady(sceneitem);
		}

	}

	if (removeNulls) {
		QMutableHashIterator<int, QPointer<FxSceneItem>>it(lightCtrlRef.activeScenes);
		while (it.hasNext()) {
			it.next();
			if (it.value() == nullptr)
				it.remove();
		}
	}
	scenes.unlock();

	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		for (int chan=0; chan<512; chan++) {
			if (lightCtrlRef.hiddenScannerScenes[t]->tube(chan)->dmxType >= DMX_POSITION_PAN) {
	//			dmxtout[t][chan] = lightCtrlRef.hiddenScannerScenes[t]->tube(chan)->dmxValue;
			}
		}
	}

	// The dmx channel data array should contain valid dmx data for stage output
	// Check if the output has changed and forward it to the output modules if it has changed
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		for (int chan=0; chan<512; chan++) {
			if (dmxtout[t].at(chan) != lightCtrlRef.dmxOutputValues[t].at(chan)) {
				lightCtrlRef.dmxOutputChanged[t] = true;
				// qDebug() << "changed dmx uni" << t+1 << "chan" << chan+1 << "to" << QString::number(quint8(dmxtout[t].at(chan)));
			}
			/// @todo move into condition block above?
			lightCtrlRef.dmxOutputValues[t][chan] = dmxtout[t].at(chan);
		}
	}

	lightCtrlRef.sendChangedDmxData();

	// Now check the list of inactive scenes. This removes NULL level scenes from active scenes list
	foreach (FxSceneItem *sceneitem, inactive_scenes) {
		lightCtrlRef.setSceneIdle(sceneitem);

		// This is for special scenes, which are temporary
		if (sceneitem->deleteOnFinished() && sceneitem->isTempCopy())
			emit wantedDeleteFxScene(sceneitem);
	}
}


/**
 * @brief Process all Tubes (Channels) of a scene and calculate the corresponding output to DMX universe
 * @param scene
 * @return true, if Scene is visible.
 */
bool LightLoop::processFxSceneItem(FxSceneItem *scene)
{
	// Now call the function that processes all active fades. If this functions returns
	// false no channel is active anymore
	/*bool active = */scene->loopFunction();

	// copy to real dmx arrays
	for (int t=0; t<scene->tubeCount(); t++) {
		DmxChannel *tube = scene->tubes.at(t);
		int channel = tube->dmxChannel;
		int universe = tube->dmxUniverse;
		// Attention, this maybe null!
		DmxChannel *gTube = lightCtrlRef.globalDmxChannel(universe, channel);

		int value = 0;
		for (int i=0; i<MIX_LINES; i++) {
			if (tube->curValue[i] > value)
				value = tube->curValue[i];
		}

		// tube->dmxValue = value * 255 / tube->targetFullValue;
		int dmxval;
		if (tube->scalerNumerator > 1 || tube->scalerDenominator > 1 || gTube == nullptr) {
			dmxval = value * 255 * tube->scalerNumerator / (tube->targetFullValue * tube->scalerDenominator);
		} else {
			dmxval = value * 255 * gTube->scalerNumerator / (tube->targetFullValue * gTube->scalerDenominator);
		}
		tube->dmxValue = std::clamp(dmxval, 0, 255);

		// if (tube->dmxValue > 0 && tube->dmxChannel == 17) {
		// 	qDebug() << scene->name() << "channel:" << tube->dmxChannel << "dmx:" << tube->dmxValue << "universe:" << tube->dmxUniverse;
		// }
		if (tube->dmxValue > quint8(dmxtout[universe].at(channel)) ) {
			dmxtout[universe][channel] = tube->dmxValue;
		}
	}

	DmxChannel *prog_chan = scene->sceneMaster;
	if (prog_chan->curValueChanged[MIX_EXTERN] || prog_chan->curValueChanged[MIX_INTERN]) {
		emit sceneFadeProgressChanged(scene, prog_chan->curValue[MIX_INTERN], prog_chan->curValue[MIX_EXTERN]);
		prog_chan->curValueChanged[MIX_INTERN] = false;
		prog_chan->curValueChanged[MIX_EXTERN] = false;
	}

	return scene->isVisible();
}


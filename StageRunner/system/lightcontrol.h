//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include "config.h"
#include "toolclasses.h"

#include <QObject>
#include <QByteArray>


class AppCentral;
class LightLoopThreadInterface;
class FxList;
class QLCIOPlugin;
class FxSceneItem;
class FxItem;
class FxSceneItem;
class FxSeqItem;


class LightControl : public QObject
{
	Q_OBJECT

public:
	AppCentral &myApp;
	LightLoopThreadInterface *lightLoopInterface;
	MutexQList<const FxList*>lightFxLists;
	MutexQHash<int,FxSceneItem*>activeScenes;
	QByteArray dmxOutputValues[MAX_DMX_UNIVERSE];
	QByteArray dmxInputValues[MAX_DMX_UNIVERSE];
	bool dmxOutputChanged[MAX_DMX_UNIVERSE];
	bool dmxInputChanged[MAX_DMX_UNIVERSE];
	FxSceneItem *hiddenScannerScenes[MAX_DMX_UNIVERSE];

public:
	LightControl(AppCentral &app_central);
	~LightControl();

	bool setLightLoopEnabled(bool state);
	bool addFxListToControlLoop(const FxList *list);
	bool sendChangedDmxData();
	bool startFxSceneSimple(FxSceneItem *scene);
	bool startFxScene(FxSceneItem *scene);
	bool stopFxScene(FxSceneItem *scene);
	bool setSceneActive(FxSceneItem *scene);
	bool setSceneIdle(FxSceneItem * scene);
	qint32 black(qint32 time_ms);
	qint32 blackFxScene(FxSceneItem *scene, qint32 time_ms);
	qint32 blackFxSequence(FxSeqItem *seq, qint32 time_ms);
	qint32 blackFxItem(FxItem *fx, qint32 time_ms);

	bool fillSceneFromInputUniverses(FxSceneItem *scene, int *feedbackActiveChannels = nullptr);

	bool setYadiInOutMergeMode(quint32 input, quint32 mode);
	bool setYadiInOutMergeMode(quint32 mode);

private:
	void init();

signals:
	void sceneChanged(FxSceneItem *scene);
	void sceneFadeChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void sceneCueReady(FxSceneItem *scene);
	void outputUniverseChanged(int universe, const QByteArray &dmxValues);
	void inputUniverseChanged(int universe, const QByteArray &dmxValues);
	void audioSlotVolChanged(int slot, int value);

public slots:
	void startFxSceneExecuter(FxSceneItem *scene);
	void onSceneStatusChanged(FxSceneItem *scene, quint32 status);
	void onSceneFadeProgressChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void onSceneCueReady(FxSceneItem *scene);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);

};

#endif // LIGHTCONTROL_H

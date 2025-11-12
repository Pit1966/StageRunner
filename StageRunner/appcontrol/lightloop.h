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

#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include "config.h"

#include <QElapsedTimer>
#include <QTimer>

class LightControl;
class FxList;
class FxSceneItem;
class DmxChannel;

class LightLoop : public QObject
{
	Q_OBJECT
public:
	enum STATUS {
		STAT_IDLE,
		STAT_RUNNING
	};

private:
	volatile STATUS loop_status;
	QElapsedTimer loop_time;
	QTimer loop_timer;
	qint64 loop_exec_target_time_ms;
	bool first_process_event_f;

	// Temporary DMX output data, cleared and filled in every processPendingEvents() iteration
	QByteArray dmxtout[MAX_DMX_UNIVERSE];

protected:
	LightControl & lightCtrlRef;

public:
	LightLoop(LightControl & unit_light);
	~LightLoop();

private:
	void init();

signals:
	void sceneStatusChanged(FxSceneItem *scene, quint32 stat);
	void sceneFadeProgressChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void sceneCueReady(FxSceneItem *scene);
	void wantedDeleteFxScene(FxSceneItem *scene);

public slots:
	void startProcessTimer();
	void stopProcessTimer();
	void processPendingEvents();
	bool processFxSceneItem(FxSceneItem *scene);

};

#endif // CONTROLLOOP_H

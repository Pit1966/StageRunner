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

#ifndef EXECCENTER_H
#define EXECCENTER_H

#include <QObject>
#include <QTimer>

#include "toolclasses.h"

class AppCentral;
class AudioControl;
class LightControl;
class FxItem;
class Executer;
class FxListExecuter;
class FxList;
class FxSceneItem;
class FxScriptItem;
class FxTimeLineItem;
class SceneExecuter;
class ScriptExecuter;
class TimeLineExecuter;

class ExecCenter : public QObject
{
	Q_OBJECT
private:
	AppCentral &myApp;

	MutexQList<Executer*>executerList;
	MutexQList<Executer*>removeQueue;
	QTimer remove_timer;

public:
	ExecCenter(AppCentral &app_central);
	~ExecCenter();

	Executer * findExecuter(const FxItem *fx);
	bool exists(Executer *exec);
	bool useExecuter(Executer *exec);
	bool freeExecuter(Executer *exec);

	inline MutexQList<Executer*> & executerListRef() {return executerList;}
	MutexQList<Executer*> & lockAndGetExecuterList();
	void unlockExecuterList();

	FxListExecuter * newFxListExecuter(FxList *fxlist, FxItem *fxitem);
	FxListExecuter * getCreateFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(FxList *fxlist);
	FxListExecuter * findFxListExecuter(const FxItem *fx);
	SceneExecuter * newSceneExecuter(FxSceneItem *scene, FxItem *parentFx);
	ScriptExecuter * newScriptExecuter(FxScriptItem *script, FxItem *parentFx);
	ScriptExecuter * findScriptExecuter(const FxItem *fx);
	TimeLineExecuter * newTimeLineExecuter(FxTimeLineItem *timeline, FxItem *parentFx);
	TimeLineExecuter * findTimeLineExecuter(const FxItem *fx);

	void queueRemove(Executer *exec);

signals:
	void executerCreated(Executer *);
	void executerDeleted(Executer *);
	void executerChanged(Executer *);

public slots:
	void deleteExecuter(Executer *exec);

	// remote
	bool executeScriptCmd(const QString &cmd);

private slots:
	void test_remove_queue();
	void on_executer_changed(Executer *exec);
};

#endif // EXECCENTER_H

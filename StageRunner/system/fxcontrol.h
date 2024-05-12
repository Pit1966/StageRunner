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

#ifndef FXCONTROL_H
#define FXCONTROL_H

#include "config.h"
#include "toolclasses.h"

#include <QObject>


class AppCentral;
class FxSeqItem;
class FxSceneItem;
class FxAudioItem;
class FxPlayListItem;
class FxScriptItem;
class FxTimeLineItem;
class FxExecLoop;
class ExecLoopThreadInterface;
class Executer;
class FxListExecuter;
class ScriptExecuter;
class TimeLineExecuter;
class ExecCenter;

class FxControl : public QObject
{
	Q_OBJECT
public:
	AppCentral &myApp;
	ExecCenter &execCenter;
	ExecLoopThreadInterface *execLoopInterface;

public:
	FxControl(AppCentral &appCentral, ExecCenter &exec_center);
	~FxControl();
	bool setExecLoopEnabled(bool state);

	FxListExecuter * startFxSequence(FxSeqItem *fxseq);
	bool stopFxSequence(FxSeqItem *fxseq);
	int stopAllFxSequences();
	bool pauseFxPlaylist(FxPlayListItem *fxplay);
	bool stopFxPlayList(FxPlayListItem *fxplay);
	int pauseAllFxPlaylist();
	int stopAllFxPlaylists();

	ScriptExecuter *startFxScript(FxScriptItem *fxscript);

	void handleDmxInputScriptEvent(FxScriptItem *fxscript, uchar dmxVal);
	bool stopFxScript(FxScriptItem *fxscript);
	int stopAllFxScripts();

	TimeLineExecuter *startFxTimeLine(FxTimeLineItem *fxtimeline, int atMs = 0);
	bool stopFxTimeLine(FxTimeLineItem *fxtimeline);
	int stopAllTimeLines();


private:
	ScriptExecuter * _startFxScript(FxScriptItem *fxscript);


public slots:
	FxListExecuter * startFxAudioPlayList(FxPlayListItem *fxplay);
	FxListExecuter * continueFxAudioPlayList(FxPlayListItem *fxplay, FxAudioItem *fxaudio);

signals:
	void executerChanged(Executer *exec);

public slots:

};

#endif // FXCONTROL_H

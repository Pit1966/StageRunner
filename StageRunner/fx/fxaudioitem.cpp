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

#include "fxaudioitem.h"
#include "../config.h"
#include "system/audiooutput/audiofileinfo.h"

#include <QFileInfo>
#ifdef IS_QT5
#  include <QMediaPlayer>
#endif

using namespace AUDIO;

QStringList FxAudioItem::attachedCmdStrings = QStringList()
		<< QObject::tr("None")
		<< QObject::tr("Fade out all audio")
		<< QObject::tr("Stop all audio")
		<< QObject::tr("Stop/BLACK video")
		<< QObject::tr("Start FX")
		<< QObject::tr("Stop all SEQ & SCRIPTS")
		<< QObject::tr("Set MASTER VOLUME")
		<< QString("DEVELOPER ADD TEXT!");


FxAudioItem::FxAudioItem(FxList *fxList)
	: FxItem(fxList)
{
	init();
}

FxAudioItem::FxAudioItem(const FxAudioItem &o)
	: FxItem(o.myParentFxList)
{
	init();
	cloneFrom(o);
}

FxAudioItem::FxAudioItem(const QString &path, FxList *fxList)
	: FxItem(fxList)
{
	init();
	QFileInfo fi(path);
	myPath = path;
	myFile = fi.fileName();
	myName = fi.completeBaseName();
}

/**
 * @brief Initialize the FxAudioItem for executing it in a sequence
 *
 *  This function is called typically by a Executer on setting the current Fx
 */
void FxAudioItem::initForSequence()
{
	mySeqStatus = AUDIO_OFF;
}

void FxAudioItem::resetFx()
{
	mySeekPosition = 0;
	if (audioDuration > 0) {
		mySeekPosPerMille = 0;
	} else {
		mySeekPosPerMille = -1;
	}
	mySeqStatus = AUDIO_OFF;
}

qint32 FxAudioItem::durationHint() const
{
	AudioFileInfo ai(filePath());
	bool ok = ai.inspectFile();

	if (ok)
		return ai.getDurationMs();

	return 25000;
}

void FxAudioItem::setSeekPosition(qint64 posMs)
{
	mySeekPosition = posMs;
	if (audioDuration) {
		mySeekPosPerMille = posMs * 1000 / audioDuration;
	} else {
		mySeekPosPerMille = -1;
	}
}

bool FxAudioItem::checkForVideoClip()
{
	const QString &suf = QFileInfo(filePath()).suffix().toLower();
	if (suf == "mkv"
			|| suf == "avi"
			|| suf == "mpg"
			|| suf == "mp4"
			|| suf == "m4v"
			|| suf == "mov") {
		isFxClip = true;
		isPicClip = false;
		qDebug() << fileName() << " is video file";
	}
	else if (suf == "jpg"
			 || suf == "png") {
		isFxClip = true;
		isPicClip = true;
		qDebug() << fileName() << " is picture file";
	}
	else {
		isFxClip = false;
		isPicClip = false;
	}

	return isFxClip;
}


void FxAudioItem::init()
{
	startSlot = -1;
	startInProgress = false;
	isDmxStarted = false;
	isFxClip = false;
	isPicClip = false;
	mySeekPosition = 0;
	mySeekPosPerMille = -1;
	myFxType = FX_AUDIO;
	mySeqStatus = AUDIO_OFF;
	myclass = PrefVarCore::FX_AUDIO_ITEM;

	tmpMasterVolAtStart = 0;

	addExistingVar(initialVolume,"InitialVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(currentVolume,"CurrentVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(panning,"Panning",0, MAX_PAN, 0);
	addExistingVar(audioDuration,"DurationMS");
	addExistingVar(initialSeekPos,"InitialSeekPos");
	addExistingVar(stopAtSeekPos,"StopAtSeekPos");
	addExistingVar(loopTimes,"LoopTimes");
	addExistingVar(playBackSlot,"PlayBackSlot");
	addExistingVar(attachedStartCmd,"AttachedStartCmd");
	addExistingVar(attachedStopCmd,"AttachedStopCmd");
	addExistingVar(attachedStartPara1,"AttachedStartPara1");
	addExistingVar(attachedStartPara2,"AttachedStartPara2");
	addExistingVar(attachedStopPara1,"AttachedStopPara1");
	addExistingVar(attachedStopPara2,"AttachedStopPara2");
	addExistingVar(blackAtVideoEnd,"BlackAtVideoEnd");
	// qDebug("init audio fx with id: %d",myId);
}

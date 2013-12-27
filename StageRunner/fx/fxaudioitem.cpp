#include "fxaudioitem.h"
#include "../config.h"

#include <QFileInfo>

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

void FxAudioItem::setFilePath(const QString &path)
{
	QFileInfo fi(path);
	myPath = path;
	myFile = fi.fileName();
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

void FxAudioItem::setSeekPosition(qint64 posMs)
{
	mySeekPosition = posMs;
	if (audioDuration) {
		mySeekPosPerMille = posMs * 1000 / audioDuration;
	} else {
		mySeekPosPerMille = -1;
	}
}


void FxAudioItem::init()
{
	mySeekPosition = 0;
	mySeekPosPerMille = -1;
	myFxType = FX_AUDIO;
	mySeqStatus = AUDIO_OFF;
	myclass = PrefVarCore::FX_AUDIO_ITEM;

	addExistingVar(initialVolume,"InitialVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(currentVolume,"CurrentVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(audioDuration,"DurationMS");
	addExistingVar(loopTimes,"LoopTimes");
	// qDebug("init audio fx with id: %d",myId);
}

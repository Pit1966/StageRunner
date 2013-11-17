#include "fxaudioitem.h"
#include "../config.h"

#include <QFileInfo>

FxAudioItem::FxAudioItem()
	: FxItem()
{
	init();
}

FxAudioItem::FxAudioItem(const FxAudioItem &o)
	: FxItem()
{
	init();
	cloneFrom(o);
}

FxAudioItem::FxAudioItem(const QString &path)
	: FxItem()
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

qint32 FxAudioItem::fadeInTime()
{
	return 0;
}

void FxAudioItem::setFadeInTime(qint32)
{

}

qint32 FxAudioItem::fadeOutTime()
{
	return 0;
}

void FxAudioItem::setFadeOutTime(qint32)
{
}

void FxAudioItem::init()
{

	myFxType = FX_AUDIO;
	myclass = PrefVarCore::FX_AUDIO_ITEM;

	addExistingVar(initialVolume,"InitialVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(currentVolume,"CurrentVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(audioDuration,"DurationMS");
	addExistingVar(loopTimes,"LoopTimes");
	// qDebug("init audio fx with id: %d",myId);
}

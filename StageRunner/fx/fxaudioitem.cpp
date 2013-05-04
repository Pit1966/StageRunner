#include "fxaudioitem.h"
#include "../config.h"

#include <QFileInfo>

FxAudioItem::FxAudioItem()
	: FxItem()
{
	init();
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

void FxAudioItem::init()
{
	myFxType = FX_AUDIO;
	addExistingVar(initialVolume,"InitialVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(currentVolume,"CurrentVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	qDebug("init audio fx with id: %d",myId);
}

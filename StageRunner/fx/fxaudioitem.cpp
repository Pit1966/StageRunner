#include "fxaudioitem.h"

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

void FxAudioItem::init()
{
	myType = FX_AUDIO;
	qDebug("init audio fx with id: %d",myId);
}

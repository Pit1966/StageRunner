#ifndef VIDEOCONTROL_H
#define VIDEOCONTROL_H

#include <QObject>

#include "commandsystem.h"

class AppCentral;
class FxClipItem;
class QMediaPlayer;
class QMediaPlaylist;
class PsVideoWidget;

class VideoControl : public QObject
{
	Q_OBJECT
private:
	AppCentral &myApp;

public:
	VideoControl(AppCentral &app_central);

	bool startFxClip(FxClipItem *fxc);

	void videoBlack(qint32 time_ms);

signals:

public slots:

};

#endif // VIDEOCONTROL_H

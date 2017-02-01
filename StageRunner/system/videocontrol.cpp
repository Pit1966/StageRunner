#include "videocontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "fxclipitem.h"
#include "audiocontrol.h"
#include "videoplayer.h"
#include "customwidget/psvideowidget.h"

VideoControl::VideoControl(AppCentral &app_central)
	: QObject()
	, myApp(app_central)
{
}

bool VideoControl::startFxClip(FxClipItem *fxc)
{
	qDebug() << "Start FxAudio as FxClip"<< fxc->name();

	return myApp.unitAudio->startFxClip(fxc);
}

void VideoControl::videoBlack(qint32 time_ms)
{
	Q_UNUSED(time_ms)

	if (!myApp.unitAudio->isValid()) return;

	VideoPlayer *vp = myApp.unitAudio->videoPlayer();
	vp->stop();

	myApp.unitAudio->videoWidget()->update();

}

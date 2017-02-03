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

bool VideoControl::startFxClipById(qint32 id)
{
	FxItem *fx = FxItem::findFxById(id);
	if (!fx) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("FX Id does not exist in start command!"));
		return false;
	}
	FxClipItem *fxclip = dynamic_cast<FxClipItem*>(fx);
	if (!fxclip) {
		POPUPERRORMSG(Q_FUNC_INFO,tr("FX Id %1 does not represent a FxClip in start command\n"
									 "It is fx with name: %2").arg(id).arg(fx->name()));
		return false;
	}

	return startFxClip(fxclip);
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

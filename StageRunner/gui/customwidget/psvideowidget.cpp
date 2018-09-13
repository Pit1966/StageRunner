#include <QSettings>
#include <QDebug>
#include <QMouseEvent>

#include "psvideowidget.h"
#include "videoplayer.h"
#include "configrev.h"

PsVideoWidget::PsVideoWidget(QWidget *parent)
	: QVideoWidget(parent)
	, m_myPlayer(nullptr)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAutoFillBackground(false);

	setPrefsSettings();
}

void PsVideoWidget::setPrefsSettings()
{
	QSettings set(QSETFORMAT,APPNAME);
	if (set.contains("VideoWinEnabled")) {
		set.beginGroup("GuiSettings");
		restoreGeometry(set.value("VideoWinGeometry").toByteArray());
		move(set.value("VideoWinPos").toPoint());
		setFullScreen(set.value("VideoWinIsFullscreen").toBool());
		set.endGroup();

		if (set.value("VideoWinEnabled").toBool() == false)
			close();
	}
}

void PsVideoWidget::saveCurrentStateToPrefs()
{
	QSettings set(QSETFORMAT,APPNAME);
	set.setValue("VideoWinEnabled",!isHidden());
	set.beginGroup("GuiSettings");
	set.setValue("VideoWinGeometry",saveGeometry());
	set.setValue("VideoWinPos",pos());
	set.setValue("VideoWinIsFullscreen",isFullScreen());
	set.endGroup();

}

void PsVideoWidget::setVideoPlayer(VideoPlayer *vidplay)
{
	m_myPlayer = vidplay;
}

void PsVideoWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	setFullScreen(!isFullScreen());
}

void PsVideoWidget::mousePressEvent(QMouseEvent *ev)
{
	Q_UNUSED(ev)
}

void PsVideoWidget::closeEvent(QCloseEvent *event)
{
	saveCurrentStateToPrefs();

#if QT_VERSION >= 0x050600
	hide();
	event->ignore();
	return;
#else
	QVideoWidget::closeEvent(event);
#endif
}

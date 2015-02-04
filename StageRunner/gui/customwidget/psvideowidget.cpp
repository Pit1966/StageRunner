#include <QSettings>
#include <QDebug>

#include "psvideowidget.h"

PsVideoWidget::PsVideoWidget(QWidget *parent)
	: QVideoWidget(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);

	QSettings set;
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

void PsVideoWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	setFullScreen(!isFullScreen());
}

void PsVideoWidget::closeEvent(QCloseEvent *event)
{
	QSettings set;
	set.setValue("VideoWinEnabled",!isHidden());
	set.beginGroup("GuiSettings");
	set.setValue("VideoWinGeometry",saveGeometry());
	set.setValue("VideoWinPos",pos());
	set.setValue("VideoWinIsFullscreen",isFullScreen());
	set.endGroup();

	QVideoWidget::closeEvent(event);
}

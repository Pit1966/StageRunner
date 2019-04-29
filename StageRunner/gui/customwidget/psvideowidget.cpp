//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

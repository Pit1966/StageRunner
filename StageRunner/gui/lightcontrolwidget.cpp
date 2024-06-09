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

#include "lightcontrolwidget.h"
#include "appcentral.h"

#ifdef IS_QT5
#	include "system/videoplayer.h"
#endif

#include <QMessageBox>

LightControlWidget::LightControlWidget(QWidget *parent) :
	QGroupBox(parent)
{
	init();
	setupUi(this);
}

void LightControlWidget::setVideoStatus(int viewState, int oldState)
{
	Q_UNUSED(oldState)
	QString txt = VideoPlayer::viewStateToString(VIDEO::VideoViewStatus(viewState));
	videoViewStateLabel->setText(txt);
}

void LightControlWidget::init()
{
	appcentral = AppCentral::instance();
}

void LightControlWidget::on_blackButton_clicked()
{
	appcentral->sequenceStop();
	appcentral->lightBlack(0);
	appcentral->videoBlack(0);

	int clickcnt = blackButton->property("clickcnt").toInt();
	qint64 ms = appcentral->uptimeMs();
	qint64 lastMs = blackButton->property("lastclicked").toLongLong();
	if (ms - lastMs > 300)
		clickcnt = 0;

	// qDebug() << "dif ms" << ms-lastMs << "clickcnt" << clickcnt;
	QWidget *parentWid = this;
	if (++clickcnt == 2 && appcentral->isVideoWidgetVisible(&parentWid)) {
		int ret = QMessageBox::question(parentWid,tr("Attention")
										,tr("Do you want to close the video output?")
										,QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
			appcentral->closeVideoWidget();
	}

	blackButton->setProperty("lastclicked", ms);
	blackButton->setProperty("clickcnt",clickcnt);
}

void LightControlWidget::on_fadeToBlackButton_clicked()
{
	appcentral->sequenceStop();
	appcentral->lightBlack(fadeSecondsSpin->value() * 1000);
	appcentral->videoBlack(fadeSecondsSpin->value() * 1000);
}


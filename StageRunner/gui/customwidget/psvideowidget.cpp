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
#include <QPainter>
#include <QPicture>

#include "psvideowidget.h"
#include "videoplayer.h"
#include "psoverlaylabel.h"
#include "configrev.h"

PsVideoWidget::PsVideoWidget(QWidget *parent)
	: QVideoWidget(parent)
	, m_myPlayer(nullptr)
	, m_overlay(nullptr)
	, m_isOverlayVisible(false)
	, m_overlayOpaticity(1.0)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAutoFillBackground(false);

	m_overlay = new PsOverlayLabel();
	connect(m_overlay, SIGNAL(doubleClicked()), this, SLOT(toggleFullScreen()));

	m_overlay->move(pos());
	m_overlay->resize(size());
	m_overlay->raise();
	m_overlay->show();


	setPrefsSettings();

	//	setStyleSheet("background: blue;");
}

PsVideoWidget::~PsVideoWidget()
{
	delete m_overlay;
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

void PsVideoWidget::raiseOverlay()
{
	if (m_overlay) {
		if (m_overlay->size() != size())
			m_overlay->resize(size());
		m_overlay->raise();
		m_overlay->update();
	}
}

void PsVideoWidget::setOverlayVisible(bool state)
{
	if (m_overlay) {
		m_isOverlayVisible = state;
		// overlay is only visible, if videowidget is also visible
		this->show();
		this->raise();
		checkOverlayShow();
		m_overlay->update();
	}
}

bool PsVideoWidget::setOverlayImage(const QString &path)
{
	if (!m_overlay)
		return true;

	m_overlay->setText(QString());

	// get label dimensions
	int w = width();
	int h = height();
	QPixmap p(path);

	// set a scaled pixmap to a w x h window keeping its aspect ratio
	m_overlay->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	m_overlay->update();
	return true;
}

void PsVideoWidget::checkOverlayShow()
{
	if (!m_overlay)
		return;

	if (isVisible()) {
		if (m_isOverlayVisible) {
			m_overlay->show();
			m_overlay->raise();
		} else {
			m_overlay->hide();
		}
	} else {
		m_overlay->hide();
	}
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

	if (m_myPlayer && m_myPlayer->isRunning())
		m_myPlayer->stop();

#if QT_VERSION >= 0x050600
	if (m_overlay)
		m_overlay->hide();
	hide();
	event->ignore();
	return;
#else
	QVideoWidget::closeEvent(event);
	if (m_overlay)
		m_overlay->close();
#endif
}

void PsVideoWidget::paintEvent(QPaintEvent */*event*/)
{

	QPainter painter(this);

//	painter.setRenderHint(QPainter::Antialiasing);
//	painter.setPen(Qt::NoPen);
//	QLinearGradient gradient(QPointF(100,100),QPointF(100,400));
//	gradient.setColorAt(0,QColor::fromRgbF(0.37f,0.55f,0.64f,0.9f));
//	gradient.setColorAt(1,QColor::fromRgbF(0.27f,0.35f,0.54f,0.9f));
//	painter.setBrush(gradient);

	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, width(), height());

	// qDebug() << "paintEvent";
}

void PsVideoWidget::moveEvent(QMoveEvent *event)
{
	if (m_overlay) {
		m_overlay->move(event->pos());
		checkOverlayShow();
	}
}

void PsVideoWidget::resizeEvent(QResizeEvent *event)
{
	if (m_overlay) {
		m_overlay->resize(event->size());
		checkOverlayShow();
	}
}

void PsVideoWidget::showEvent(QShowEvent *)
{
	if (m_overlay) {
		checkOverlayShow();
		m_overlay->update();
	}
}

void PsVideoWidget::hideEvent(QHideEvent *)
{
	checkOverlayShow();
}

void PsVideoWidget::toggleFullScreen()
{
	setFullScreen(!isFullScreen());
}

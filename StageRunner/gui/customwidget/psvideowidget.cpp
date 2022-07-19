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
#include <QGraphicsOpacityEffect>

#include "psvideowidget.h"
#include "videoplayer.h"
#include "psoverlaylabel.h"
#include "configrev.h"

PsVideoWidget::PsVideoWidget(QWidget *parent)
	: QVideoWidget(parent)
	, m_myPlayer(nullptr)
	, m_overlay{nullptr,}
	, m_opFx{nullptr,}
	, m_hasOverlays(false)
	, m_hasOpacityFx(false)
	, m_isOverlayVisible(false)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAutoFillBackground(false);

	m_hasOpacityFx = true;

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlay[i] = new PsOverlayLabel(this);

		if (m_hasOpacityFx) {
			m_opFx[i] = new QGraphicsOpacityEffect();
			//auto ef = new QGraphicsOpacityEffect();;
			//m_overlay[i]->setGraphicsEffect(ef);
			//ef->setOpacity(0.3);
			m_overlay[i]->setGraphicsEffect(m_opFx[i]);
			m_opFx[i]->setOpacity(0.0);

			m_overlay[i]->resize(size());
			m_overlay[i]->move(0, 0);
			m_overlay[i]->show();
			// m_overlay[i]->setStyleSheet("background: green; color: white;");
			m_overlay[i]->raise();
		}
		else {

			// qDebug("winflags %x", int(m_overlay[i]->windowFlags()));
			// m_overlay[i]->setWindowFlags(Qt::ToolTip);
			// m_overlay[i]->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint | Qt::WindowTransparentForInput);		// this prevents windows from being listed in task bar

			m_overlay[i]->move(pos());
			m_overlay[i]->resize(size());
			m_overlay[i]->raise();
			m_overlay[i]->show();
			m_overlay[i]->setWindowOpacity(0.0);
		}

		// connect(m_overlay[i], SIGNAL(doubleClicked()), this, SLOT(toggleFullScreen()));
	}
	m_hasOverlays = true;

	setPrefsSettings();

	// setStyleSheet("background: blue;");
}

PsVideoWidget::~PsVideoWidget()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		delete m_opFx[i];
		delete m_overlay[i];
	}
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

void PsVideoWidget::raisePicClipOverlay(int layer)
{
	if (m_hasOpacityFx)
		return;

	if (layer >= PIC_OVERLAY_COUNT || !m_hasOverlays)
		return;

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		if (m_overlay[i]->size() != size())
			m_overlay[i]->resize(size());
		m_overlay[i]->raise();
		m_overlay[i]->update();
	}

	if (layer >= 0)
		m_overlay[layer]->raise();
}

void PsVideoWidget::setPicClipOverlaysActive(bool state)
{
	if (!m_hasOverlays)
		return;

	if (m_isOverlayVisible != state) {
		m_isOverlayVisible = state;

		if (state) {
			// overlay is only visible, if videowidget is also visible
			this->show();
			this->raise();
			// Adjust sizes, for a freshly activated overlay to this video widget
			for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
				m_overlay[i]->resize(size());
				if (!m_hasOpacityFx)
					m_overlay[i]->move(pos());
			}
		}

		checkOverlayShow();
	}
}

bool PsVideoWidget::setPicClipOverlayImage(const QString &path, int layer, qreal opacity)
{
	if (!m_hasOverlays)
		return true;

	if (layer >= PIC_OVERLAY_COUNT)
		return false;

	m_overlay[layer]->setText(QString());

	// get label dimensions
	int w = width();
	int h = height();
	QPixmap p(path);

	// set a scaled pixmap to a w x h window keeping its aspect ratio
	m_overlay[layer]->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	setPicClipOverlayOpacity(opacity, layer);
	m_currentPicPaths[layer] = path;
	return true;
}

void PsVideoWidget::clearPicClipOverlayImage(int layer)
{
	if (layer < 0) {
		for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
			m_currentPicPaths[i].clear();
			m_overlay[i]->setPixmap(QPixmap());
			m_overlay[i]->update();
		}
	}
	else if (layer < PIC_OVERLAY_COUNT) {
		m_currentPicPaths[layer].clear();
		m_overlay[layer]->setPixmap(QPixmap());
		m_overlay[layer]->update();
	}
}

const QString &PsVideoWidget::overlayPicPath(int layer)
{
	return m_currentPicPaths[layer];
}

void PsVideoWidget::setPicClipOverlayOpacity(qreal val, int layer)
{
	if (m_hasOpacityFx) {
		qDebug() << "set opFx opacity: layer" << layer << val;
		if (layer < 0) {
			for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
				m_opFx[i]->setOpacity(val);
				m_overlay[i]->update();
			}
		}
		else if (layer < PIC_OVERLAY_COUNT) {
			m_opFx[layer]->setOpacity(val);
			m_overlay[layer]->update();
		}
	}
	else {
		// qDebug() << "set window opacity: layer" << layer << val;
		if (layer < 0) {
			for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
				m_overlay[i]->setWindowOpacity(val);
				m_overlay[i]->update();
			}
		}
		else if (layer < PIC_OVERLAY_COUNT) {
			m_overlay[layer]->setWindowOpacity(val);
			m_overlay[layer]->update();
		}
	}
}

bool PsVideoWidget::isPicClipVisible(int layer)
{
	if (layer >= PIC_OVERLAY_COUNT)
		return false;

	if (m_hasOpacityFx) {
		return m_opFx[layer]->opacity() > 0.0;
	} else {
		return m_overlay[layer]->windowOpacity() > 0.0;
	}
}

bool PsVideoWidget::isPicPathVisible(const QString &path)
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		if (m_currentPicPaths[i] == path && isPicClipVisible(i))
			return true;
	}

	return false;
}

void PsVideoWidget::checkOverlayShow()
{
	if (!m_hasOverlays)
		return;

	if (isVisible()) {
		if (m_isOverlayVisible) {
			for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
				m_overlay[i]->show();
				m_overlay[i]->raise();
				m_overlay[i]->update();
			}
		} else {
			hideOverlays();
		}
	} else {
		hideOverlays();
	}
}

void PsVideoWidget::hideOverlays()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		setPicClipOverlayOpacity(0.0, i);
		m_overlay[i]->hide();
		m_currentPicPaths[i].clear();
	}
}

void PsVideoWidget::closeOverlays()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlay[i]->close();
		m_currentPicPaths[i].clear();
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
	if (m_hasOverlays)
		hideOverlays();
	hide();
	event->ignore();
	return;
#else
	QVideoWidget::closeEvent(event);
	if (m_hasOverlays)
		closeOverlays();
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
	if (!m_hasOverlays || m_hasOpacityFx)
		return;

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlay[i]->move(event->pos());
	}
	checkOverlayShow();
}

void PsVideoWidget::resizeEvent(QResizeEvent *event)
{
	if (m_hasOverlays) {
		for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
			m_overlay[i]->resize(event->size());
		}
		checkOverlayShow();
	}
}

void PsVideoWidget::showEvent(QShowEvent *)
{
	if (m_hasOverlays) {
		checkOverlayShow();
		for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
			m_overlay[i]->update();
		}
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

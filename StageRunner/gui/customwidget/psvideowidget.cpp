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
#include <QMenu>

#include "psvideowidget.h"
#ifdef IS_QT6
#	include "videoplayer6.h"
#else
#	include "videoplayer.h"
#endif
#include "psoverlaylabel.h"
#include "configrev.h"

PsVideoWidget::PsVideoWidget(QWidget *parent)
	: QVideoWidget(parent)
	, m_myPlayer(nullptr)
	, m_overlay{nullptr,}
	, m_hasOverlays(false)
	, m_isOverlayVisible(false)
	, m_mousePressed(false)
	, m_isWindowMoveMode(false)
	, m_doubleClicked(false)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAutoFillBackground(false);
	setWindowTitle("Video screen");

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	// setWindowFlag(Qt::WindowTransparentForInput);

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlay[i] = new PsOverlayLabel();

		// qDebug("winflags %x", int(m_overlay[i]->windowFlags()));
		// m_overlay[i]->setWindowFlags(Qt::ToolTip);
		m_overlay[i]->setWindowFlags(Qt::FramelessWindowHint | Qt::BypassWindowManagerHint | Qt::WindowTransparentForInput);		// this prevents windows from being listed in task bar

		m_overlay[i]->move(pos());
		m_overlay[i]->resize(size());
		m_overlay[i]->show();
		m_overlay[i]->raise();
		m_overlay[i]->setWindowOpacity(0.0);
		m_overlay[i]->setWindowTitle(QString("Video overlay %1").arg(i+1));

		// not necessary since Qt::WindowTransparentForInput is set for overlays
		// connect(m_overlay[i], SIGNAL(doubleClicked()), this, SLOT(toggleFullScreen()));
	}

	m_hasOverlays = true;

	setPrefsSettings();

	// setStyleSheet("background: blue;");
}

PsVideoWidget::~PsVideoWidget()
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
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

void PsVideoWidget::showVideoWidgetAndOverlays()
{
	this->show();
	this->raise();

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		if (m_overlay[i]->size() != size())
			m_overlay[i]->resize(size());
		if (m_overlay[i]->pos() != pos())
			m_overlay[i]->move(pos());

		m_overlay[i]->show();
		m_overlay[i]->raise();
	}
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
				m_overlay[i]->move(pos());
			}
		}

		checkOverlayShow();
	}

	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		qDebug() << "video overlay" << i << "size" << m_overlay[i]->width() << m_overlay[i]->height();
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

bool PsVideoWidget::isPicClipVisible(int layer)
{
	if (layer < 0) {
		for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
			if (m_overlay[i]->windowOpacity() > 0.0)
				return true;
		}
		return false;
	}
	else if (layer < PIC_OVERLAY_COUNT)
		return m_overlay[layer]->windowOpacity() > 0.0;

	return false;
}

bool PsVideoWidget::isPicPathVisible(const QString &path)
{
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		if (m_currentPicPaths[i] == path && isPicClipVisible(i) && isPicClipVisible(i))
			return true;
	}

	return false;
}

/**
 * @brief Returns PicClip layer that should be used for next PicClip display.
 * @return layer number. -1, if there is no free layer
 *
 * If layer one is visible this will return 0, if layer one is  visible it will return 1.
 * If no layer is used so far it will return layer 1
 */int PsVideoWidget::firstFreePicClicLayer()
{
	bool v1 = isPicClipVisible(1);
	bool v0 = isPicClipVisible(0);
	qDebug() << "visible layer" << v0 << v1;
	if (!v1)
		return 1;
	if (!v0)
		return 0;

	return -1;
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
	// qDebug() << "hide overlays";
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		setPicClipOverlayOpacity(0.0, i);
		m_overlay[i]->hide();
		m_currentPicPaths[i].clear();
	}
}

void PsVideoWidget::closeOverlays()
{
	// qDebug() << "close overlays";
	for (int i=0; i<PIC_OVERLAY_COUNT; i++) {
		m_overlay[i]->close();
		m_currentPicPaths[i].clear();
	}

	m_isOverlayVisible = false;
}

void PsVideoWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	if (isFullScreen()) {
		setFullScreen(false);
		if (!m_windowGeometry.isEmpty())
			restoreGeometry(m_windowGeometry);
	}
	else {
		m_windowGeometry = saveGeometry();
		setFullScreen(true);
	}
	m_mousePressed = false;
	m_isWindowMoveMode = false;
	m_doubleClicked = true;
}

void PsVideoWidget::mousePressEvent(QMouseEvent *ev)
{
	m_mousePressed = true;
	m_clickStartPos = ev->globalPos();
	m_clickWindowPos = pos();
	m_doubleClicked = false;
}

void PsVideoWidget::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	m_mousePressed = false;
	m_isWindowMoveMode = false;

	if (event->button() == Qt::RightButton) {
		QMenu menu(this);
		QAction *act;
		act = menu.addAction(tr("Resolution 640x480 (4:3)"));
		act->setProperty("cmd", "resolution");
		act->setProperty("x", 640);
		act->setProperty("y", 480);

		act = menu.addAction(tr("Resolution 720x405 (16:9)"));
		act->setProperty("cmd", "resolution");
		act->setProperty("x", 720);
		act->setProperty("y", 405);

		act = menu.addAction(tr("Resolution 1920x1080 (16:9 HD)"));
		act->setProperty("cmd", "resolution");
		act->setProperty("x", 1920);
		act->setProperty("y", 1080);

		if (isFullScreen()) {
			act = menu.addAction(tr("Fullscreen off"));
			act->setProperty("cmd","fullscreen");
			act->setProperty("opt1", false);
		} else {
			act = menu.addAction(tr("Fullscreen on"));
			act->setProperty("cmd","fullscreen");
			act->setProperty("opt1", true);
		}

		menu.addSeparator();

		act = menu.addAction(tr("Close video window"));
		act->setProperty("cmd","close");
		act = menu.exec(event->globalPos());
		if (!act)
			return;

		QString cmd = act->property("cmd").toString();
		if (cmd == "resolution") {
			resize(act->property("x").toInt(), act->property("y").toInt());
		}
		else if (cmd == "fullscreen") {
			setFullScreen(act->property("opt1").toBool());
		}
		else if (cmd == "close") {
			close();
		}
	}
}

void PsVideoWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPoint curpos = event->globalPos();
	QPoint movevec = m_clickStartPos - curpos;

	if (!isFullScreen() && !m_doubleClicked) {

		if (m_isWindowMoveMode) {
			move(m_clickWindowPos - movevec);
			qDebug() << "video wid pos" << this->pos();
		}
		else {
			int dist = movevec.manhattanLength();
			if (dist > 20)
				m_isWindowMoveMode = true;
		}
	}
}

void PsVideoWidget::closeEvent(QCloseEvent *event)
{
	saveCurrentStateToPrefs();

	if (m_myPlayer) {
		if (m_myPlayer->isRunning())
			m_myPlayer->stop();
		m_myPlayer->clearViewState();
	}

	m_isOverlayVisible = false;

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

	// if (m_myPlayer->viewState() == VIDEO::VIEW_BLACK) {
	// 	painter.setBrush(Qt::blue);
	// } else {

	painter.setBrush(Qt::black);
	painter.drawRect(-1, -1, width()+2, height()+2);

	// qDebug() << "paintEvent";
}

void PsVideoWidget::moveEvent(QMoveEvent *event)
{
	if (!m_hasOverlays)
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

//bool PsVideoWidget::event(QEvent *event)
//{
//	qDebug() << "event" << event->type();

//	return QVideoWidget::event(event);
//}

void PsVideoWidget::toggleFullScreen()
{
	setFullScreen(!isFullScreen());
}


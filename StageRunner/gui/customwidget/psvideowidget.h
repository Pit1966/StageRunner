//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef PSVIDEOWIDGET_H
#define PSVIDEOWIDGET_H

#include <QVideoWidget>

class VideoPlayer;
class PsOverlayLabel;
class QGraphicsOpacityEffect;

#define PIC_OVERLAY_COUNT 2

class PsVideoWidget : public QVideoWidget
{
	Q_OBJECT
private:
	VideoPlayer *m_myPlayer;
	PsOverlayLabel *m_overlay[PIC_OVERLAY_COUNT];

	QString m_currentPicPaths[PIC_OVERLAY_COUNT];
	bool m_hasOverlays;
	bool m_isOverlayVisible;
	bool m_mousePressed;
	bool m_isWindowMoveMode;
	bool m_doubleClicked;

	QPoint m_clickStartPos;
	QPoint m_clickWindowPos;
	QByteArray m_windowGeometry;		///< Window geometry before switch to fullscreen

public:
	PsVideoWidget(QWidget *parent = nullptr);
	~PsVideoWidget();
	void setPrefsSettings();
	void saveCurrentStateToPrefs();
	void setVideoPlayer(VideoPlayer *vidplay);
	void raisePicClipOverlay(int layer = -1);
	void showVideoWidgetAndOverlays();
	bool isPicClipOverlaysActive() const {return m_isOverlayVisible;}
	void setPicClipOverlaysActive(bool state);
	bool setPicClipOverlayImage(const QString &path, int layer, qreal opacity);
	void clearPicClipOverlayImage(int layer = -1);
	const QString &overlayPicPath(int layer);
	void setPicClipOverlayOpacity(qreal val, int layer = -1);
	bool isPicClipVisible(int layer = -1);
	bool isPicPathVisible(const QString &path);
	int firstFreePicClicLayer();

protected:
	void checkOverlayShow();
	void hideOverlays();
	void closeOverlays();
	void mouseDoubleClickEvent(QMouseEvent *) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void closeEvent(QCloseEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void moveEvent(QMoveEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;
//	bool event(QEvent *event);

public slots:
	void toggleFullScreen();
};

#endif // PSVIDEOWIDGET_H

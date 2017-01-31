#ifndef PSVIDEOWIDGET_H
#define PSVIDEOWIDGET_H

#include <QVideoWidget>

class VideoPlayer;

class PsVideoWidget : public QVideoWidget
{
private:
	VideoPlayer *m_myPlayer;
public:
	PsVideoWidget(QWidget *parent = 0);
	void setVideoPlayer(VideoPlayer *vidplay);


protected:
	void mouseDoubleClickEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *ev);
	void closeEvent(QCloseEvent *event);
};

#endif // PSVIDEOWIDGET_H

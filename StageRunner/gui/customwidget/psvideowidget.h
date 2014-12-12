#ifndef PSVIDEOWIDGET_H
#define PSVIDEOWIDGET_H

#include <QVideoWidget>

class PsVideoWidget : public QVideoWidget
{
public:
	PsVideoWidget(QWidget *parent = 0);

protected:
	void mouseDoubleClickEvent(QMouseEvent *);
	void closeEvent(QCloseEvent *event);
};

#endif // PSVIDEOWIDGET_H

#ifndef VIDEOSTATUSWIDGET_H
#define VIDEOSTATUSWIDGET_H

#include <QWidget>

namespace Ui {
class VideoStatusWidget;
}

class VideoStatusWidget : public QWidget
{
	Q_OBJECT

public:
	explicit VideoStatusWidget(QWidget *parent = nullptr);
	~VideoStatusWidget();

private:
	Ui::VideoStatusWidget *ui;
};

#endif // VIDEOSTATUSWIDGET_H

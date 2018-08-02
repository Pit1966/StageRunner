#ifndef PSDOCKWIDGET_H
#define PSDOCKWIDGET_H

#include <QDockWidget>

class PsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	PsDockWidget(QWidget * parent = nullptr, Qt::WindowFlags flags = nullptr);
};

#endif // PSDOCKWIDGET_H

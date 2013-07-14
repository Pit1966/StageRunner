#ifndef PSLISTWIDGET_H
#define PSLISTWIDGET_H

#ifdef IS_QT5
#include <QtWidgets>
#endif

class PsListWidget : public QListWidget
{
	Q_OBJECT
public:
	PsListWidget(QWidget *parent = 0);

signals:

public slots:

};

#endif // PSLISTWIDGET_H

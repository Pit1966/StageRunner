#ifndef PSLABEL_H
#define PSLABEL_H

#include <QLabel>

class PsLabel : public QLabel
{
	Q_OBJECT
public:
	PsLabel(QWidget *parent = 0);

	void mouseDoubleClickEvent(QMouseEvent *event);

signals:

public slots:

signals:
	void doubleClicked();

};

#endif // PSLABEL_H

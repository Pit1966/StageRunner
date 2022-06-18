#ifndef PSOVERLAYLABEL_H
#define PSOVERLAYLABEL_H

#include <QLabel>

class PsOverlayLabel : public QLabel
{
	Q_OBJECT

public:
	PsOverlayLabel(QWidget *parent = nullptr);

	// QWidget interface
protected:
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:
	void doubleClicked();

};

#endif // PSOVERLAYLABEL_H

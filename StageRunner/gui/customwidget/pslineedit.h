#ifndef PSLINEEDIT_H
#define PSLINEEDIT_H

#include <QLineEdit>

class PsLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	PsLineEdit(QWidget *parent = 0);

private:
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:
	void doubleClicked();

public slots:

};

#endif // PSLINEEDIT_H

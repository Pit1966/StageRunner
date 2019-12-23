#ifndef PSPUSHBUTTON_H
#define PSPUSHBUTTON_H

#include <QPushButton>

class PsPushButton : public QPushButton
{
	Q_OBJECT
public:
	bool lastEventWasInputMethodQuery = false;

public:
	explicit PsPushButton(QWidget *parent = nullptr);
	explicit PsPushButton(const QString &text, QWidget *parent = nullptr);

protected:
	bool event(QEvent *e);

};

#endif // PSPUSHBUTTON_H

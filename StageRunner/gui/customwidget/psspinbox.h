#ifndef PSSPINBOX_H
#define PSSPINBOX_H

#include <QSpinBox>

class PsSpinBox : public QSpinBox
{
	Q_OBJECT
private:
	bool m_pressed;					///< will be set to true on click
	int m_valueOnPress;					///< this will be the value of the spin at mouse click time
public:
	PsSpinBox();

public:
	PsSpinBox(QWidget *parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

signals:
	void clickedAndChanged(int val);

};

#endif // PSSPINBOX_H

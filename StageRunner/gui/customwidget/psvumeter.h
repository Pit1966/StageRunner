#ifndef PSVUMETER_H
#define PSVUMETER_H

#include "thirdparty/widget/qsynthmeter.h"


class PsVuMeter : public qsynthMeter
{
	Q_OBJECT

private:
	float m_volume;
	float m_dragBeginVolume;
	int m_mouseButton;
	QPoint m_dragBeginMousePos;

public:
	PsVuMeter(QWidget *parent = 0);
	inline float volume() const {return m_volume;}
	void setVolume(float vol);

private:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *);

signals:
	void valueChanged(float level);
	void valueChanged(int level);
	void sliderPressed();
	void sliderReleased();
};

#endif // PSVUMETER_H

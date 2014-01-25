#ifndef PSSPECTROMETER_H
#define PSSPECTROMETER_H

#include "frqspectrum.h"

#include <QWidget>

class PsSpectrometer : public QWidget
{
	Q_OBJECT
private:
	FrqSpectrum m_spectrum;

public:
	PsSpectrometer(QWidget *parent = 0, int specSize = 40);
	inline int frqBands() const {return m_spectrum.size();}

private:
	void paintEvent(QPaintEvent *event);

signals:

public slots:
	void setFrqSpectrum(FrqSpectrum *newSpectrum);

};

#endif // PSSPECTROMETER_H

#ifndef FXTIMER_H
#define FXTIMER_H

#include <QObject>

class FxTimer : public QObject
{
	Q_OBJECT
public:
	explicit FxTimer(QObject *parent = nullptr);

signals:

};

#endif // FXTIMER_H

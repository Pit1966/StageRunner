#ifndef FXTIMER_H
#define FXTIMER_H

#include <QTimer>

class FxItem;

class FxTimer : public QTimer
{
	Q_OBJECT
private:
	FxItem *m_fxItem = nullptr;

public:
	explicit FxTimer(QObject *parent = nullptr);
	explicit FxTimer(FxItem * fx);
	~FxTimer();

	FxItem *fxItem() {return m_fxItem;}

private slots:
	void onTimeout();

signals:
	void fxTimeout(FxItem *fx);
	void fxTimeout(FxTimer *fxTimer);

};

#endif // FXTIMER_H

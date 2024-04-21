#ifndef EXTELAPSEDTIMER_H
#define EXTELAPSEDTIMER_H

#include <QElapsedTimer>

class ExtElapsedTimer
{
private:
	QElapsedTimer m_time;
	bool m_isRunning;
	qint64 m_addNs;

public:
	ExtElapsedTimer();
	void start();
	void stop();
	void cont();
	void reset();
	qint64 nsecsElapsed() const;
	qint64 elapsed() const;
	void setMs(qint64 ms, bool stop = false);
};

#endif // EXTELAPSEDTIMER_H

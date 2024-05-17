#include "extelapsedtimer.h"
#include <QDebug>

ExtElapsedTimer::ExtElapsedTimer(bool startTimer)
	: m_isRunning(false)
	, m_addNs(0)
{
	if (startTimer)
		start();
}

/**
 * @brief Start timer
 *
 * The timer is started. If timer is already running, the time will be resetted to zero.
 */
void ExtElapsedTimer::start()
{
	m_time.start();
	m_isRunning = true;
	m_addNs = 0;
}

void ExtElapsedTimer::stop()
{
	if (m_isRunning) {
		m_addNs += m_time.nsecsElapsed();
		m_isRunning = false;
	}
}

void ExtElapsedTimer::cont()
{
	if (!m_isRunning) {
		m_time.start();
		m_isRunning = true;
	}
}

void ExtElapsedTimer::reset()
{
	m_addNs = 0;
	m_time.start();
}

/**
 * @brief Read the elapsed time of the timer
 * @return time in nanoseconds
 *
 * If timer is running, the current elapsed time is returned.
 * If timer is not running, the timer value when it was stopped is returned or zero, if it never has run
 */
qint64 ExtElapsedTimer::nsecsElapsed() const
{
	if (m_isRunning) {
		return m_addNs + m_time.nsecsElapsed();
	} else {
		return m_addNs;
	}
}

/**
 * @brief Read the elapsed time of the timer
 * @return time in milliseconds
 *
 * If timer is running, the current elapsed time is returned.
 * If timer is not running, the timer value when it was stopped is returned or zero, if it never has run
 */
qint64 ExtElapsedTimer::elapsed() const
{
	qint64 ms = nsecsElapsed() / 1000000;
	// qDebug() << "elapsed" << ms;
	return ms;
}

void ExtElapsedTimer::setMs(qint64 ms, bool stop)
{
	// qDebug() << "runtime set ms" << ms << stop;
	m_addNs = ms * 1000000;
	if (stop) {
		m_isRunning = false;
	} else {
		m_time.start();
		m_isRunning = true;
	}
}

/** @file toolclasses.cpp */
/***************************************************************************
*   Copyright (C) 2008-2012 QASS GmbH                                     *
*                                                                         *
*   Ulrich Seuthe <useuthe@qass.net>                                      *
*   Peter Steinmeyer <steinmeyer@qass.net>                                *
*   Christian Günther <c.guenther@qass.net>                               *
*                                                                         *
*  This file is part of the OPTIMIZER project                             *
***************************************************************************/
#include "toolclasses.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QThread>
// #include <errno.h>


#ifdef USE_SAFE_MUTEX

#include <QtCore/qelapsedtimer.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <signal.h>

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
SafeMutex::SafeMutex(QMutex::RecursionMode mode)
	: d(new SafeMutexPrivate(mode))
{ }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
SafeMutex::~SafeMutex()
{ delete static_cast<SafeMutexPrivate *>(d); }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void SafeMutex::lock()
{
	SafeMutexPrivate *d = static_cast<SafeMutexPrivate *>(this->d);
	Qt::HANDLE self;

	if (d->recursive) {
		self = QThread::currentThreadId();
		if (d->owner == self) {
			++d->count;
			Q_ASSERT_X(d->count != 0, "SafeMutex::lock", "Overflow in recursion counter");
			return;
		}

		bool isLocked = d->contenders.testAndSetAcquire(0, 1);
		if (!isLocked) {
			// didn't get the lock, wait for it
			isLocked = d->wait();
			Q_ASSERT_X(isLocked, "SafeMutex::lock",
					   "Internal error, infinite wait has timed out.");
		}

		d->owner = self;
		++d->count;
		Q_ASSERT_X(d->count != 0, "SafeMutex::lock", "Overflow in recursion counter");
		return;
	}

	bool isLocked = d->contenders.testAndSetAcquire(0, 1);
	if (!isLocked) {
		lockInternal();
	}
	else
		d->count = 1;

}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
bool SafeMutex::tryLock()
{
	SafeMutexPrivate *d = static_cast<SafeMutexPrivate *>(this->d);
	Qt::HANDLE self;

	if (d->recursive) {
		self = QThread::currentThreadId();
		if (d->owner == self) {
			++d->count;
			Q_ASSERT_X(d->count != 0, "SafeMutex::tryLock", "Overflow in recursion counter");
			return true;
		}

		bool isLocked = d->contenders.testAndSetAcquire(0, 1);
		if (!isLocked) {
			// some other thread has the mutex locked, or we tried to
			// recursively lock an non-recursive mutex
			return isLocked;
		}

		d->owner = self;
		++d->count;
		Q_ASSERT_X(d->count != 0, "SafeMutex::tryLock", "Overflow in recursion counter");
		return isLocked;
	}

	bool result = d->contenders.testAndSetAcquire(0, 1);

	if ( result )
		 d->count = 1;

	return result;
}

/*!
	\internal helper for lockInline()
 */
void SafeMutex::lockInternal()
{
	SafeMutexPrivate *d = static_cast<SafeMutexPrivate *>(this->d);

	if (QThread::idealThreadCount() == 1) {
		// don't spin on single cpu machines
		bool isLocked = d->wait();
		Q_ASSERT_X(isLocked, "QMutex::lock",
				   "Internal error, infinite wait has timed out.");
		Q_UNUSED(isLocked);
		d->count = isLocked ? 1 : 0;
		return;
	}

	QElapsedTimer elapsedTimer;
	elapsedTimer.start();
	do {
		qint64 spinTime = elapsedTimer.nsecsElapsed();
		if (spinTime > d->maximumSpinTime) {
			// didn't get the lock, wait for it, since we're not going to gain anything by spinning more
			elapsedTimer.start();
			bool isLocked = d->wait();
			Q_ASSERT_X(isLocked, "QMutex::lock",
					   "Internal error, infinite wait has timed out.");
			Q_UNUSED(isLocked);
			d->count = isLocked ? 1 : 0;

			qint64 maximumSpinTime = d->maximumSpinTime;
			qint64 averageWaitTime = d->averageWaitTime;
			qint64 actualWaitTime = elapsedTimer.nsecsElapsed();
			if (actualWaitTime < (SafeMutexPrivate::MaximumSpinTimeThreshold * 3 / 2)) {
				// measure the wait times
				averageWaitTime = d->averageWaitTime = qMin((averageWaitTime + actualWaitTime) / 2, qint64(SafeMutexPrivate::MaximumSpinTimeThreshold));
			}

			// adjust the spin count when spinning does not benefit contention performance
			if ((spinTime + actualWaitTime) - qint64(SafeMutexPrivate::MaximumSpinTimeThreshold) >= qint64(SafeMutexPrivate::MaximumSpinTimeThreshold)) {
				// long waits, stop spinning
				d->maximumSpinTime = 0;
			} else {
				// allow spinning if wait times decrease, but never spin more than the average wait time (otherwise we may perform worse)
				d->maximumSpinTime = qBound(qint64(averageWaitTime * 3 / 2), maximumSpinTime / 2, qint64(SafeMutexPrivate::MaximumSpinTimeThreshold));
			}
			return;
		}
		// be a good citizen... yielding lets something else run if there is something to run, but may also relieve memory pressure if not
		QThread::yieldCurrentThread();
	} while (d->contenders != 0 || !d->contenders.testAndSetAcquire(0, 1));

	// spinning is working, do not change the spin time (unless we are using much less time than allowed to spin)
	qint64 maximumSpinTime = d->maximumSpinTime;
	qint64 spinTime = elapsedTimer.nsecsElapsed();
	if (spinTime < maximumSpinTime / 2) {
		// we are using much less time than we need, adjust the limit
		d->maximumSpinTime = qBound(qint64(d->averageWaitTime * 3 / 2), maximumSpinTime / 2, qint64(SafeMutexPrivate::MaximumSpinTimeThreshold));
	}

	d->count = 1;

}

void SafeMutex::unlockInternal()
{
	static_cast<SafeMutexPrivate *>(d)->wakeUp();
	static_cast<SafeMutexPrivate *>(d)->count = 0;
}
//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
bool SafeMutex::tryLock(int timeout)
{
	SafeMutexPrivate *d = static_cast<SafeMutexPrivate *>(this->d);
	Qt::HANDLE self;

	if (d->recursive) {
		self = QThread::currentThreadId();
		if (d->owner == self) {
			++d->count;
			Q_ASSERT_X(d->count != 0, "SafeMutex::tryLock", "Overflow in recursion counter");
			return true;
		}

		bool isLocked = d->contenders.testAndSetAcquire(0, 1);
		if (!isLocked) {
			// didn't get the lock, wait for it
			isLocked = d->wait(timeout);
			if (!isLocked)
				return false;
		}

		d->owner = self;
		++d->count;
		Q_ASSERT_X(d->count != 0, "SafeMutex::tryLock", "Overflow in recursion counter");
		return true;
	}

	bool result = (d->contenders.testAndSetAcquire(0, 1)
					// didn't get the lock, wait for it
					|| d->wait(timeout));

	if ( result )
		 d->count = 1;

	return result;
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void SafeMutex::unlock()
{
	SafeMutexPrivate *d = static_cast<SafeMutexPrivate *>(this->d);

	if ( (0==d->count && d->recursive) || (!d->recursive && d->contenders<1) )
	{
		DEBUG_BREAK();
		qDebug("tried to unlock a non-locked mutex which results in undefined behavior");
	}

	if (d->recursive) {
		if (!--d->count) {
			d->owner = 0;
			if (!d->contenders.testAndSetRelease(1, 0))
				d->wakeUp();
		}
	} else {
		if (!d->contenders.testAndSetRelease(1, 0))	{
			d->wakeUp();
			d->count = 0;
		}
	}
}



SafeMutexPrivate::SafeMutexPrivate(QMutex::RecursionMode mode)
	: SafeMutexData(mode), maximumSpinTime(MaximumSpinTimeThreshold), averageWaitTime(0), owner(0), count(0)
{
}

SafeMutexPrivate::~SafeMutexPrivate()
{
}


static inline int _q_futex(volatile int *addr, int op, int val, const struct timespec *timeout, int *addr2, int val2)
{
	return syscall(SYS_futex, addr, op, val, timeout, addr2, val2);
}

bool SafeMutexPrivate::wait(int timeout)
{
	struct timespec ts, *pts = 0;
	QElapsedTimer timer;
	if (timeout >= 0) {
		ts.tv_nsec = ((timeout % 1000) * 1000) * 1000;
		ts.tv_sec = (timeout / 1000);
		pts = &ts;
		timer.start();
	}
	while (contenders.fetchAndStoreAcquire(2) > 0) {
		int r = _q_futex(&contenders._q_value, FUTEX_WAIT, 2, pts, 0, 0);
		if (r != 0 && errno == ETIMEDOUT)
			return false;

		if (pts) {
			// recalculate the timeout
			qint64 xtimeout = timeout * 1000 * 1000;
			xtimeout -= timer.nsecsElapsed();
			if (xtimeout < 0) {
				// timer expired after we returned
				return false;
			}

			ts.tv_sec = xtimeout / Q_INT64_C(1000) / 1000 / 1000;
			ts.tv_nsec = xtimeout % (Q_INT64_C(1000) * 1000 * 1000);
		}
	}
	return true;
}

void SafeMutexPrivate::wakeUp()
{
	(void) contenders.fetchAndStoreRelease(0);
	(void) _q_futex(&contenders._q_value, FUTEX_WAKE, 1, 0, 0, 0);
}


#endif

/** @file toolclasses.h */
/***************************************************************************

***************************************************************************/
#ifndef TOOLCLASSES_H
#define TOOLCLASSES_H

#include <QList>
#include <QMutex>
#include <QString>

#include "system/log.h"

// #define USE_SAFE_MUTEX


/**
	@author QASS GmbH <steinmeyer@qass.net>
*/
template <class T> class MutexQList : public QList <T>
{
public:
	MutexQList() {
		is_modified_f = false;
		mutex = new QMutex();
		lock_cnt = 0;
		ci = 0;
		co = 0;
	}

	~MutexQList() {
		if (lock_cnt > 0) {
			DEBUGERROR("MutexQList is locked while deleting: '%s'"
					   ,name.toLatin1().data());
		} else {
			delete mutex;
		}
	}

	MutexQList<T> & operator =(const MutexQList & other) {
		this->QList<T>::operator=(other);
		is_modified_f = other.is_modified_f;

		return *this;
	}

private:
	QMutex * mutex;							///< Mutex, mit dessen Hilfe die Liste gelockt wird
	bool is_modified_f;						///< Liste ist modifiziert
	QString name;							///< Listenname
	int lock_cnt;
	int ci,co;

public:
	inline int lockCnt() {return lock_cnt;}

	inline void lock() {
		if (lock_cnt > 0) {
			if (lock_cnt > 0) {
				DEVELTEXT("MutexQList lock:: maybe DOUBLE-LOCK! lockcnt:%d: %p '%s'"
					  ,lock_cnt,this,name.toLatin1().data());
				if (mutex->tryLock()) {
					lock_cnt++;
					DEVELTEXT("Lock it after TryLock");
				} else {
					DEBUGERROR("Did not lock it again!!");
				}
				return;
			}
		}
		mutex->lock();
		lock_cnt++;

//		if (name == "ProjectProcessList") {
//			if (ci == 6) {
//				qDebug("__stop");
//			}
//			qDebug("__in %d",ci++);
//		}
	}

	inline void unlock() {
		if (lock_cnt < 1) {
			DEVELTEXT("MutexQList unlock:: NOT_LOCKED! lockcnt:%d: %p '%s'"
					  ,lock_cnt,this,name.toLatin1().data());
		} else {
			lock_cnt--;
			mutex->unlock();
		}
//		if (name == "ProjectProcessList") {
//			qDebug("__out %d",co++);
//		}
	}

	inline bool tryLock(int timeout) {
		if (mutex->tryLock(timeout)) {
			lock_cnt++;
//			if (name == "ProjectProcessList") {
//				qDebug("__try_in %d",ci++);
//			}
			return true;
		} else {
			return false;
		}

	}

	inline bool tryLock() {
		if (lock_cnt > 0) {
			if (lock_cnt > 0) {
				DEVELTEXT("MutexQList tryLock:: maybe DOUBLE-LOCK! lockcnt:%d: %p '%s'"
					  ,lock_cnt,this,name.toLatin1().data());
				if (mutex->tryLock()) {
					lock_cnt++;
					DEVELTEXT("Lock it after TryLock");
					return true;
				} else {
					DEBUGERROR("Did not lock it again!!");
					return false;
				}
			}
		}
		if (mutex->tryLock()) {
			lock_cnt++;
//			if (name == "ProjectProcessList") {
//				qDebug("__try_in %d",ci++);
//			}
			return true;
		} else {
			return false;
		}
	}

	inline void lockAppend (const T & value) {
		lock();
		this->QList<T>::append(value);
		is_modified_f = true;
		unlock();
	}

	inline bool lockRemoveOne(const T & value) {
		bool ok;
		lock();
		ok = this->QList<T>::removeOne(value);
		if (ok) is_modified_f = true;
		unlock();
		return ok;
	}

	inline T lockTakeFirst() {
		T val;
		lock();
		val = this->QList<T>::takeFirst();
		is_modified_f = true;
		unlock();
		return val;
	}

	inline bool lockContains(const T & value) {
		bool ok;
		lock();
		ok = this->QList<T>::contains(value);
		unlock();
		return ok;
	}

	inline void lockClear () {
		lock();
		if (this->QList<T>::size()) is_modified_f = true;
		this->QList<T>::clear();
		unlock();
	}

	inline void lockSetModified(bool state) {
		lock();
		is_modified_f = state;
		unlock();
	}

	inline void append (const T & value) {
		this->QList<T>::append(value);
		is_modified_f = true;
	}

	inline bool removeOne(const T & value) {
		bool ok = this->QList<T>::removeOne(value);
		if (ok) is_modified_f = true;
		return ok;
	}

	inline int removeAll(const T & value) {
		bool cnt = this->QList<T>::removeAll(value);
		if (cnt) is_modified_f = true;
		return cnt;
	}

	inline void removeFirst() {
		is_modified_f = true;
		this->QList<T>::removeFirst();
	}

	inline void removeLast() {
		is_modified_f = true;
		this->QList<T>::removeLast();
	}

	inline void removeAt(int idx) {
		this->QList<T>::removeAt(idx);
		is_modified_f = true;
	}

	inline T takeAt(int idx) {
		is_modified_f = true;
		return this->QList<T>::takeAt(idx);
	}

	inline T takeFirst() {
		is_modified_f = true;
		return this->QList<T>::takeFirst();
	}

	inline T takeLast() {
		is_modified_f = true;
		return this->QList<T>::takeLast();
	}

	inline void setModified(bool state) {
		is_modified_f = state;
	}

	inline bool isModified() {return is_modified_f;}

	inline void clear () {
		if (this->QList<T>::size()) is_modified_f = true;
		this->QList<T>::clear();
	}

	void setName(const QString & n) {name = n;}

};




#ifdef USE_SAFE_MUTEX

class SafeMutexData
{
	public:
		QAtomicInt contenders;
		const uint recursive : 1;
		uint reserved : 31;
	protected:
		SafeMutexData(QMutex::RecursionMode mode);
		~SafeMutexData();
};


/**
 * \brief SafeMutex: QMutex-Wrapper  mit sicherem unlock
 * (QMutex verhält sich bei unlock auf nicht gelockte Threads undefiniert)
 *
 */
class SafeMutex
{
	friend class QWaitCondition;
	friend class QWaitConditionPrivate;

public:
	explicit SafeMutex(QMutex::RecursionMode mode = QMutex::NonRecursive);
	~SafeMutex();

	void lock();     //### Qt5: make inline;
	inline void lockInline();
	bool tryLock();  //### Qt5: make inline;
	bool tryLock(int timeout);
	inline bool tryLockInline();
	void unlock();     //### Qt5: make inline;
	inline void unlockInline();

private:
	void lockInternal();
	void unlockInternal();
	Q_DISABLE_COPY(SafeMutex)

	SafeMutexData *d;
};


#ifdef QT_NO_DEBUG
inline void SafeMutex::unlockInline()
{
	if (d->recursive) {
		unlock();
	} else if (!d->contenders.testAndSetRelease(1, 0)) {
		unlockInternal();
	}
}

inline bool SafeMutex::tryLockInline()
{
	if (d->recursive) {
		return tryLock();
	} else {
		return d->contenders.testAndSetAcquire(0, 1);
	}
}

inline void SafeMutex::lockInline()
{
	if (d->recursive) {
		lock();
	} else if(!tryLockInline()) {
		lockInternal();
	}
}
#else // QT_NO_DEBUG
//in debug we do not use inline calls in order to allow debugging tools
// to hook the mutex locking functions.
inline void SafeMutex::unlockInline() { unlock(); }
inline bool SafeMutex::tryLockInline() { return tryLock(); }
inline void SafeMutex::lockInline() { lock(); }
#endif // QT_NO_DEBUG

/**
 * \brief SafeMutexLocker: QMutexLocker-Wrapper für SafeMutex Instanzen
 * akzeptiert ein SafeMutex als Constructor-Parameter und lockt implizit beim
 * Erzeugen bzw. unlockt implizit beim Zerstören
 *
 */
class SafeMutexLocker
{
public:
	inline explicit SafeMutexLocker(SafeMutex *m)
	{
		Q_ASSERT_X((reinterpret_cast<quintptr>(m) & quintptr(1u)) == quintptr(0),
				   "SafeMutexLocker", "QMutex pointer is misaligned");
		if (m) {
			m->lockInline();
			val = reinterpret_cast<quintptr>(m) | quintptr(1u);
		} else {
			val = 0;
		}
	}
	inline ~SafeMutexLocker() { unlock(); }

	inline void unlock()
	{
		if ((val & quintptr(1u)) == quintptr(1u)) {
			val &= ~quintptr(1u);
			mutex()->unlockInline();
		}
	}

	inline void relock()
	{
		if (val) {
			if ((val & quintptr(1u)) == quintptr(0u)) {
				mutex()->lockInline();
				val |= quintptr(1u);
			}
		}
	}

	inline SafeMutex *mutex() const
	{
		return reinterpret_cast<SafeMutex*>(val & ~quintptr(1u));
	}

private:
	Q_DISABLE_COPY(SafeMutexLocker)

	quintptr val;
};

class SafeMutexPrivate : public SafeMutexData {
public:
	SafeMutexPrivate(QMutex::RecursionMode mode);
	~SafeMutexPrivate();

	bool wait(int timeout = -1);
	void wakeUp();

	// 1ms = 1000000ns
	enum { MaximumSpinTimeThreshold = 1000000 };
	volatile qint64 maximumSpinTime;
	volatile qint64 averageWaitTime;
	Qt::HANDLE owner;
	uint count;

#if defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_SYMBIAN)
	volatile bool wakeup;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#elif defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
	HANDLE event;
#elif defined(Q_OS_SYMBIAN)
	RSemaphore lock;
#endif
};

inline SafeMutexData::SafeMutexData(QMutex::RecursionMode mode)
	: recursive(mode == QMutex::Recursive)
{}

inline SafeMutexData::~SafeMutexData() {}

#else										// USE_SAFE_MUTEX
	typedef QMutex SafeMutex;
	typedef QMutexLocker SafeMutexLocker;
#endif


#endif //TOOLCLASSES_H

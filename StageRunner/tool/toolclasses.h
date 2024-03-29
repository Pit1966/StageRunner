/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
************************************************************************************/

#ifndef TOOLCLASSES_H
#define TOOLCLASSES_H

#include <QList>
#include <QMutex>
#include <QHash>
#include <QReadWriteLock>
#include <QString>
#if QT_VERSION >= 0x050e00
#  include <QRecursiveMutex>
#endif


template <class Key, class T> class MutexQHash : public QHash <Key, T>
{
private:
	/** Use this Mutex to lock Hash access */
	QReadWriteLock * myLock;
	/** How many times this instance was locked */
	QAtomicInt lockCount;
	/** For debugging give this instance a name */
	QString myName;

public:
	MutexQHash()
		: myLock(new QReadWriteLock(QReadWriteLock::Recursive))
	{}

	MutexQHash(const MutexQHash & other)
		: QHash<Key, T>(other)
		,myLock(new QReadWriteLock(QReadWriteLock::Recursive))
	{}

	~MutexQHash() {
#if QT_VERSION < 0x050000
		if (lockCount > 0) {
			qDebug("MutexQHash is locked %d times while deleting: '%s'"
					   ,int(lockCount), myName.toLocal8Bit().data());
#elif QT_VERSION >= 0x050f00
		if (lockCount.loadRelaxed()) {
			qDebug("MutexQHash is locked %d times while deleting: '%s'"
					   ,lockCount.loadRelaxed(), myName.toLocal8Bit().constData());
#else
		if (lockCount.load()) {
			qDebug("MutexQHash is locked %d times while deleting: '%s'"
					   ,lockCount.load(), myName.toLocal8Bit().constData());

#endif
		} else {
			delete myLock;
		}
	}

	MutexQHash<Key, T> & operator =(const MutexQHash & other) {
		this->QHash<Key, T>::operator=(other);
		return *this;
	}

	inline void readLock() {
		myLock->lockForRead();
		lockCount.ref();
	}

	inline void writeLock() {
		myLock->lockForWrite();
		lockCount.ref();
	}

	inline void unlock() {
		lockCount.deref();
		myLock->unlock();
	}

	typename QHash<Key, T>::iterator lockInsert(const Key &key, const T &value) {
		typename QHash<Key, T>::iterator it;
		myLock->lockForWrite();
		it = QHash<Key, T>::insert(key,value);
		myLock->unlock();
		return it;
	}

	bool lockContains(const Key &key) {
		bool found;
		myLock->lockForRead();
		found = QHash<Key, T>::contains(key);
		myLock->unlock();
		return found;
	}

	const T value(const Key &key) {
		myLock->lockForRead();
		const T val = QHash<Key, T>::value(key);
		myLock->unlock();
		return val;
	}

	int lockRemove(const Key &key) {
		int removed;
		myLock->lockForWrite();
		removed = QHash<Key, T>::remove(key);
		myLock->unlock();
		return removed;
	}





};




/***************************************************************************

***************************************************************************/


template <class T> class MutexQList : public QList <T>
{
public:
	MutexQList() {
		is_modified_f = false;
#if QT_VERSION < 0x050e00
		mutex = new QMutex(QMutex::Recursive);
#else
		mutex = new QRecursiveMutex();
#endif
		lock_cnt = 0;
		ci = 0;
		co = 0;
	}

	~MutexQList() {
		if (lock_cnt > 0) {
			qDebug("MutexQList is locked while deleting: '%s'"
					   ,name.toLocal8Bit().data());
		} else {
			delete mutex;
		}
	}

	MutexQList(const MutexQList & other)
		: QList<T>() {
		is_modified_f = false;
#if QT_VERSION < 0x050e00
		mutex = new QMutex(QMutex::Recursive);
#else
		mutex = new QRecursiveMutex();
#endif
		lock_cnt = 0;
		ci = 0;
		co = 0;
		this->QList<T>::operator=(other);
	}

	MutexQList<T> & operator =(const MutexQList & other) {
		this->QList<T>::operator=(other);
		is_modified_f = true;
		return *this;
	}

private:

#if QT_VERSION < 0x050e00
	QMutex * mutex;						///< Mutex, mit dessen Hilfe die Liste gelockt wird
#else
	QRecursiveMutex * mutex;
#endif
	bool is_modified_f;					///< Liste ist modifiziert
	QString name;						///< Listenname
	int lock_cnt;
	int ci,co;

public:
	inline int lockCnt() {return lock_cnt;}

	inline void lock() {
		mutex->lock();
		lock_cnt++;
	}

	inline void unlock() {
		if (lock_cnt < 1) {
			qDebug("MutexQList unlock:: NOT_LOCKED! lockcnt:%d: %p '%s'"
					  ,lock_cnt,this,name.toLocal8Bit().data());
		} else {
			lock_cnt--;
			mutex->unlock();
		}
	}

	inline bool tryLock(int timeout) {
		if (mutex->tryLock(timeout)) {
			lock_cnt++;
			return true;
		} else {
			return false;
		}

	}

	inline bool tryLock() {
		if (mutex->tryLock()) {
			lock_cnt++;
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

	friend class MutexQListLocker;

};

class MutexQListLocker
{
private:
#if QT_VERSION < 0x050e00
	QMutex * mutexRef;
#else
	QRecursiveMutex * mutexRef;
#endif

public:
	template <typename T> MutexQListLocker ( MutexQList<T> & list)
		: mutexRef(list.mutex)
	{
		mutexRef->lock();
	}
	~MutexQListLocker()
	{
		mutexRef->unlock();
	}
};

#endif //TOOLCLASSES_H

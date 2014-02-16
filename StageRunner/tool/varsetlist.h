#ifndef VARSETLIST_H
#define VARSETLIST_H

#include <QList>
#include "varset.h"

template <typename T>
class VarSetList
{
private:
	QList <T>list;
public:
	VarSetList() {
		// Lets test if the template class is derived from VarSet and is a Pointer
		// Should cause a compilation error if not
		VarSet *test = (T)0;
		(void)test;
	}
	~VarSetList() {
		while (!list.isEmpty()) {
			delete list.takeFirst();
		}

	}

	inline const QList<T> & nativeList() const {
		return list;
	}

	inline QList<T> & nativeList() {
		return list;
	}

	inline void append(const T &t) {
		list.append(t);
	}

	inline void insert(int i, const T &t) {
		list.insert(i,t);
	}

	inline const T &at(int i) const {
		return list.at(i);
	}

	inline T & operator[] (int i) {
		return list[i];
	}

	inline int size() const {
		return list.size();
	}

	inline bool removeOne(const T &t) {
		return list.removeOne(t);
	}

	inline void removeAt(int i) {
		list.removeAt(i);
	}

	inline T takeAt(int i) {
		return list.takeAt(i);
	}

	inline T takeFirst() {
		return list.takeFirst();
	}

	inline bool isEmpty() const {
		return list.isEmpty();
	}

	inline int indexOf(const T &t, int from = 0) {
		return list.indexOf(t, from);
	}

	inline T first() {
		return list.first();
	}


};


#endif // VARSETLIST_H

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
	void *parentVoid;

public:
	VarSetList(void *parent = nullptr)
		: parentVoid(parent)
	{
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

	void clear() {
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

	inline T takeLast() {
		return list.takeLast();
	}

	inline bool isEmpty() const {
		return list.isEmpty();
	}

	inline int indexOf(const T &t, int from = 0) const {
		return list.indexOf(t, from);
	}

	inline T first() {
		return list.first();
	}
	inline T last() {
		return list.last();
	}


};


#endif // VARSETLIST_H

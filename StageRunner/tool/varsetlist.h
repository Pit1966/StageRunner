#ifndef VARSETLIST_H
#define VARSETLIST_H

#include <QList>

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

	inline void append(const T &t) {
		list.append(t);
		VarSet *set = t;
		qDebug("Member: %d: %s",list.size(),set->className().toLocal8Bit().data());
	}

	inline const T &at(int i) const {
		return list.at(i);
	}

	inline int size() {
		return list.size();
	}

	inline bool removeOne(const T &t) {
		return list.removeOne(t);
	}

	inline void removeAt(int i) {
		list.removeAt(i);
	}

	inline T &takeAt(int i) {
		return list.takeAt(i);
	}


};


#endif // VARSETLIST_H

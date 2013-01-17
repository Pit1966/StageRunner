#ifndef FXLIST_H
#define FXLIST_H

#include <QObject>
#include <QList>


class FxItem;

class FxList : public QObject
{
	Q_OBJECT
private:
	QList<FxItem*>fx_list;

public:
	FxList();
	~FxList();

	void clear();

	inline int size() const {return fx_list.size();}
	inline FxItem* at(int idx) const {return fx_list.at(idx);}
	inline FxItem & operator[] (int idx) {return *fx_list[idx];}

	bool addFxAudioSimple(const QString & path);

	FxItem * addFx(int fxtype);

signals:

public slots:

};

#endif // FXLIST_H

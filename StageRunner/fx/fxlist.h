#ifndef FXLIST_H
#define FXLIST_H

#include <QObject>
#include <QList>


class FxItem;

class FxList : public QObject
{
	Q_OBJECT
private:
	int id;						///< Becomes the register Id when registered in appCentral
	QList<FxItem*>fx_list;
	FxItem * fx_last;
	FxItem * fx_next;
	FxItem * fx_current;

public:
	FxList();
	~FxList();

	void clear();

	inline int size() const {return fx_list.size();}
	inline FxItem* at(int idx) const {return fx_list.at(idx);}
	inline FxItem & operator[] (int idx) {return *fx_list[idx];}
	inline FxItem * nextFx() {return fx_next;}
	inline FxItem * lastFx() {return fx_last;}
	inline FxItem * currentFx() {return fx_current;}
	inline void setNextFx(FxItem *nfx) {fx_next = nfx;}
	FxItem * getSequenceNext();

	inline int regId() {return id;}
	inline void setRegId(int new_id) {id = new_id;}

	bool addFxAudioSimple(const QString & path);

	FxItem * addFx(int fxtype);

signals:

public slots:

};

#endif // FXLIST_H

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

	bool auto_proceed_f;

public:
	FxList();
	~FxList();

	void clear();

	inline int size() const {return fx_list.size();}
	inline void append(FxItem* fx) {fx_list.append(fx);}
	inline void insert(int idx, FxItem* fx) {fx_list.insert(idx,fx);}
	inline FxItem* at(int idx) const {return fx_list.at(idx);}
	inline FxItem* takeAt(int idx) {return fx_list.takeAt(idx);}
	inline FxItem & operator[] (int idx) {return *fx_list[idx];}
	inline FxItem * nextFx() {return fx_next;}
	inline FxItem * lastFx() {return fx_last;}
	inline FxItem * currentFx() {return fx_current;}
	void setNextFx(FxItem *nfx);
	FxItem * stepToSequenceNext();

	inline int regId() {return id;}
	inline void setRegId(int new_id) {id = new_id;}
	inline void setAutoProceedSequence(bool state) {auto_proceed_f = state;}
	inline bool autoProceedSequence() {return auto_proceed_f;}

	bool addFxAudioSimple(const QString & path, int pos = -1);
	void moveFromTo(int srcidx, int destidx);
	bool deleteFx(FxItem *fx);

	FxItem * addFx(int fxtype);
	FxItem *findSequenceFollower(FxItem *curfx);

signals:
	void fxNextChanged(FxItem *nextfx);
	void fxListChanged();

public slots:

};

#endif // FXLIST_H

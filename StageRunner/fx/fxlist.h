#ifndef FXLIST_H
#define FXLIST_H

#include <QObject>
#include <QList>

#include "varsetlist.h"


class FxItem;

class FxList : public QObject
{
	friend class FxPlayListItem;
	friend class FxSeqItem;

	Q_OBJECT
private:
	/**
	 * Becomes the register Id when registered in appCentral
	 * This Id is unique in the system
	 */
	int regid;
	VarSetList<FxItem*>fx_list;
	FxItem * fx_last;
	FxItem * fx_next;
	FxItem * fx_current;

	bool auto_run_f;
	bool modified_f;

	FxItem * fx_parent;					///< If the list resides in a FxSequence or FxAudioPlaylist: here is the pointer

public:
	bool showColumnFadeinFlag;
	bool showColumnFadeoutFlag;
	bool showColumnIdFlag;
	bool showColumnHoldFlag;
	bool showColumnPredelayFlag;
	bool showColumnPostdelayFlag;

protected:
	int myLoopTimes;
	bool myAutoProceedFlag;
	bool myLoopFlag;
	bool myRandomizedFlag;

public:
	FxList(FxItem *parentFx = 0);
	~FxList();

	void clear();

	inline VarSetList<FxItem*> & nativeFxList() {return fx_list;}
	inline int size() const {return fx_list.size();}
	inline void append(FxItem* fx) {fx_list.append(fx);}
	inline void insert(int idx, FxItem* fx) {fx_list.insert(idx,fx);}
	inline FxItem* at(int idx) const {return fx_list.at(idx);}
	inline FxItem* takeAt(int idx) {return fx_list.takeAt(idx);}
	inline bool removeOne(FxItem *fx) {return fx_list.removeOne(fx);}
	inline FxItem & operator[] (int idx) {return *fx_list[idx];}
	inline FxItem * nextFx() {return fx_next;}
	inline FxItem * lastFx() {return fx_last;}
	inline FxItem * currentFx() {return fx_current;}
	inline void setParentFx(FxItem *fx) {fx_parent = fx;}
	inline FxItem * parentFx() const {return fx_parent;}
	FxItem * stepToSequenceNext();
	FxItem * getFxByKeyCode(int keycode) const;
	FxItem * getFxByListIndex(int idx) const;
	FxItem * getFirstFx() const {return getFxByListIndex(0);}

	inline int regId() const {return regid;}
	inline void setRegId(int new_id) {regid = new_id;}
	inline void setAutoProceedSequence(bool state) {myAutoProceedFlag = state;}
	inline bool autoProceedSequence() {return myAutoProceedFlag;}
	inline void setLoopList(bool state) {myLoopFlag = state;}
	inline bool isLooped() const {return myLoopFlag;}
	inline void setRandomized(bool state) {myRandomizedFlag = state;}
	inline bool isRandomized() const {return myRandomizedFlag;}
	int loopTimes() const;
	void setLoopTimes(int loops);

	bool addFxAudioSimple(const QString & path, int pos = -1);
	bool addFxScene(int tubes = 1);
	bool addFxAudioPlayList();
	bool addFxSequence();
	void moveFromTo(int srcidx, int destidx);
	bool deleteFx(FxItem *fx);
	bool contains(FxItem *fx);
	bool isModified() const;
	void setModified(bool state);
	void cloneSelectedSceneItem();
	void resetFxItems(FxItem *skipFx = 0);
	void resetFxItemsForNewExecuter();

	FxItem * addFx(int fxtype, int option = -1);
	FxItem * findSequenceFollower(FxItem *curfx = 0);
	FxItem * findSequenceForerunner(FxItem *curfx = 0);
	FxItem * findSequenceRandomFxItem();

	QList<FxItem *> getFxListByKeyCode(int keycode) const;
	void postLoadProcess();

signals:
	void fxNextChanged(FxItem *nextfx);
	void fxListChanged();
	void fxCurrentChanged(FxItem *curfx, FxItem *oldcurfx);

public slots:
	void setNextFx(FxItem *nfx);
	void setCurrentFx(FxItem *curfx);

};

#endif // FXLIST_H

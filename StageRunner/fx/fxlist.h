#ifndef FXLIST_H
#define FXLIST_H

#include <QObject>
#include <QList>

#include "../tool/varsetlist.h"


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
	VarSetList<FxItem*>m_fxList;
	FxItem * m_fxLast;
	FxItem * m_fxNext;
	FxItem * m_fxCurrent;

	bool m_isModified;
	bool m_isProtected;					///< if Flag is set Drag and Move is disabled

	FxItem * m_fxParent;				///< If the list resides in a FxSequence or FxAudioPlaylist: here is the pointer

public:
	bool showColumnFadeinFlag;
	bool showColumnFadeoutFlag;
	bool showColumnIdFlag;
	bool showColumnHoldFlag;
	bool showColumnPredelayFlag;
	bool showColumnPostdelayFlag;
	bool showColumnKeyFlag;
	bool showColumnMoveFlag;

protected:
	int myLoopTimes;
	bool myLoopFlag;
	bool myAutoProceedFlag;
	bool myRandomizedFlag;

public:
	FxList(FxItem *parentFx = 0);
	FxList(const FxList &o);
	~FxList();

	void copyFrom(const FxList &o);

	void clear();

	inline VarSetList<FxItem*> & nativeFxList() {return m_fxList;}
	inline int size() const {return m_fxList.size();}
	inline void append(FxItem* fx) {m_fxList.append(fx);}
	inline void insert(int idx, FxItem* fx) {m_fxList.insert(idx,fx);}
	inline FxItem* at(int idx) const {return m_fxList.at(idx);}
	inline FxItem* takeAt(int idx) {return m_fxList.takeAt(idx);}
	inline bool removeOne(FxItem *fx) {return m_fxList.removeOne(fx);}
	inline FxItem & operator[] (int idx) {return *m_fxList[idx];}
	inline FxItem * nextFx() {return m_fxNext;}
	inline FxItem * lastFx() {return m_fxLast;}
	inline FxItem * currentFx() {return m_fxCurrent;}
	inline void setParentFx(FxItem *fx) {m_fxParent = fx;}
	inline FxItem * parentFx() const {return m_fxParent;}
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
	inline void setProtected(bool state) {m_isProtected = state;}
	inline bool isProtected() const {return m_isProtected;}
	int loopTimes();
	void setLoopTimes(int loops);
	void emitListChangedSignal();

	bool addFxAudioSimple(const QString & path, int pos = -1);
	bool addFxScene(int tubes = 1, FxItem **addedFxPointer = 0);
	bool addFxAudioPlayList();
	bool addFxSequence();
	bool addFxScript();
	bool addFxCue();
	void moveFromTo(int srcidx, int destidx);
	bool deleteFx(FxItem *fx);
	bool contains(FxItem *fx);
	bool isModified() const;
	void setModified(bool state);
	void cloneSelectedSceneItem();
	void cloneSelectedSeqItem();
	void resetFxItems(FxItem *skipFx = 0);
	void resetFxItemsForNewExecuter();

	FxItem * addFx(int fxtype, int option = -1);
	void addFx(FxItem *newfx);
	FxItem * findSequenceFollower(FxItem *curfx = 0);
	FxItem * findSequenceForerunner(FxItem *curfx = 0);
	FxItem * findSequenceRandomFxItem();
	FxItem * findSequenceFirstItem();

	QList<FxItem *> getFxListByKeyCode(int keycode) const;
	void postLoadProcess();
	bool postLoadResetScenes();

private:
	void init();

signals:
	void fxNextChanged(FxItem *nextfx);
	void fxListChanged();
	void fxCurrentChanged(FxItem *curfx, FxItem *oldcurfx);

public slots:
	void setNextFx(FxItem *nfx);
	void setCurrentFx(FxItem *curfx);

};

#endif // FXLIST_H

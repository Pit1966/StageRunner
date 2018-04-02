#ifndef FXITEM_H
#define FXITEM_H

#include "../tool/varset.h"
#include "../system/commandsystem.h"

#include <QString>
#include <QList>
#include <QAtomicInt>


/**
 * @brief The FxType enum
 *
 * @note There is an additional type definition in PrefVar (prefvar.h). You will need this to save/load the fx
 */
enum FxType {
	FX_NULL,				// no fx type set
	FX_AUDIO,				// is Audio Fx
	FX_SCENE,				// is Light Scene
	FX_AUDIO_PLAYLIST,		// is Audio Play List
	FX_SEQUENCE,			// is a Sequence of FxItems
	FX_CLIP,				// is a video clip (or audio)
	FX_SCRIPT,				// is a script file
	Fx_CUE,					// is a cue list for existing other fx types

	FX_SIZE
};

enum FxSearchMode {
    FXSM_EXACT,
    FXSM_LIKE,
    FXSM_LEFT,
    FXSM_RIGHT

};

class FxList;
class FxItem;

typedef QList<FxItem*> FxItemList;


class FxItem : public VarSet
{
private:
	static QList<FxItem*>*global_fx_list;
    static QMutex global_fx_lock;

protected:
	QAtomicInt refCount;
	qint32 myId;
	qint32 myFxType;
	QString myName;
	QString myFile;
	QString myPath;
	qint32 myKey;
	qint32 defaultFadeInTime;
	qint32 defaultFadeOutTime;
	qint32 defaultHoldTime;
	qint32 defaultPreDelay;
	qint32 defaultPostDelay;
	qint32 defaultMoveTime;
	qint32 hookedToInputUniverse;
	qint32 hookedToInputDmxChannel;

	FxList *myParentFxList;
	bool playedInRandomList;

    FxItem *m_isTempCopyOf;				///< FX is temporary copy (maybe a work copy);

public:
	FxItem();
	FxItem(FxList *fxList);
	FxItem(const FxItem &o);
	~FxItem();

	static bool exists(FxItem *item);
	static inline QList<FxItem*> & globalFxList() {return *global_fx_list;}
	static FxItem * findFxById(qint32 id);
    static QList<FxItem*> findFxByName(const QString &name, FxSearchMode mode = FXSM_EXACT);

	inline void setParentFxList(FxList *fxList) {myParentFxList = fxList;}
	inline FxList * parentFxList() const {return myParentFxList;}

    inline void setIsTempCopyOf(FxItem *other) {m_isTempCopyOf = other;}
    inline bool isTempCopy() const {return m_isTempCopyOf != 0;}
    FxItem *tempCopyOrigin() const {return m_isTempCopyOf;}
	static FxItemList getTempCopiesOfFx(FxItem *fx);

	FxItem * parentFxItem();

	inline int fxType() const {return myFxType;}
	inline const QString & name() const {return myName;}
	void setName(const QString &name);
	inline int id() const {return myId;}
	inline const QString & filePath() const {return myPath;}
	inline const QString & fileName() const {return myFile;}
	void setKeyCode(int code);
	inline int keyCode() const {return myKey;}

	bool isHookedToInput(qint32 universe, qint32 channel);
	inline qint32 hookedUniverse() {return hookedToInputUniverse;}
	inline qint32 hookedChannel() {return hookedToInputDmxChannel;}
	void hookToInput(qint32 universe, qint32 channel);
	inline void hookToUniverse(qint32 universe) {hookedToInputUniverse = universe;}
	inline void hookToChannel(qint32 channel) {hookedToInputDmxChannel = channel;}
	inline bool playedRandom() const {return playedInRandomList;}

	virtual void initForSequence() {;}
	virtual qint32 fadeInTime() {return defaultFadeInTime;}
	virtual	void setFadeInTime(qint32 val);
	virtual qint32 moveTime() {return defaultMoveTime;}
	virtual void setMoveTime(qint32 val);
	virtual qint32 fadeOutTime() {return defaultFadeOutTime;}
	virtual void setFadeOutTime(qint32 val);
	virtual qint32 preDelay() {return defaultPreDelay;}
	virtual void setPreDelay(qint32 val);
	virtual qint32 postDelay() {return defaultPostDelay;}
	virtual void setPostDelay(qint32 val);
	virtual qint32 holdTime() {return defaultHoldTime;}
	virtual void setHoldTime(qint32 val);
	virtual qint32 loopValue() const = 0;
	virtual void setLoopValue(qint32 val) = 0;
	virtual bool isRandomized() {return false;}
	virtual void setRandomized(bool state) {Q_UNUSED(state);}
	virtual void resetFx() = 0;
	virtual QString widgetPosition() {return QString();}
	virtual void setWidgetPosition(const QString &geometry) {Q_UNUSED(geometry);}

private:
	void init();
	int init_generate_id();

	friend class FxList;
	friend class FxItemTool;
};


#endif // FXITEM_H

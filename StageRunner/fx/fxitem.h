#ifndef FXITEM_H
#define FXITEM_H

#include "../tool/varset.h"
#include "../system/commandsystem.h"

#include <QString>
#include <QList>
#include <QAtomicInt>

enum FxType {
	FX_NULL,				// no fx type set
	FX_AUDIO,				// is Audio Fx
	FX_SCENE,				// is Light Scene
	FX_AUDIO_PLAYLIST,		// is Audio Play List

	FX_SIZE
};

class FxItem : public QObject, public VarSet
{
private:
	static QList<FxItem*>*global_fx_list;


protected:
	QAtomicInt refCount;
	qint32 myId;
	qint32 myFxType;
	QString myName;
	QString myFile;
	QString myPath;
	qint32 myKey;
	qint32 hookedToInputUniverse;
	qint32 hookedToInputDmxChannel;

public:
	FxItem();
	FxItem(const FxItem &o);
	~FxItem();
	static bool exists(FxItem *item);
	static inline QList<FxItem*> & globalFxList() {return *global_fx_list;}

	inline int fxType() const {return myFxType;}
	inline const QString & name() const {return myName;}
	inline void setName(const QString &name) {myName = name;}
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

private:
	void init();
	int init_generate_id();

	friend class FxList;
	friend class FxItemTool;
};

#endif // FXITEM_H

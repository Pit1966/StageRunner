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

	FX_SIZE
};

class FxItem : public VarSet
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

public:
	FxItem();
	~FxItem();
	static bool exists(FxItem *item);
	static inline QList<FxItem*> & globalFxList() {return *global_fx_list;}

	inline int fxType() const {return myFxType;}
	inline const QString & name() const {return myName;}
	inline void setName(const QString &name) {myName = name;}
	inline int id() const {return myId;}
	inline const QString & filePath() const {return myPath;}
	inline const QString & fileName() const {return myFile;}
	inline void setKeyCode(ushort code) {myKey = code;}
	inline ushort keyCode() const {return myKey;}


private:
	void init(qint32 id);

	friend class FxList;
};

#endif // FXITEM_H

#ifndef FXITEM_H
#define FXITEM_H

#include "../tool/prefvarset.h"
#include "../system/commandsystem.h"

#include <QString>
#include <QList>
#include <QAtomicInt>

enum FxType {
	FX_NULL,				// no fx type set
	FX_AUDIO,				// is Audio Fx

	FX_SIZE
};

class FxItem : public VarSet
{
private:
	static QList<FxItem*>*global_fx_list;


protected:
	QAtomicInt refCount;
	qint32 myId;
	qint32 myType;
	QString myName;
	QString myFile;
	QString myPath;

public:
	FxItem();
	~FxItem();
	static bool exists(FxItem * item);

	inline int fxType() const {return myType;}
	inline const QString & displayName() const {return myName;}
	inline int fxID() const {return myId;}
	inline const QString & filePath() const {return myPath;}
	inline const QString & fileName() const {return myFile;}


private:
	void init(qint32 fxID);

	friend class FxList;
};

#endif // FXITEM_H

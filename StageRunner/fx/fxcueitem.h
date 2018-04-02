#ifndef FXCUEITEM_H
#define FXCUEITEM_H

#include "fxitem.h"

#include <QtGlobal>

class FxList;

class FxCueItem : public FxItem
{
private:
	bool m_init;

public:
	FxCueItem(FxList *fxList);
	FxCueItem(const FxCueItem &o);
	~FxCueItem();

	qint32 loopValue() const;
	void setLoopValue(qint32 val);
	void resetFx();
};

#endif // FXCUEITEM_H

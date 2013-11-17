#ifndef FXSEQITEM_H
#define FXSEQITEM_H

#include "fxitem.h"

class FxList;
class FxItemObj;

class FxSeqItem : public FxItem
{
public:
	pstring widgetPos;
	FxList *seqList;

private:
	FxItemObj *itemObj;

public:
	FxSeqItem();
	~FxSeqItem();

	qint32 fadeInTime();
	void setFadeInTime(qint32 val);
	qint32 fadeOutTime();
	void setFadeOutTime(qint32 val);



private:
	void init();
};

#endif // FXSEQITEM_H

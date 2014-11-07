#ifndef FXSEQITEM_H
#define FXSEQITEM_H

#include "fxitem.h"

class FxList;
class FxItemObj;

class FxSeqItem : public FxItem
{
public:
	FxList *seqList;
	pbool blackOtherSeqOnStart;
	pbool stopOtherSeqOnStart;

private:

	pstring widgetPos;
	FxItemObj *itemObj;

public:
	FxSeqItem(FxList *fxList);
	~FxSeqItem();

	qint32 fadeInTime();
	void setFadeInTime(qint32 val);
	qint32 fadeOutTime();
	void setFadeOutTime(qint32 val);
	qint32 loopValue();
	void setLoopValue(qint32 val);
	void resetFx();
	bool isRandomized();
	void setRandomized(bool state);
	QString widgetPosition() {return widgetPos;}
	void setWidgetPosition(const QString & geometry) {widgetPos = geometry;}


private:
	void init();
};

#endif // FXSEQITEM_H

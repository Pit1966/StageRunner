#ifndef FXPLAYLISTITEM_H
#define FXPLAYLISTITEM_H

#include "fxitem.h"
#include "fxaudioitem.h"
#include "varsetlist.h"

class FxList;
class FxItemObj;


class FxPlayListItem : public FxAudioItem
{
public:
	FxList *fxPlayList;
private:
	pstring widgetPos;

	FxItemObj *itemObj;

public:
	FxPlayListItem(FxList *fxList);
	~FxPlayListItem();

	void setLoopValue(qint32 val);
	qint32 loopValue() const;
	bool isRandomized();
	void setRandomized(bool state);


	bool addAudioTrack(const QString & path);
	int size();
	void continuePlay(FxItem *fx, CtrlCmd cmd, Executer *exec);
	inline FxItemObj * connector() {return itemObj;}
	QString widgetPosition() {return widgetPos;}
	void setWidgetPosition(const QString & geometry) {widgetPos = geometry;}

private:
	void init();

};

#endif // FXPLAYLISTITEM_H

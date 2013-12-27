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
	pstring widgetPos;
	FxList *fxPlayList;

private:
	FxItemObj *itemObj;

public:
	FxPlayListItem(FxList *fxList);
	~FxPlayListItem();

	void setLoopValue(qint32 val);
	qint32 loopValue();
	bool isRandomized();
	void setRandomized(bool state);


	bool addAudioTrack(const QString & path);
	int size();
	void continuePlay(FxItem *fx, CtrlCmd cmd, Executer *exec);
	inline FxItemObj * connector() {return itemObj;}

private:
	void init();

};

#endif // FXPLAYLISTITEM_H

#ifndef FXPLAYLISTITEM_H
#define FXPLAYLISTITEM_H

#include "fxitem.h"
#include "fxaudioitem.h"
#include "varsetlist.h"

#include <QObject>

class FxList;


class FxPlayListItem : public FxAudioItem
{
	Q_OBJECT
public:
	FxList *fxPlayList;

public:
	FxPlayListItem();
	~FxPlayListItem();
	bool addAudioTrack(const QString & path);
	int size();

private:
	void init();

public slots:
	void continuePlay(FxItem *fx, CtrlCmd cmd, Executer *exec);
};

#endif // FXPLAYLISTITEM_H

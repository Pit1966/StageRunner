#ifndef FXITEMOBJ_H
#define FXITEMOBJ_H

#include <QObject>
#include "commandsystem.h"

class FxItem;
class Executer;

class FxItemObj : public QObject
{
	Q_OBJECT
private:
	FxItem *parentFx;

public:
	FxItemObj(FxItem *fx);

signals:

public slots:
	void playFxPlayList(FxItem *fx, CtrlCmd cmd, Executer *exec);


};

#endif // FXITEMOBJ_H

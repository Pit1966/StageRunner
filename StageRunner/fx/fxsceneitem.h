#ifndef FXSCENEITEM_H
#define FXSCENEITEM_H

#include "fxitem.h"
#include "varsetlist.h"

class DmxChannel;
class QWidget;

class FxSceneItem : public FxItem
{
public:
	qint32 defaultFadeInTime;
	qint32 defaultFadeOutTime;
	VarSetList<DmxChannel*>tubes;

public:
	FxSceneItem();
	void createDefaultTubes(int tubecount);

private:
	void init();
};

#endif // FXSCENEITEM_H
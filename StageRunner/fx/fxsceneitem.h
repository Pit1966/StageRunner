#ifndef FXSCENEITEM_H
#define FXSCENEITEM_H

#include "fxitem.h"
#include "varsetlist.h"
#include "commandsystem.h"

class DmxChannel;
class QWidget;

class FxSceneItem : public FxItem
{
public:
	qint32 defaultFadeInTime;
	qint32 defaultFadeOutTime;
	DmxChannel *sceneMaster;
	VarSetList<DmxChannel*>tubes;

private:
	bool is_live;
	bool is_active;

public:
	FxSceneItem();
	~FxSceneItem();

	void createDefaultTubes(int tubecount);
	void setTubeCount(int tubecount);
	inline int tubeCount() const {return tubes.size();}

	bool initSceneCommand(CtrlCmd cmd);
	bool loopFunction();
	inline bool isActive() const {return is_active;}
	inline bool isIdle() const {return !is_active;}
	inline bool isLive() const {return is_live;}

private:
	void init();
};

#endif // FXSCENEITEM_H

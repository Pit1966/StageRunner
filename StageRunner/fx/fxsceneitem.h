#ifndef FXSCENEITEM_H
#define FXSCENEITEM_H

#include "fxitem.h"
#include "varsetlist.h"
#include "commandsystem.h"

class DmxChannel;
class QWidget;

using namespace LIGHT;

class FxSceneItem : public FxItem
{
public:
	qint32 defaultFadeInTime;
	qint32 defaultFadeOutTime;
	DmxChannel *sceneMaster;				///< Not used
	VarSetList<DmxChannel*>tubes;

protected:
	quint32 myStatus;

private:
	quint32 my_last_status;
	bool wasBlacked;

public:
	FxSceneItem();
	FxSceneItem(const FxSceneItem &o);
	~FxSceneItem();

	void createDefaultTubes(int tubecount);
	void setTubeCount(int tubecount);
	inline int tubeCount() const {return tubes.size();}
	DmxChannel *tube(int id);

	bool initSceneCommand(CtrlCmd cmd, int cmdTime = 0);
	bool directFadeToDmx(qint32 dmxval, qint32 time_ms);
	bool loopFunction();
	void setLive(bool state);
	void setBlacked(bool state);
	bool isBlacked() const {return wasBlacked;}
	inline bool isActive() const {return myStatus & SCENE_ACTIVE;}
	inline bool isIdle() const {return (myStatus == SCENE_IDLE);}
	inline bool isLive() const {return myStatus & SCENE_LIVE;}
	inline bool isOnStage() const {return myStatus & SCENE_STAGE;}
	inline bool isVisible() const {return myStatus & (SCENE_STAGE | SCENE_ACTIVE | SCENE_LIVE);}
	bool statusHasChanged();
	inline quint32 status() {return myStatus;}
	bool postLoadInitTubes(bool restore_light);

private:
	void init();
};

#endif // FXSCENEITEM_H

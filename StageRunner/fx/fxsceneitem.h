#ifndef FXSCENEITEM_H
#define FXSCENEITEM_H

#include "fxitem.h"
#include "varsetlist.h"
#include "commandsystem.h"

#ifndef MIX_LINES
#define MIX_INTERN 0
#define MIX_EXTERN 1
#define MIX_LINES 2
#endif

class DmxChannel;
class QWidget;

using namespace LIGHT;

class FxSceneItem : public FxItem
{
public:
	DmxChannel *sceneMaster;				///< Not used
	VarSetList<DmxChannel*>tubes;

protected:
	quint32 myStatus;
	SceneSeqState mySeqStatus;

private:
	quint32 my_last_status;
	bool my_last_active_flag;
	bool wasBlacked[MIX_LINES];

public:
	FxSceneItem();
	FxSceneItem(const FxSceneItem &o);
	~FxSceneItem();

	qint32 loopValue() {return 0;}
	void setLoopValue(qint32 val);

	void createDefaultTubes(int tubecount);
	void setTubeCount(int tubecount);
	inline int tubeCount() const {return tubes.size();}
	DmxChannel *tube(int id);

	bool initSceneCommand(int mixline, CtrlCmd cmd, int cmdTime = 0);
	bool directFadeToDmx(qint32 dmxval, qint32 time_ms);
	bool loopFunction();
	void setLive(bool state);
	void setBlacked(int mixline, bool state);
	bool isBlacked(int mixline) const {return wasBlacked[mixline];}
	inline bool isActive() const {return myStatus & (SCENE_ACTIVE_INTERN | SCENE_ACTIVE_EXTERN);}
	inline bool isIdle() const {return (myStatus == SCENE_IDLE);}
	inline bool isLive() const {return myStatus & SCENE_STAGE_LIVE;}
	inline bool isOnStageIntern() const {return myStatus & SCENE_STAGE_INTERN;}
	inline bool isOnStageExtern() const {return myStatus & SCENE_STAGE_EXTERN;}
	inline bool isVisible() const {return myStatus & (SCENE_STAGE_INTERN | SCENE_STAGE_EXTERN | SCENE_ACTIVE_INTERN | SCENE_ACTIVE_EXTERN | SCENE_STAGE_LIVE);}
	bool getClearStatusHasChanged();
	bool getClearActiveHasChanged();
	inline quint32 status() const {return myStatus;}
	inline SceneSeqState seqStatus() const {return mySeqStatus;}
	inline void setSeqStatus(SceneSeqState status) {mySeqStatus = status;}
	QString statusString();
	bool postLoadInitTubes(bool restore_light);

private:
	void init();
};

#endif // FXSCENEITEM_H

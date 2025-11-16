//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef FXSCENEITEM_H
#define FXSCENEITEM_H

#include "fxitem.h"
#include "tool/varsetlist.h"
#include "system/commandsystem.h"

#ifndef MIX_LINES
#define MIX_INTERN 0
#define MIX_EXTERN 1
#define MIX_LINES 2
#endif

class DmxChannel;
class QWidget;
class FxList;

using namespace LIGHT;

class FxSceneItem : public FxItem
{
	Q_OBJECT

public:
	DmxChannel *sceneMaster;				///< Not used
	VarSetList<DmxChannel*>tubes;

protected:

	pstring widgetPos;
	mutable quint32 myStatus;
	SceneSeqState mySeqStatus;

private:
	uint mLastStatus;
	bool mLastActiveFlag;
	bool m_wasBlacked[MIX_LINES];
	bool m_deleteMeOnFinished;

public:
	FxSceneItem();
	FxSceneItem(FxList *fxList);
	FxSceneItem(const FxSceneItem &o);

	void cloneFrom(const FxSceneItem &o);

	qint32 loopValue() const override{return 0;}
	void setLoopValue(qint32 val) override;
	void resetFx() override;
	bool isUsed() const override;
	QString widgetPosition() const override {return widgetPos;}
	void setWidgetPosition(const QString & geometry) override {widgetPos = geometry;}
	qint32 durationHint() const override;

	qint32 getNewTubeId() const;
	qint32 getNextUnusedDmxChannel(int universe) const;
	qint32 guessUniverse() const;
	void createDefaultTubes(int tubecount, uint universe = 0);
	void setTubeCount(int tubecount, int defaultUniverse);
	void addTube();
	inline int tubeCount() const {return tubes.size();}
	DmxChannel *tube(int idx) const;
	DmxChannel *findTube(int tubeId) const;
	DmxChannel *findTube(int univ, int dmxchan) const;
	bool removeTubeById(int tubeId);
	bool removeTube(DmxChannel *tube);

	bool initSceneCommand(int mixline, CtrlCmd cmd, int cmdTime = 0);
	bool directFadeToDmx(qint32 dmxval, qint32 time_ms);
	bool loopFunction();
	void setLive(bool state) const;
	void setBlacked(int mixline, bool state);
	bool isBlacked(int mixline) const {return m_wasBlacked[mixline];}
	inline bool isActive() const {return myStatus & (SCENE_ACTIVE_INTERN | SCENE_ACTIVE_EXTERN);}
	inline bool isActiveIntern() const {return myStatus & SCENE_ACTIVE_INTERN;}
	inline bool isIdle() const {return (myStatus == SCENE_IDLE);}
	inline bool isLive() const {return myStatus & SCENE_STAGE_LIVE;}
	bool isOnStageIntern() const;
	inline bool isOnStageExtern() const {return myStatus & SCENE_STAGE_EXTERN;}
	bool evaluateOnStageIntern() const;
	inline bool isVisible() const {return myStatus & (SCENE_STAGE_INTERN | SCENE_STAGE_EXTERN | SCENE_ACTIVE_INTERN | SCENE_ACTIVE_EXTERN | SCENE_STAGE_LIVE);}
	inline void setDeleteOnFinished() {m_deleteMeOnFinished = true;}
	inline bool deleteOnFinished() const {return m_deleteMeOnFinished;}
	bool getClearStatusHasChanged();
	bool getClearActiveHasChanged();
	inline quint32 status() const {return myStatus;}
	inline void setStatus(quint32 stat) {myStatus = stat;}
	inline void setActiveIntern() {myStatus |= SCENE_ACTIVE_INTERN;}
	inline SceneSeqState seqStatus() const {return mySeqStatus;}
	inline void setSeqStatus(SceneSeqState status) {mySeqStatus = status;}
	QString statusString();
	bool postLoadInitTubes(bool restore_light);
	bool updateSceneFromOlderProjectVersion(int oldVer);

	bool isEqual(const FxSceneItem *o) const;

private:
	~FxSceneItem();
	void init();

	friend class AppCentral;
	friend class SceneDeskWidget;
};

#endif // FXSCENEITEM_H

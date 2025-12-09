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

#include "fxlist.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxplaylistitem.h"
#include "fxseqitem.h"
#include "fxscriptitem.h"
#include "fxcueitem.h"
#include "fxtimelineitem.h"
#include "log.h"
#include "fxitemtool.h"

#include <QMutableListIterator>
#include <QDateTime>
#if QT_VERSION >= 0x050a00
#  include <QRandomGenerator>
#endif

FxList::FxList(FxItem *parentFx)
	: QObject()
	, m_fxList(this)
{
	m_fxParent = parentFx;

	init();
}

FxList::FxList(const FxList &o)
	: QObject()
	, m_fxList(this)
{
	m_fxParent = o.m_fxParent;

	copyFrom(o);
}

FxList::~FxList()
{
	clear();
}

void FxList::init()
{
	regid = 0;

	m_fxLast = 0;
	m_fxNext = 0;
	m_fxPrev = 0;
	m_fxCurrent = 0;
	m_isModified = false;

	myAutoProceedFlag = false;
	myRandomizedFlag = false;
	myLoopTimes = 1;
	myLoopFlag = false;
	m_isProtected = false;

	showColumnFadeinFlag = false;
	showColumnFadeoutFlag = false;
	showColumnIdFlag = false;
	showColumnHoldFlag = false;
	showColumnPredelayFlag = false;
	showColumnPostdelayFlag = false;
	showColumnKeyFlag = false;
	showColumnMoveFlag = false;
}

/**
 * @brief FxList::copyFrom
 * @param o
 * @param exactClone 1: clone keyCodes in FxItems too!, 2: Additionally keep all FxItem IDs
 * @return
 *
 * @note By default keyCodes and fxIDs are not cloned
 */
bool FxList::copyFrom(const FxList &o, int exactClone, FxIdMap *oldNewIdMap)
{
	bool ok = true;

	regid = 0;

	m_fxLast = 0;
	m_fxNext = 0;
	m_fxCurrent = 0;
	m_isModified = true;

	myAutoProceedFlag = o.myAutoProceedFlag;
	myRandomizedFlag = o.myRandomizedFlag;
	myLoopTimes = o.myLoopTimes;
	myLoopFlag = o.myLoopFlag;
	m_isProtected = o.m_isProtected;

	showColumnFadeinFlag = o.showColumnFadeinFlag;
	showColumnFadeoutFlag = o.showColumnFadeoutFlag;
	showColumnIdFlag = o.showColumnIdFlag;
	showColumnHoldFlag = o.showColumnHoldFlag;
	showColumnPredelayFlag = o.showColumnPredelayFlag;
	showColumnPostdelayFlag = o.showColumnPostdelayFlag;
	showColumnKeyFlag = o.showColumnKeyFlag;
	showColumnMoveFlag = o.showColumnMoveFlag;

	// Copy Sequence FX items
	for (int t=0; t<o.m_fxList.size(); t++) {
		FxItem *newfx = FxItemTool::cloneFxItem(o.m_fxList.at(t), false, exactClone, oldNewIdMap);
		if (newfx) {
			m_fxList.append(newfx);
		} else {
			ok = false;
		}
	}

	return ok;
}

void FxList::clear()
{
	m_fxLast = 0;
	m_fxNext = 0;
	m_fxCurrent = 0;

	while (!m_fxList.isEmpty()) {
		m_isModified = true;
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = m_fxList.takeFirst();
		if (fx->refCount == 0) {
			qDebug() << "delete project fx" << fx->name() << "unreferenced!!";
			delete fx;
		}
		else if (!fx->refCount.deref()) {
			delete fx;
		}
		else {
			qDebug() << "delete project fx" << fx->name() << "ref count:" << fx->refCount;
		}
	}
}

void FxList::setNextFx(FxItem *nfx)
{
	if (nfx != m_fxNext) {
		m_fxNext = nfx;
		emit fxNextChanged(m_fxNext);
	}
}

void FxList::setPrevFx(FxItem *nfx)
{
	if (nfx != m_fxPrev) {
		m_fxPrev = nfx;
		emit fxPrevChanged(m_fxPrev);
	}
}

void FxList::setCurrentFx(FxItem *curfx)
{
	if (!FxItem::exists(curfx)) curfx = 0;

	if (m_fxCurrent != curfx) {
		emit fxCurrentChanged(curfx, m_fxCurrent);
		m_fxCurrent = curfx;
	}
}

FxItem *FxList::stepToSequenceNext()
{
	if (!m_fxNext) return 0;

	m_fxLast = m_fxCurrent;
	if (m_fxCurrent != m_fxNext) {
		emit fxCurrentChanged(m_fxNext, m_fxCurrent);
		m_fxCurrent = m_fxNext;
	}

	// Now check if auto proceed is selected and find next entry in list
	if (myAutoProceedFlag) {
		setNextFx( findSequenceFollower(m_fxCurrent) );
		setPrevFx( findSequenceForerunner(m_fxCurrent) );
	}
	return m_fxCurrent;
}

FxItem *FxList::stepToSequencePrev()
{
	if (!m_fxPrev)
		return 0;

	m_fxLast = m_fxCurrent;
	if (m_fxCurrent != m_fxPrev) {
		emit fxCurrentChanged(m_fxPrev, m_fxCurrent);
		m_fxCurrent = m_fxPrev;
	}

	// Now check if auto proceed is selected and find next entry in list
	if (myAutoProceedFlag) {
		setNextFx( findSequenceFollower(m_fxCurrent) );
		setPrevFx( findSequenceForerunner(m_fxCurrent) );
	}
	return m_fxCurrent;
}

FxItem *FxList::getFxByKeyCode(int keycode) const
{
	QListIterator<FxItem*> it(m_fxList.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->keyCode() == keycode) return fx;
	}
	return 0;
}

FxItem *FxList::getFxByListIndex(int idx) const
{
	if (idx < m_fxList.nativeList().size()) {
		return m_fxList.nativeList().at(idx);
	}
	return 0;
}

int FxList::indexOf(const FxItem *fx) const
{
	return m_fxList.indexOf(const_cast<FxItem *>(fx));
}

int FxList::loopTimes()
{
	if (m_fxParent) {
		switch (m_fxParent->fxType()) {
		case FX_AUDIO_PLAYLIST:
		case FX_SEQUENCE:
			myLoopTimes = m_fxParent->loopValue();
			break;
		default:
			break;
		}
	}
	return myLoopTimes;
}

void FxList::setLoopTimes(int loops)
{
	myLoopTimes = loops;
}

void FxList::emitListChangedSignal()
{
	emit fxListChanged();
}

/**
 * @brief Returns a list with FXs that contains the desired key code
 * @param keycode
 * @return QList<FxItem*> List of fx items that match the key code (maybe empty if code not found)
 */
QList<FxItem*>FxList::getFxListByKeyCode(int keycode) const
{
	QList<FxItem*>fxlist;
	QListIterator<FxItem*> it(m_fxList.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->keyCode() == keycode) fxlist.append(fx);
	}
	return fxlist;
}

/**
 * @brief Process FxList after Load from File
 *
 * This sets the parentFxList Pointer for any member of the list
 */
void FxList::postLoadProcess()
{
	for (int t=0; t<m_fxList.size(); t++) {
		FxItem *fx = m_fxList.at(t);
		fx->setParentFxList(this);
		switch (fx->fxType()) {
		case FX_AUDIO:
			reinterpret_cast<FxAudioItem*>(fx)->checkForVideoClip();
			break;
		case FX_AUDIO_PLAYLIST:
			reinterpret_cast<FxPlayListItem*>(fx)->fxPlayList->postLoadProcess();
			break;
		case FX_SEQUENCE:
			reinterpret_cast<FxSeqItem*>(fx)->seqList->postLoadProcess();
		default:
			break;
		}
	}
}

/**
 * @brief Reset Scene output values after loading a project
 * @return  true, a scene was on stage
 *
 * When saving the project at the same time all current scene (tubes) output values are stored as well.
 * Loading this scenes will restore these states. That could cause problems on fading in the scene (cause
 * it maybe already faded to the target values)
 *
 * This function calls itself recursively for all FxSequence members of the list
 */
bool FxList::postLoadResetScenes()
{
	bool was_on_stage = false;
	for (int t=0; t<size(); t++) {
		FxItem *fx = at(t);
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
			/// @todo: Light restore on load funktioniert nicht
			bool scene_was_on_stage = scene->postLoadInitTubes(false);
			if (scene_was_on_stage) {
				was_on_stage = true;
				LOGTEXT(QObject::tr("Scene <font color=orange>'%1'</font> was on stage when project was saved")
						.arg(scene->name()));
			}
		}
		else if (fx->fxType() == FX_SEQUENCE) {
			// A sequence could hold scenes as well. We process them recursively
			FxSeqItem *fxseq = reinterpret_cast<FxSeqItem*>(fx);
			if (fxseq->seqList)
				was_on_stage |= fxseq->seqList->postLoadResetScenes();
		}
	}
	return was_on_stage;
}

/**
 * @brief Generate new IDs for all FxItems in the list.
 * @param from Begin ID creation with this ID value.
 * @return true, if there were no conflicts with existing IDs during creation.
 *
 * The function takes every FxItem, one after the other and tries to generate a new
 * ID based on the given "from" lowest ID value.
 * If from is > NULL, it will be incremented for each list member.
 * If the given "from" ID is already used, the next free ID is taken.
 */
bool FxList::recreateFxIDs(int from)
{
	for (FxItem *fx : m_fxList.nativeList()) {
		fx->myId = 0;
	}

	bool ok = true;

	for (FxItem *fx : m_fxList.nativeList()) {
		int newid = fx->generateNewID(from);
		if (from != newid)
			ok = false;

		if (from != 0)
			from++;
	}

	return ok;
}


/**
 * @brief Find the follower of an arbitrary FX entry in the list
 * @param curfx Pointer to the FxItem the follower is search for or NULL
 * @return Pointer to FxItem or 0, if there is no follower cause list is at tail
 *
 * If curfx parameter is NULL the current nextFx() Pointer is taken as search begin
 */
FxItem *FxList::findSequenceFollower(FxItem *curfx, bool turnover)
{
	if (m_fxList.size() < 2)
		return nullptr;

	if (!curfx)
		curfx = m_fxNext;

	FxItem *followfx = nullptr;
	int idx_cur = m_fxList.indexOf(curfx);
	if (idx_cur >= 0) {
		if (idx_cur+1 < m_fxList.size()) {
			followfx = m_fxList.at(idx_cur+1);
		}
		else if (turnover) {
			followfx = m_fxList.first();
		}
	}
	return followfx;
}

/**
 * @brief Find the Predecessor (Forerunner) of an arbitrary FX entry in the list
 * @param curfx Pointer to the FxItem the Predecessor is search for or NULL
 * @return Pointer to FxItem or 0, if there is no Predecessor cause list is at head
 *
 * If curfx parameter is NULL the current prevFx() Pointer is taken as search begin
 */
FxItem *FxList::findSequenceForerunner(FxItem *curfx, bool turnover)
{
	if (m_fxList.size() < 2)
		return nullptr;

	if (!curfx)
		curfx = m_fxPrev;

	FxItem *prevfx = 0;
	int idx_cur = m_fxList.indexOf(curfx);
	if (idx_cur >= 0) {
		if (idx_cur > 0) {
			prevfx = m_fxList.at(idx_cur-1);
		}
		else if (turnover) {
			prevfx = m_fxList.last();
		}
	}
	return prevfx;
}

FxItem *FxList::findSequenceRandomFxItem()
{
	int cnt = m_fxList.size();
	FxItem *fx = 0;
#if QT_VERSION >= 0x050a00
	// QRandomGenerator::global()->seed(QDateTime::currentDateTime().toTime_t());
#else
    qsrand(QDateTime::currentDateTime().toTime_t());
#endif
	int tries = 0;
	while (!fx && tries++ < 1000) {
#if QT_VERSION >= 0x050a00
        int rnd = QRandomGenerator::global()->generate() % cnt;
#else
        int rnd = qrand() % cnt;
#endif
        // int rnd = double(qrand()) * cnt / RAND_MAX;
		if (!m_fxList.at(rnd)->playedInRandomList) {
			fx = m_fxList.at(rnd);
			fx->playedInRandomList = true;
		}
	}

	int i = 0;
	while (!fx && i<cnt) {
		if (!m_fxList.at(i)->playedInRandomList) {
			fx = m_fxList.at(i);
			fx->playedInRandomList = true;
		}
		i++;
	}

	return fx;
}

FxItem *FxList::findSequenceFirstItem()
{
	FxItem *fx = 0;
	if (m_fxList.size())
		fx = m_fxList.first();

	return fx;
}

FxItem *FxList::findFxItemById(qint32 id)
{
	for (int i=0; i<m_fxList.size(); i++) {
		if (m_fxList.at(i)->id() == id)
			return m_fxList.at(i);
	}

	return nullptr;
}

FxItem *FxList::findFxItemBySubId(qint32 subId)
{
	for (int i=0; i<m_fxList.size(); i++) {
		if (m_fxList.at(i)->subId() == subId)
			return m_fxList.at(i);
	}

	return nullptr;
}

int FxList::countRandomPlayedItemInList() const
{
	// count random played fxitems;
	int played = 0;
	for (int i=0; i<m_fxList.size(); i++) {
		if (m_fxList.at(i)->playedInRandomList)
			played++;
	}
	return played;
}

FxAudioItem *FxList::addFxAudioSimple(const QString &path, int pos)
{
	FxAudioItem *fx = new FxAudioItem(path,this);
	fx->refCount.ref();
	if (pos < 0 || pos >= m_fxList.size()) {
		m_fxList.append(fx);
	} else {
		m_fxList.insert(pos,fx);
	}
	m_isModified = true;

	fx->checkForVideoClip();

	return fx;
}

bool FxList::addFxScene(int tubes, FxItem **addedFxPointer)
{
	FxItem *fx = addFx(FX_SCENE,tubes);
	if (fx) {
		fx->refCount.ref();
		fx->setName("New Scene");
		m_isModified = true;

		if (addedFxPointer)
			*addedFxPointer = fx;

		return true;
	}

	return false;
}

bool FxList::addFxAudioPlayList()
{
	FxItem *fx = addFx(FX_AUDIO_PLAYLIST);
	if (fx) {
		fx->refCount.ref();
		fx->setName("Audio play list");
		m_isModified = true;
		return true;
	}
	return false;
}

bool FxList::addFxSequence()
{
	FxItem *fx = addFx(FX_SEQUENCE);
	if (fx) {
		fx->refCount.ref();
		fx->setName("FX Sequence");
		m_isModified = true;
		return true;
	}
	return false;
}

bool FxList::addFxScript()
{
	FxItem *fx = addFx(FX_SCRIPT);
	if (fx) {
		fx->refCount.ref();
		fx->setName("FX Script");
		m_isModified = true;
		return true;
	}
	return false;
}


bool FxList::addFxCue()
{
	FxItem *fx = addFx(Fx_CUE);
	if (fx) {
		fx->refCount.ref();
		fx->setName("FX Cue");
		m_isModified = true;
		return true;
	}
	return false;
}

bool FxList::addFxTimeLine()
{
	FxItem *fx = addFx(FX_TIMELINE);
	if (fx) {
		fx->refCount.ref();
		fx->setName("FX TimeLine");
		m_isModified = true;
		return true;
	}
	return false;
}

void FxList::moveFromTo(int srcidx, int destidx)
{
	FxItem *xitem = 0;
	if (srcidx >= 0 && srcidx < m_fxList.size()) {
		xitem = m_fxList.takeAt(srcidx);
	} else {
		return;
	}

	if (destidx < 0) {
		m_fxList.append(xitem);
	}
	else if (srcidx < destidx) {
		m_fxList.insert(destidx,xitem);
	}
	else if (destidx <= srcidx ){
		m_fxList.insert(destidx,xitem);
	}
	m_isModified = true;

	emit fxListChanged();
}

bool FxList::deleteFx(FxItem *fx)
{
	if (fx->isUsed())
		return false;

	bool del = m_fxList.removeOne(fx);
	if (del) {
		if (fx == m_fxNext) m_fxNext = 0;
		if (fx == m_fxLast) m_fxLast = 0;
		if (fx == m_fxCurrent) {
			emit fxCurrentChanged(0,m_fxCurrent);
			m_fxCurrent = 0;
		}
		delete fx;
		emit fxListChanged();
		m_isModified = true;
	}
	return del;
}

bool FxList::contains(FxItem *fx)
{
	if (m_fxList.indexOf(fx) < 0) {
		return false;
	} else {
		return true;
	}
}

bool FxList::isModified() const
{
	bool modified = m_isModified;
	QListIterator<FxItem*> it(m_fxList.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->isModified()) {
			qDebug() << fx->name() << "is modified";
			modified = true;
		}
	}
	return modified;
}

void FxList::setModified(bool state)
{
	QMutableListIterator<FxItem*> it(m_fxList.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		fx->setModified(state);
	}
	m_isModified = state;
}

void FxList::cloneSelectedSceneItem()
{
	if (m_fxNext && m_fxNext->fxType() == FX_SCENE) {
		FxSceneItem *scene = reinterpret_cast<FxSceneItem*>(m_fxNext);
		FxSceneItem *new_scene = new FxSceneItem(*scene);
		new_scene->setKeyCode(0);
		new_scene->refCount.ref();
		int idx = m_fxList.indexOf(scene);
		if (idx >= 0) {
			m_fxList.insert(idx + 1, new_scene);
		} else {
			m_fxList.append(new_scene);
		}
		m_isModified = true;
		FxItemTool::setClonedFxName(scene,new_scene,this);

	}
}

void FxList::cloneSelectedSeqItem()
{
	if (m_fxNext && m_fxNext->fxType() == FX_SEQUENCE) {
		FxSeqItem *seq = reinterpret_cast<FxSeqItem*>(m_fxNext);
		FxSeqItem *new_seq = new FxSeqItem(*seq);
		new_seq->refCount.ref();
		int idx = m_fxList.indexOf(seq);
		if (idx >= 0) {
			m_fxList.insert(idx + 1, new_seq);
		} else {
			m_fxList.append(new_seq);
		}
		m_isModified = true;
		FxItemTool::setClonedFxName(seq,new_seq,this);
	}
}

void FxList::cloneSelectedScriptItem()
{
	if (m_fxNext && m_fxNext->fxType() == FX_SCRIPT) {
		FxScriptItem *script = reinterpret_cast<FxScriptItem*>(m_fxNext);
		FxScriptItem *new_script = new FxScriptItem(*script);
		new_script->refCount.ref();
		new_script->setKeyCode(0);
		int idx = m_fxList.indexOf(script);
		if (idx >= 0) {
			m_fxList.insert(idx + 1, new_script);
		} else {
			m_fxList.append(new_script);
		}
		m_isModified = true;
		FxItemTool::setClonedFxName(script,new_script,this);
	}
}

void FxList::cloneSelectedTimelineItem()
{
	if (!m_fxNext || m_fxNext->fxType() != FX_TIMELINE)
		return;

	FxTimeLineItem *fxt = static_cast<FxTimeLineItem*>(m_fxNext);
	FxTimeLineItem *new_fxt = new FxTimeLineItem(*fxt);
	new_fxt->refCount.ref();
	new_fxt->setKeyCode(0);
	int idx = m_fxList.indexOf(fxt);
	if (idx >= 0) {
		m_fxList.insert(idx + 1, new_fxt);
	} else {
		m_fxList.append(new_fxt);
	}
	m_isModified = true;
	FxItemTool::setClonedFxName(fxt,new_fxt,this);
}


/**
 * @brief Reset the members of the list for (sequence) start
 * @param skipFx If not NULL this FxItem will be skip in initializing (usefull for countinue)
 */
void FxList::resetFxItems(FxItem *skipFx)
{
	for (int t=0; t<m_fxList.size(); t++) {
		if (m_fxList.at(t) != skipFx) {
			m_fxList.at(t)->resetFx();
		}
	}
}

void FxList::resetFxItemsForNewExecuter()
{
	for (int t=0; t<m_fxList.size(); t++) {
		FxItem *fx = m_fxList.at(t);
		fx->playedInRandomList = false;
	}
}

FxItem *FxList::addFx(int fxtype, int option)
{
	FxItem *retfx = 0;

	switch (fxtype) {
	case FX_AUDIO:
		{
			FxAudioItem *fx = new FxAudioItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *fx = new FxSceneItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			if (option >= 0) {
				fx->createDefaultTubes(option);
			}
			retfx = fx;
		}
		break;
	case FX_AUDIO_PLAYLIST:
		{
			FxPlayListItem *fx = new FxPlayListItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
		break;
	case FX_SEQUENCE:
		{
			FxSeqItem *fx = new FxSeqItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
		break;
	case FX_SCRIPT:
		{
			FxScriptItem *fx = new FxScriptItem(this);
			fx->refCount.ref();
			fx->setRawScript("mode BreakOnCancel,DisableMultiStart\n");
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
		break;
	case Fx_CUE:
		{
			FxCueItem *fx = new FxCueItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
		break;
	case FX_TIMELINE:
		{
			FxTimeLineItem *fx = new FxTimeLineItem(this);
			fx->refCount.ref();
			// init FxTimeLine here
			m_fxList.append(fx);
			m_isModified = true;
			retfx = fx;
		}
	default:
		break;
	}

//	if (retfx)
//		emit fxListChanged();

	return retfx;
}

void FxList::addFx(FxItem *newfx)
{
	newfx->refCount.ref();
	m_fxList.append(newfx);
	m_isModified = true;
}

bool FxList::removeFx(FxItem *fx)
{
	if (!fx)
		return false;

	int i = m_fxList.indexOf(fx);
	if (i >= 0) {
		m_fxList.removeAt(i);
		if (!fx->refCount.deref()) {
			delete fx;
		} else {
			qWarning() << "Fx" << fx->name() << "still used when removed -> keep in memory";
		}
		return true;
	}

	return false;
}

/**
 * @brief Increment ref counter for all members in this list.
 *
 * This funktion is called after load a project.
 */
void FxList::refAllMembers()
{
	for (int t=0; t<m_fxList.size(); t++) {
		m_fxList.at(t)->refCount.ref();
		// check if FxItem has a child list
		if (m_fxList.at(t)->fxList())
			m_fxList.at(t)->fxList()->refAllMembers();
	}
}

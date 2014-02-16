#include "fxlist.h"
#include "fxaudioitem.h"
#include "fxsceneitem.h"
#include "fxplaylistitem.h"
#include "fxseqitem.h"
#include "log.h"
#include "fxitemtool.h"

#include <QMutableListIterator>
#include <QDateTime>


FxList::FxList(FxItem *parentFx) :
	QObject()
{
	m_fxLast = 0;
	m_fxNext = 0;
	m_fxCurrent = 0;
	m_fxParent = parentFx;
	myAutoProceedFlag = false;
	regid = 0;
	m_isModified = false;
	myRandomizedFlag = false;
	myLoopTimes = 1;
	m_isProtected = false;

	showColumnFadeinFlag = false;
	showColumnFadeoutFlag = false;
	showColumnIdFlag = false;
	showColumnHoldFlag = false;
	showColumnPredelayFlag = false;
	showColumnPostdelayFlag = false;
	showColumnKeyFlag = false;
}

FxList::~FxList()
{
	clear();
}

void FxList::clear()
{
	m_fxLast = 0;
	m_fxNext = 0;
	m_fxCurrent = 0;
	m_isModified = true;

	while (!m_fxList.isEmpty()) {
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = m_fxList.takeFirst();
		if (!fx->refCount.deref()) {
			delete fx;
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
 * @brief Find the follower of an arbitrary FX entry in the list
 * @param curfx Pointer to the FxItem the follower is search for or NULL
 * @return Pointer to FxItem or 0, if there is no follower cause list is at tail
 *
 * If curfx parameter is NULL the current nextFx() Pointer is taken as search begin
 */
FxItem *FxList::findSequenceFollower(FxItem *curfx)
{
	if (!curfx) curfx = m_fxNext;

	FxItem *followfx = 0;
	int idx_cur = m_fxList.indexOf(curfx);
	if (idx_cur >= 0) {
		if (idx_cur+1 < m_fxList.size()) {
			followfx = m_fxList.at(idx_cur+1);

		}
	}
	return followfx;
}

/**
 * @brief Find the Predecessor (Forerunner) of an arbitrary FX entry in the list
 * @param curfx Pointer to the FxItem the Predecessor is search for or NULL
 * @return Pointer to FxItem or 0, if there is no Predecessor cause list is at head
 *
 * If curfx parameter is NULL the current nextFx() Pointer is taken as search begin
 */
FxItem *FxList::findSequenceForerunner(FxItem *curfx)
{
	if (!curfx) curfx = m_fxNext;

	FxItem *prevfx = 0;
	int idx_cur = m_fxList.indexOf(curfx);
	if (idx_cur > 0) {
		prevfx = m_fxList.at(idx_cur-1);
	}
	return prevfx;
}

FxItem *FxList::findSequenceRandomFxItem()
{
	int cnt = m_fxList.size();
	FxItem *fx = 0;
	qsrand(QDateTime::currentDateTime().toTime_t());
	int tries = 0;
	while (!fx && tries++ < 1000) {
		int rnd = double(qrand()) * cnt / RAND_MAX;
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

bool FxList::addFxAudioSimple(const QString &path, int pos)
{
	FxAudioItem *fx = new FxAudioItem(path,this);
	fx->refCount.ref();
	if (pos < 0 || pos >= m_fxList.size()) {
		m_fxList.append(fx);
	} else {
		m_fxList.insert(pos,fx);
	}
	m_isModified = true;

	return true;
}

bool FxList::addFxScene(int tubes, FxItem **addedFxPointer)
{
	FxItem *fx = addFx(FX_SCENE,tubes);
	if (fx) {
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
		fx->setName("FX Sequence");
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
		new_scene->refCount.ref();
		m_fxList.append(new_scene);
		m_isModified = true;
		FxItemTool::setClonedFxName(scene,new_scene,this);
	}
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
	switch (fxtype) {
	case FX_AUDIO:
		{
			FxAudioItem *fx = new FxAudioItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			return fx;
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
			return fx;
		}
	case FX_AUDIO_PLAYLIST:
		{
			FxPlayListItem *fx = new FxPlayListItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			return fx;
		}
	case FX_SEQUENCE:
		{
			FxSeqItem *fx = new FxSeqItem(this);
			fx->refCount.ref();
			m_fxList.append(fx);
			m_isModified = true;
			return fx;
		}
	default:
		break;
	}

	return 0;
}

void FxList::addFx(FxItem *newfx)
{
	newfx->refCount.ref();
	m_fxList.append(newfx);
	m_isModified = true;
}

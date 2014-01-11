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
	fx_last = 0;
	fx_next = 0;
	fx_current = 0;
	fx_parent = parentFx;
	myAutoProceedFlag = false;
	myLoopFlag = false;
	auto_run_f = false;
	regid = 0;
	modified_f = false;
	myRandomizedFlag = false;
	myLoopTimes = 1;

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
	fx_last = 0;
	fx_next = 0;
	fx_current = 0;
	modified_f = true;

	while (!fx_list.isEmpty()) {
		// remove effect from list and delete it if there is no more reference
		FxItem *fx = fx_list.takeFirst();
		if (!fx->refCount.deref()) {
			delete fx;
		}
	}
}

void FxList::setNextFx(FxItem *nfx)
{
	if (nfx != fx_next) {
		fx_next = nfx;
		emit fxNextChanged(fx_next);
	}
}

void FxList::setCurrentFx(FxItem *curfx)
{
	if (!FxItem::exists(curfx)) curfx = 0;

	if (fx_current != curfx) {
		emit fxCurrentChanged(curfx, fx_current);
		fx_current = curfx;
	}
}

FxItem *FxList::stepToSequenceNext()
{
	if (!fx_next) return 0;
	fx_last = fx_current;
	if (fx_current != fx_next) {
		emit fxCurrentChanged(fx_next, fx_current);
		fx_current = fx_next;
	}

	// Now check if auto proceed is selected and find next entry in list
	if (myAutoProceedFlag) {
		setNextFx( findSequenceFollower(fx_current) );
	}
	return fx_current;
}

FxItem *FxList::getFxByKeyCode(int keycode) const
{
	QListIterator<FxItem*> it(fx_list.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		if (fx->keyCode() == keycode) return fx;
	}
	return 0;
}

FxItem *FxList::getFxByListIndex(int idx) const
{
	if (idx < fx_list.nativeList().size()) {
		return fx_list.nativeList().at(idx);
	}
	return 0;
}

int FxList::loopTimes() const
{
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
	QListIterator<FxItem*> it(fx_list.nativeList());
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
	for (int t=0; t<fx_list.size(); t++) {
		FxItem *fx = fx_list.at(t);
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
	if (!curfx) curfx = fx_next;

	FxItem *followfx = 0;
	int idx_cur = fx_list.indexOf(curfx);
	if (idx_cur >= 0) {
		if (idx_cur+1 < fx_list.size()) {
			followfx = fx_list.at(idx_cur+1);

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
	if (!curfx) curfx = fx_next;

	FxItem *prevfx = 0;
	int idx_cur = fx_list.indexOf(curfx);
	if (idx_cur > 0) {
		prevfx = fx_list.at(idx_cur-1);
	}
	return prevfx;
}

FxItem *FxList::findSequenceRandomFxItem()
{
	int cnt = fx_list.size();
	FxItem *fx = 0;
	qsrand(QDateTime::currentDateTime().toTime_t());
	int tries = 0;
	while (!fx && tries++ < 1000) {
		int rnd = double(qrand()) * cnt / RAND_MAX;
		if (!fx_list.at(rnd)->playedInRandomList) {
			fx = fx_list.at(rnd);
			fx->playedInRandomList = true;
		}
	}

	int i = 0;
	while (!fx && i<cnt) {
		if (!fx_list.at(i)->playedInRandomList) {
			fx = fx_list.at(i);
			fx->playedInRandomList = true;
		}
		i++;
	}
	return fx;
}

bool FxList::addFxAudioSimple(const QString &path, int pos)
{
	FxAudioItem *fx = new FxAudioItem(path,this);
	fx->refCount.ref();
	if (pos < 0 || pos >= fx_list.size()) {
		fx_list.append(fx);
	} else {
		fx_list.insert(pos,fx);
	}
	modified_f = true;

	return true;
}

bool FxList::addFxScene(int tubes)
{
	FxItem *fx = addFx(FX_SCENE,tubes);
	if (fx) {
		fx->setName("New Scene");
		return true;
	}

	return false;
}

bool FxList::addFxAudioPlayList()
{
	FxItem *fx = addFx(FX_AUDIO_PLAYLIST);
	if (fx) {
		fx->setName("Audio play list");
		return true;
	}
	return false;
}

bool FxList::addFxSequence()
{
	FxItem *fx = addFx(FX_SEQUENCE);
	if (fx) {
		fx->setName("FX Sequence");
		return true;
	}
	return false;
}

void FxList::moveFromTo(int srcidx, int destidx)
{
	FxItem *xitem = 0;
	if (srcidx >= 0 && srcidx < fx_list.size()) {
		xitem = fx_list.takeAt(srcidx);
	} else {
		return;
	}

	if (destidx < 0) {
		fx_list.append(xitem);
	}
	else if (srcidx < destidx) {
		fx_list.insert(destidx,xitem);
	}
	else if (destidx <= srcidx ){
		fx_list.insert(destidx,xitem);
	}
	modified_f = true;

	emit fxListChanged();
}

bool FxList::deleteFx(FxItem *fx)
{
	bool del = fx_list.removeOne(fx);
	if (del) {
		if (fx == fx_next) fx_next = 0;
		if (fx == fx_last) fx_last = 0;
		if (fx == fx_current) {
			emit fxCurrentChanged(0,fx_current);
			fx_current = 0;
		}
		delete fx;
		emit fxListChanged();
		modified_f = true;
	}
	return del;
}

bool FxList::contains(FxItem *fx)
{
	if (fx_list.indexOf(fx) < 0) {
		return false;
	} else {
		return true;
	}
}

bool FxList::isModified() const
{
	bool modified = modified_f;
	QListIterator<FxItem*> it(fx_list.nativeList());
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
	QMutableListIterator<FxItem*> it(fx_list.nativeList());
	while (it.hasNext()) {
		FxItem *fx = it.next();
		fx->setModified(state);
	}
	modified_f = state;
}

void FxList::cloneSelectedSceneItem()
{
	if (fx_next && fx_next->fxType() == FX_SCENE) {
		FxSceneItem *scene = reinterpret_cast<FxSceneItem*>(fx_next);
		FxSceneItem *new_scene = new FxSceneItem(*scene);
		new_scene->refCount.ref();
		fx_list.append(new_scene);
		modified_f = true;
		FxItemTool::setClonedFxName(scene,new_scene,this);
	}
}


/**
 * @brief Reset the members of the list for (sequence) start
 * @param skipFx If not NULL this FxItem will be skip in initializing (usefull for countinue)
 */
void FxList::resetFxItems(FxItem *skipFx)
{
	for (int t=0; t<fx_list.size(); t++) {
		if (fx_list.at(t) != skipFx) {
			fx_list.at(t)->resetFx();
		}
	}
}

void FxList::resetFxItemsForNewExecuter()
{
	for (int t=0; t<fx_list.size(); t++) {
		FxItem *fx = fx_list.at(t);
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
			fx_list.append(fx);
			modified_f = true;
			return fx;
		}
		break;
	case FX_SCENE:
		{
			FxSceneItem *fx = new FxSceneItem(this);
			fx->refCount.ref();
			fx_list.append(fx);
			modified_f = true;
			if (option >= 0) {
				fx->createDefaultTubes(option);
			}
			return fx;
		}
	case FX_AUDIO_PLAYLIST:
		{
			FxPlayListItem *fx = new FxPlayListItem(this);
			fx->refCount.ref();
			fx_list.append(fx);
			modified_f = true;
			return fx;
		}
	case FX_SEQUENCE:
		{
			FxSeqItem *fx = new FxSeqItem(this);
			fx->refCount.ref();
			fx_list.append(fx);
			modified_f = true;
			return fx;
		}
	default:
		break;
	}

	return 0;
}

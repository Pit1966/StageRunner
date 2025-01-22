#include "fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/exttimelineitem.h"
#include "fx/timelineexecuter.h"
#include "fx/execcenter.h"
#include "fx/fxtimelinetrack.h"
#include "mods/timeline/timelinewidget.h"
#include "mods/timeline/timelinebox.h"
#include "mods/timeline/timelineruler.h"
#include "mods/timeline/timelinecurve.h"
#include "system/fxcontrol.h"
#include "appcontrol/appcentral.h"
#include "gui/customwidget/extmimedata.h"
#include "gui/fxlistwidgetitem.h"
#include "gui/fxlistwidget.h"
#include "log.h"

#include <QMimeData>
#include <QDragEnterEvent>


FxTimeLineScene::FxTimeLineScene(TimeLineWidget *timeLineWidget)
	: TimeLineGfxScene(timeLineWidget)
{	
}

void FxTimeLineScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	// qDebug() << Q_FUNC_INFO;
	// qDebug() << "FxTimeLineScene drop:" << event->dropAction();
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);
	if (extmime) {
		QPointF dropPos = event->scenePos();
		int trackID = m_timeLine->yPosToTrackId(dropPos.y());
		QDrag *drag = extmime->dragObject;
		// qDebug() << "drag pixmap size" << drag->pixmap().size();
		// qDebug() << "drag hot spot" << drag->hotSpot();

		FxItem *fx = extmime->fxListWidgetItem->linkedFxItem;
		if (!FxItem::exists(fx))
			return;
		// qDebug() << "  dropped:" << fx->name() << "on track" << trackID;
		if (trackID < 1)
			return;

		// create a timeline object at drop position
		int xMs = m_timeLine->pixelToMs(dropPos.x() - drag->hotSpot().x());
		if (xMs < 0)
			xMs = 0;
		TimeLineBox *tli = m_timeLine->addTimeLineBox(xMs, 5000, "drop item", trackID);
		ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);
		if (!extTLI->linkToFxItem(fx)) {
			m_timeLine->removeTimeLineItem(tli, true);
		}
		event->ignore();
	} else {
		qDebug() << Q_FUNC_INFO << "something dropped";
		event->accept();
	}
}

void FxTimeLineScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	// qDebug() << Q_FUNC_INFO;
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	if (extmime) {
		event->accept();
	} else {
		event->ignore();
	}
}

void FxTimeLineScene::dragLeaveEvent(QGraphicsSceneDragDropEvent */*event*/)
{
	// qDebug() << Q_FUNC_INFO;
}

void FxTimeLineScene::dragMoveEvent(QGraphicsSceneDragDropEvent */*event*/)
{

}


// ------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------

bool ExtTimeLineWidget::setFxTimeLineItem(FxTimeLineItem *fxt)
{
	// clear all tracks (and items) in the widget
	clear();

	if (fxt->m_timeLineDurationMs > 1000) {
		qDebug() << "setInitialTimeLineDuration" << fxt->m_timeLineDurationMs;
		setTimeLineDuration(fxt->m_timeLineDurationMs);
	}

	bool hasItems = false;

	// Each track has its own list with item/objs
	// t is also the trackID !!
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		int trackID = t;
		FxTimeLineTrack *fxtrack = nullptr;
		TimeLineTrack *track = nullptr;
		// add timeline track
		if (t < fxt->m_tracks.size()) {
			// copy track data to TimeLineTrack
			fxtrack = fxt->m_tracks.at(t);
			track = new TimeLineTrack(this, fxtrack->trackType(), fxtrack->trackId());
			fxtrack->copyDataTo(track);

			if (fxtrack->trackType() == TRACK_RULER && m_tracks.size() == 1 && m_tracks.at(0)->trackType() == TRACK_RULER)
				continue;

			if (!addTimeLineTrack(track))
				LOGERROR(tr("Could not add timeline track with ID #%1 from FX in timeline editor")
						 .arg(fxtrack->trackId()));

		}

		// get references to the source list in the FxItem for this track
		const VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];

		if (fxtrack && fxtrack->trackType() == TRACK_AUDIO_VOL) {
			if (!track) {
				qWarning() << Q_FUNC_INFO << "missing track_audio_env";
				continue;
			}

			// an audio envelope (TinmeLimeCurve) Track should have only one item, but who knows ....
			for (int i=0; i<varset.size(); i++) {
				FxTimeLineObj *obj = varset.at(i);

				// Now create an item and add it to the track
				TimeLineCurve *item = new TimeLineCurve(this, track->trackId());
				item->setLabel(obj->label);
				item->setTimeLineDuration(m_timeLineLenMs);
				// item->setTrackDuration(m_timeLineLenMs);
				item->setPosition(obj->posMs);
				item->setYPos(track->yPos());

				// copy config data (nodes, ...) to the item
				item->setConfigDat(obj->configDat);

				track->appendTimeLineItem(item);

				// finaly add the item to the scene
				m_scene->addItem(item);
			}
		}
		else {
			// This should be a TimeLine Track with editable items of type TRACK_ITEMS
			// now copy the elements of the track
			// we have to convert a FxTimeLineObj which is used by the fx control unit to an TimeLineItem, which
			// is used in the TimeLineWidget

			for (int i=0; i<varset.size(); i++) {
				FxTimeLineObj *obj = varset.at(i);
				TimeLineBox *tli = addTimeLineBox(obj->posMs, obj->lenMs, obj->label, trackID);
				ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);
				extTLI->cloneItemDataFrom(obj);
				// auto correct color setting
				if (extTLI->linkedObjType() == LINKED_FX_SCENE) {
					extTLI->setBackgroundColor(QColor(0x923d0c));
					extTLI->setBorderColor(QColor(0x923d0c));
				}
				else if (extTLI->linkedObjType() == LINKED_FX_SCRIPT) {
					extTLI->setBackgroundColor(QColor(0x413f32));
					extTLI->setBorderColor(QColor(0x413f32));
				}
				else if (extTLI->linkedObjType() == CMD_PAUSE) {
					extTLI->setBackgroundColor(QColor(0xb02127));
					extTLI->setBorderColor(QColor(0xb02127));
				}

				hasItems = true;
			}
		}
	}

	// Add a track, if there are no items and therefor no tracks so far
	if (!hasItems)
		addTimeLineTrack();

	return true;
}

bool ExtTimeLineWidget::copyToFxTimeLineItem(FxTimeLineItem *fxt)
{
	if (!fxt)
		return false;

	// clear all tracks (and FxTimeLineObjs) in the FxTimeLineItem
	// fxt->clear();

	// Each track has a its own list with item/objs
	// t is also the trackID !!
	for (int t=0; t<m_tracks.size(); t++) {
		int trackID = t;
		TimeLineTrack *track = m_tracks.at(trackID);
		// copy track definition data first
		if (fxt->m_tracks.size() <= t) {	// track does not exist in FxTimeLineItem
			FxTimeLineTrack *fxtrack = new FxTimeLineTrack(this);
			fxtrack->copyDataFrom(track);
			fxt->m_tracks.append(fxtrack);
			fxt->setModified(true);
		}
		else if (fxt->m_tracks.at(t) == nullptr) {	// empty track in list
			FxTimeLineTrack *fxtrack = new FxTimeLineTrack(this);
			fxtrack->copyDataFrom(track);
			fxt->m_tracks[t] = fxtrack;
			fxt->setModified(true);
		}
		else {
			// copy data in existing track, but only if not modified
			if (!fxt->m_tracks.at(t)->isEqual(track)) {
				fxt->m_tracks.at(t)->copyDataFrom(track);
				fxt->setModified(true);
			}
		}

		// Check kind of track first
		if (track->trackType() == TRACK_AUDIO_VOL) {

		}

		// get references to the source and destination list for this track
		// const QList<TimeLineItem*> &timelinelist = m_itemLists[t];
		VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];

		int i = -1;
		// now copy the elements of the track
		// we have to convert the TimeLineItem back to a FxTimeLineObj
		while (++i < track->itemCount()) {
			TimeLineItem *tli = track->itemAt(i);
			ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);

			FxTimeLineObj *obj = new FxTimeLineObj(tli->position(), tli->duration(), tli->label(), tli->trackID());
			obj->configDat = tli->getConfigDat();
			if (extTLI) {
				obj->cloneItemDataFrom(extTLI);
				// obj->m_fxID = extTLI->m_fxID;
				// obj->m_linkedObjType = extTLI->m_linkedObjType;
			}

			if (i < varset.size()) {
				if (varset.at(i)->isEqual(obj)) {
					// item not modified
					delete obj;
				} else {
					if (varset.at(i)->isEqual(obj)) {
						qDebug() << "varset modified" << varset.at(i)->className();
					}
					// delete old item in timeline
					delete varset.at(i);
					// and set new one at the same position
					varset[i] = obj;
					fxt->setModified(true);
				}
			}
			else {
				varset.append(obj);
				fxt->setModified(true);
			}
		}
		// delete superflues elements in fxitem timeline data
		while (varset.size() > track->itemCount()) {
			delete varset.takeLast();
			fxt->setModified(true);
		}
	}

	// if tracks were deleted in the time line, the track count in the FxItem is higher than the count in the editor
	// so we have to delete the superflues
	int trackCnt = m_tracks.size();
	if (fxt->m_tracks.size() != trackCnt) {
		fxt->setModified(true);
		while (fxt->m_tracks.size() > trackCnt) {
			delete fxt->m_tracks.takeLast();
		}

		for (int i=trackCnt; i<TIMELINE_MAX_TRACKS; i++) {
			fxt->m_timelines[i].clear();
		}
	}

	fxt->m_timeLineDurationMs = timeLineDuration();

	return true;
}

TimeLineBox *ExtTimeLineWidget::createNewTimeLineBox(TimeLineWidget *timeline, int trackId)
{
	ExtTimeLineItem *tlItem = new ExtTimeLineItem(timeline, trackId);
	return tlItem;
}

TimeLineGfxScene *ExtTimeLineWidget::createTimeLineScene(TimeLineWidget *timeline)
{
	qDebug() << "create new timeline scene";

	if (m_fxTimeLineScene == nullptr) {
		m_fxTimeLineScene = new FxTimeLineScene(timeline);
	} else {
		qWarning() << Q_FUNC_INFO << "Timeline scene already created!";
	}

	return m_fxTimeLineScene;
}

// ------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------

QList<FxTimeLineEditWidget*>FxTimeLineEditWidget::m_timelineEditWidgetList;


FxTimeLineEditWidget::FxTimeLineEditWidget(AppCentral *app_central, QWidget *parent)
	: QWidget(parent)
	, m_aC(app_central)
	, m_fxCtrl(app_central->unitFx)
	, m_timeline(new ExtTimeLineWidget())
{
	if (!m_timeline->isInitialized())
		m_timeline->init();
	setupUi(this);
	//	QFont font("DejaVu Sans Mono", 13);
	//	timeCodeCursorLabel->setFont(font);
	//	timeCodeMouseLabel->setFont(font);

	m_timeline->setAcceptDrops(true);

	mainLayout->addWidget(m_timeline);
	m_timeline->setCursorPos(0);
	onMousePositionChanged(0);
	onCursorPositionChanged(0);

	connect(m_timeline, SIGNAL(cursorPosChanged(int)), this, SLOT(onCursorPositionChanged(int)));
	connect(m_timeline, SIGNAL(mousePosMsChanged(int)), this, SLOT(onMousePositionChanged(int)));

	setWindowFlag(Qt::WindowStaysOnTopHint);
}

FxTimeLineEditWidget::~FxTimeLineEditWidget()
{
	delete m_timeline;
}

FxTimeLineEditWidget *FxTimeLineEditWidget::openTimeLinePanel(FxTimeLineItem *fx, QWidget *parent)
{
	for (FxTimeLineEditWidget *tlwid : std::as_const(m_timelineEditWidgetList)) {
		if (tlwid->currentFxItem() == fx) {
			tlwid->setWindowState(tlwid->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
			tlwid->raise();
			tlwid->show();
			return nullptr;
		}
	}

	FxTimeLineEditWidget *editwid = new FxTimeLineEditWidget(AppCentral::instance(), parent);
	editwid->setFxTimeLineItem(fx);
	m_timelineEditWidgetList.append(editwid);

	// is there already an executor running for this timeline?
	TimeLineExecuter *exec = AppCentral::ref().execCenter->findTimeLineExecuter(fx);
	if (exec) {
		connect(exec, SIGNAL(timeLineStatusChanged(int)), editwid, SLOT(onChildRunStatusChanged(int)));
		connect (exec, SIGNAL(playPosChanged(int)), editwid->timeLineWidget(), SLOT(setCursorPos(int)));

		// // Determine what the FxListWidget Window is where the FxPlayListItem lives in
		// FxList *fxlist = fx->parentFxList();
		// FxListWidget *wid = FxListWidget::findFxListWidget(fxlist);
		// if (wid) {
		// 	// FxListWidgetItem *listitem = wid->getFxListWidgetItemFor(fx);
		// 	connect(exec, SIGNAL(timeLineStatusMsgChanged(FxItem*,QString)), wid, SLOT(propagateStatusMsg(FxItem*,QString)));
		// 	// connect(exe,SIGNAL(listProgressStepChanged(int,int)),listitem,SLOT(setActivationProgress(int,int)),Qt::UniqueConnection);
		// }

		if (exec->isRunning())
			editwid->onChildRunStatusChanged(Executer::EXEC_RUNNING);
	}


	return editwid;
}

void FxTimeLineEditWidget::destroyAllTimelinePanels()
{
	while (!m_timelineEditWidgetList.isEmpty())
		delete m_timelineEditWidgetList.takeFirst();
}

FxTimeLineEditWidget *FxTimeLineEditWidget::findParentFxTimeLinePanel(FxItem *fx)
{
	for (FxTimeLineEditWidget *tlwid : std::as_const(m_timelineEditWidgetList)) {
		if (tlwid->currentFxItem() == fx)
			return tlwid;
	}
	return nullptr;
}


FxTimeLineItem *FxTimeLineEditWidget::currentFxItem() const
{
	return m_curFxItem.data();
}

bool FxTimeLineEditWidget::setFxTimeLineItem(FxTimeLineItem *fxt)
{
	m_curFxItem = fxt;

	return m_timeline->setFxTimeLineItem(fxt);
}

bool FxTimeLineEditWidget::copyToFxTimeLineItem(FxTimeLineItem *fxt)
{

	return m_timeline->copyToFxTimeLineItem(fxt);
}

bool FxTimeLineEditWidget::stopRunningTimeLine()
{
	if (!m_timelineExecuter) {
		qDebug() << "timeline executer not known";
		if (m_curFxItem == nullptr) {
			qWarning() << "FxTimeLineItem not available!";
			return false;
		}
		m_timelineExecuter = m_aC->execCenter->findTimeLineExecuter(m_curFxItem);

		if (m_timelineExecuter == nullptr) {
			qWarning() << "TimeLineExecutor for TimeLineItem not found";
			m_timelineIsRunning = false;
			return false;
		}
	}

	m_timelineExecuter->setStopAllAtFinishFlag();
	m_timelineExecuter->setFinish();

	return false;
}

void FxTimeLineEditWidget::onChildRunStatusChanged(int runStatus)
{
	if (runStatus == Executer::EXEC_RUNNING) {
		runButton->setText("Stop");
		m_timelineIsRunning = true;
	}
	else {
		runButton->setText("Run");
		m_timelineIsRunning = false;
	}
}


void FxTimeLineEditWidget::closeEvent(QCloseEvent */*event*/)
{
	qDebug() << "close event" << m_curFxItem;
	if (m_curFxItem)
		copyToFxTimeLineItem(m_curFxItem);


	m_timelineEditWidgetList.removeOne(this);
	deleteLater();
}


void FxTimeLineEditWidget::on_runButton_clicked()
{
	if (!m_curFxItem)
		return;

	if (m_timelineIsRunning) {
		bool stopped = stopRunningTimeLine();
		return;
	}

	// we have to update the FxItem with timeline changes
	/// @todo modify flag would be nice here.
	copyToFxTimeLineItem(m_curFxItem);

	if (!m_timelineExecuter) {
		m_timelineExecuter = m_fxCtrl->startFxTimeLine(m_curFxItem, m_timeline->cursorPos());

		// if (m_timelineExecuter && m_timelineExecuter->isRunning()) {
		// 	runButton->setText("Stop");
		// }
	}
	else {
		m_timelineExecuter->setStopAllAtFinishFlag();
		m_timelineExecuter->setFinish();
		// runButton->setText("Run");
	}
}

void FxTimeLineEditWidget::onCursorPositionChanged(int ms)
{
	QString tcStr = TimeLineRuler::msToTimeLineString(ms, 3);
	while (tcStr.size() < 10)
		tcStr.prepend(' ');

	QString str = QString("Pos ") + tcStr;

	timeCodeCursorLabel->setText(str);
}

void FxTimeLineEditWidget::onMousePositionChanged(int ms)
{
	QString tcStr = TimeLineRuler::msToTimeLineString(ms, 3);
	while (tcStr.size() < 10)
		tcStr.prepend(' ');

	QString str = QString("    ") + tcStr;
	timeCodeMouseLabel->setText(str);
}



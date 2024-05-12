#include "fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/exttimelineitem.h"
#include "fx/timelineexecuter.h"
#include "fx/execcenter.h"
#include "mods/timeline/timelinewidget.h"
#include "mods/timeline/timelineitem.h"
#include "mods/timeline/timelineruler.h"
#include "system/fxcontrol.h"
#include "appcontrol/appcentral.h"
#include "gui/customwidget/extmimedata.h"
#include "gui/fxlistwidgetitem.h"

#include <QMimeData>
#include <QDragEnterEvent>


FxTimeLineScene::FxTimeLineScene(TimeLineWidget *timeLineWidget)
	: TimeLineGfxScene(timeLineWidget)
{	
}

void FxTimeLineScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	qDebug() << Q_FUNC_INFO;
	qDebug() << "FxTimeLineScene drop:" << event->dropAction();
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);
	if (extmime) {
		QPointF dropPos = event->scenePos();
		int trackID = m_timeLine->yPosToTrackId(dropPos.y());

		FxItem *fx = extmime->fxListWidgetItem->linkedFxItem;
		if (!FxItem::exists(fx))
			return;
		qDebug() << "  dropped:" << fx->name() << "on track" << trackID;
		if (trackID < 1)
			return;

		// create a timeline object at drop position
		int xMs = m_timeLine->pixelToMs(dropPos.x());
		TimeLineItem *tli = m_timeLine->addTimeLineItem(xMs, 5000, "drop item", trackID);
		ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);
		extTLI->linkToFxItem(fx);

		event->ignore();
	} else {
		qDebug() << Q_FUNC_INFO << "something dropped";
		event->accept();
	}
}

void FxTimeLineScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	qDebug() << Q_FUNC_INFO;
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	if (extmime) {
		event->accept();
	} else {
		event->ignore();
	}
}

void FxTimeLineScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	qDebug() << Q_FUNC_INFO;
}

void FxTimeLineScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
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

	// Each track has a its own list with item/objs
	// t is also the trackID !!
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		int trackID = t;
		// get references to the source and destination list for this track
		const VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];
		// QList<TimeLineItem*> &timelinelist = m_itemLists[t];

		// now copy the elements of the track
		// we have to convert a FxTimeLineObj which is used by the fx control unit to an TimeLineItem, which
		// is used in the TimeLineWidget

		for (int i=0; i<varset.size(); i++) {
			FxTimeLineObj *obj = varset.at(i);
			TimeLineItem *tli = addTimeLineItem(obj->posMs, obj->lenMs, obj->label, trackID);
			ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);
			extTLI->cloneItemDataFrom(obj);
			// extTLI->m_fxID = obj->m_fxID;
			// extTLI->m_linkedObjType = LINKED_OBJ_TYPE(obj->m_linkedObjType);
			hasItems = true;
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
	for (int t=0; t<TIMELINE_MAX_TRACKS; t++) {
		int trackID = t;
		// get references to the source and destination list for this track
		// const QList<TimeLineItem*> &timelinelist = m_itemLists[t];
		VarSetList<FxTimeLineObj*> &varset = fxt->m_timelines[t];

		int i = -1;
		// now copy the elements of the track
		// we have to convert a FxTimeLineObj which is used by the fx control unit to an TimeLineItem, which
		// is used in the TimeLineWidget
		while (++i < m_itemLists[trackID].size()) {
			TimeLineItem *tli = at(trackID, i);
			ExtTimeLineItem *extTLI = dynamic_cast<ExtTimeLineItem*>(tli);

			FxTimeLineObj *obj = new FxTimeLineObj(tli->position(), tli->duration(), tli->label(), tli->trackID());
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
	}

	fxt->m_timeLineDurationMs = timeLineDuration();

	return true;
}

TimeLineItem *ExtTimeLineWidget::createNewTimeLineItem(TimeLineWidget *timeline, int trackId)
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
}

FxTimeLineEditWidget::~FxTimeLineEditWidget()
{
	delete m_timeline;
}

FxTimeLineEditWidget *FxTimeLineEditWidget::openTimeLinePanel(FxTimeLineItem *fx, QWidget *parent)
{
	for (FxTimeLineEditWidget *tlwid : qAsConst(m_timelineEditWidgetList)) {
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
	for (FxTimeLineEditWidget *tlwid : qAsConst(m_timelineEditWidgetList)) {
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

void FxTimeLineEditWidget::onChildRunStatusChanged(int runStatus)
{
	if (runStatus == Executer::EXEC_RUNNING) {
		runButton->setText("Stop");
	}
	else {
		runButton->setText("Run");
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

	// we have to update the FxItem with timeline changes
	/// @todo modify flag would be nice here.
	copyToFxTimeLineItem(m_curFxItem);

	if (!m_timelineExecuter) {
		m_timelineExecuter = m_fxCtrl->startFxTimeLine(m_curFxItem, m_timeline->cursorPos());

		if (m_timelineExecuter->isRunning()) {
			runButton->setText("Stop");
		}
	}
	else {
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



#include "fxlistwidget.h"
#include "log.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "fxaudioitem.h"
#include "fxseqitem.h"
#include "fxitemobj.h"
#include "appcentral.h"
#include "usersettings.h"
#include "qtstatictools.h"
#include "customwidget/pslineedit.h"
#include "fxlistwidgetitem.h"
#include "customwidget/extmimedata.h"
#include "fxitempropertywidget.h"
#include "fxplaylistitem.h"
#include "fxitemtool.h"
#include "fxcontrol.h"
#include "execcenter.h"
#include "executer.h"
#include "qtstatictools.h"
#include "audiocontrol.h"
#include "messagedialog.h"
#include "lightcontrol.h"
#include <QDebug>
#include <QLineEdit>
#include <QDrag>
#include <QPainter>
#include <QKeyEvent>


MutexQList<FxListWidget*>FxListWidget::globalFxListWidgetList;

FxListWidget::FxListWidget(QWidget *parent) :
	QWidget(parent)
{
	init();

	setupUi(this);
	setStandAlone(false);

	fxTable->setDragEnabled(true);
	fxTable->setSelectionMode(QAbstractItemView::SingleSelection);
	fxTable->setDragDropMode(QAbstractItemView::DragDrop);
	// fxTable->setDropIndicatorShown(true);
	fxTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	fxTable->setSelectionMode(QAbstractItemView::NoSelection);


	// fxTable->setContextMenuPolicy(Qt::NoContextMenu);

	QPalette pal = fxTable->palette();
	pal.setBrush(QPalette::Highlight,Qt::darkGreen);
	fxTable->setPalette(pal);

	// Push a pointer to this widget to the global Pointer list to keep track of all FxListWidgets
	globalFxListWidgetList.lockAppend(this);

	// fxTable->viewport()->acceptDrops();
	connect(fxTable,SIGNAL(dropEventReceived(QString,int)),this,SIGNAL(dropEventReceived(QString,int)),Qt::QueuedConnection);
	connect(fxTable,SIGNAL(dropEventReceived(QString,int)),this,SLOT(drop_event_receiver(QString,int)),Qt::QueuedConnection);
	connect(fxTable,SIGNAL(rowMovedFromTo(int,int)),this,SLOT(moveRowFromTo(int,int)),Qt::QueuedConnection);
	connect(fxTable,SIGNAL(rowClonedFrom(PTableWidget*,int,int,bool)),this,SLOT(cloneRowFromPTable(PTableWidget*,int,int,bool)),Qt::QueuedConnection);

}

FxListWidget::~FxListWidget()
{
	// Remove tracking information in global FxListWidget list
	globalFxListWidgetList.removeOne(this);

	qDebug("FxListWidget destroyed");
}

void FxListWidget::setFxList(FxList *fxlist)
{
	cur_selected_item = 0;
	selected_rows.clear();
	fxTable->clear();

	is_modified_f = true;
	if (!fxlist) {
		myfxlist = 0;
		return;
	}
	if (myfxlist != fxlist) {
		myfxlist = fxlist;
		disconnect(this,SLOT(refreshList()));
		connect(fxlist,SIGNAL(fxListChanged()),this,SLOT(refreshList()));
	}

	int rows = fxlist->size();

	QStringList header;
	if (fxlist->showColumnKeyFlag) {
		header << tr("Key");
	}

	header << tr("Name") << tr("Function");

	if (fxlist->showColumnIdFlag) {
		header << tr("Id");
	}

	if (fxlist->showColumnPredelayFlag) {
		header << tr("PreDelay");
	}
	if (fxlist->showColumnFadeinFlag) {
		header << tr("FadeIN");
	}
	if (fxlist->showColumnHoldFlag) {
		header << tr("Hold");
	}
	if (fxlist->showColumnFadeoutFlag) {
		header << tr("FadeOUT");
	}
	if (fxlist->showColumnPostdelayFlag) {
		header << tr("PostDelay");
	}


	fxTable->setRowCount(rows);
	fxTable->setColumnCount(header.size());


	fxTable->setHorizontalHeaderLabels(header);

	QFont key_font;
	key_font.setPointSize(14);
	key_font.setBold(true);

	for (int t=0; t<rows; t++) {
		int col = 0;
		FxItem *fx = fxlist->at(t);
		// FxAudioItem *fxa = reinterpret_cast<FxAudioItem*>(fx);
		FxListWidgetItem *item;

		if (fxlist->showColumnKeyFlag) {
			QString key;
			if (fx->keyCode() != Qt::Key_Space) {
				key = QtStaticTools::keyToString(fx->keyCode());
			}

			item = new_fxlistwidgetitem(fx,key,FxListWidgetItem::CT_KEY);
			item->itemEdit->setMinimized(true);
			item->itemEdit->setSingleKeyEditEnabled(true);
			item->itemEdit->setFont(key_font);
			item->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
			item->itemLabel->hide();
			item->myRow = t;
			item->myColumn = col;
			fxTable->setCellWidget(t,col++,item);
		}

		item = new_fxlistwidgetitem(fx,fx->name(),FxListWidgetItem::CT_NAME);
		item->myRow = t;
		item->myColumn = col;
		item->itemLabel->hide();
		if (fx->loopValue() > 1) {
			item->itemExtra->setPixmap(QPixmap(":/gfx/icons/audio_repeat_22.png"));
			item->itemExtra->show();
		}
		else if (fx->preDelay() == -1) {
			item->itemExtra->setPixmap(QPixmap(":/gfx/icons/green_down_22.png"));
			item->itemExtra->show();
		}
		fxTable->setCellWidget(t,col++,item);
		switch (fx->fxType()) {
		case FX_AUDIO:
//			if (fxa->seekPosPerMille() > 0) {
//				item->setActivationProgressA(fxa->seekPosPerMille());
//				qDebug() << "seek pos" << fxa->seekPosPerMille();
//			}
			break;
		case FX_SCENE:
			break;
		case FX_AUDIO_PLAYLIST:
			break;
		case FX_SEQUENCE:
			break;
		default:
			break;
		}

		item = new_fxlistwidgetitem(fx,"",FxListWidgetItem::CT_FX_TYPE);
		item->setNeverEditable(true);
		item->itemLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
		switch (fx->fxType()) {
		case FX_AUDIO:
			item->itemLabel->setPixmap(QPixmap(":/gfx/icons/audio_speaker_grey.png"));
			break;
		case FX_SCENE:
			item->itemLabel->setPixmap(QPixmap(":/gfx/icons/scene_mixer.png"));
			break;
		case FX_AUDIO_PLAYLIST:
			item->itemLabel->setPixmap(QPixmap(":/gfx/icons/playlist.png"));
			break;
		case FX_SEQUENCE:
			item->itemLabel->setPixmap(QPixmap(":/gfx/icons/sequence.png"));
			break;
		default:
			break;
		}
		item->itemEdit->hide();
		item->myRow = t;
		item->myColumn = col;
		fxTable->setCellWidget(t,col++,item);

		if (fxlist->showColumnIdFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->id()),FxListWidgetItem::CT_ID);
			item->setNeverEditable(true);
			item->itemEdit->setMinimized(true);
			item->myRow = t;
			item->myColumn = col;
			fxTable->setCellWidget(t,col++,item);
		}

		if (fxlist->showColumnPredelayFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->preDelay()),FxListWidgetItem::CT_PRE_DELAY);
			item->myRow = t;
			item->myColumn = col;
			item->itemEdit->setMinimized(true);
			item->itemLabel->hide();
			fxTable->setCellWidget(t,col++,item);
		}

		if (fxlist->showColumnFadeinFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->fadeInTime()),FxListWidgetItem::CT_FADEIN_TIME);
			item->myRow = t;
			item->myColumn = col;
			item->itemEdit->setMinimized(true);
			item->itemLabel->hide();
			fxTable->setCellWidget(t,col++,item);
		}

		if (fxlist->showColumnHoldFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->holdTime()),FxListWidgetItem::CT_HOLD_TIME);
			item->myRow = t;
			item->myColumn = col;
			item->itemEdit->setMinimized(true);
			item->itemLabel->hide();
			fxTable->setCellWidget(t,col++,item);
		}

		if (fxlist->showColumnFadeoutFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->fadeOutTime()),FxListWidgetItem::CT_FADEOUT_TIME);
			item->myRow = t;
			item->myColumn = col;
			item->itemEdit->setMinimized(true);
			item->itemLabel->hide();
			fxTable->setCellWidget(t,col++,item);
		}

		if (fxlist->showColumnPostdelayFlag) {
			item = new_fxlistwidgetitem(fx,QString::number(fx->postDelay()),FxListWidgetItem::CT_POST_DELAY);
			item->myRow = t;
			item->myColumn = col;
			item->itemEdit->setMinimized(true);
			item->itemLabel->hide();
			fxTable->setCellWidget(t,col++,item);
		}

	}
	fxTable->resizeColumnsToContents();

	int resize_col = 0;
	if (fxlist->showColumnKeyFlag)
		resize_col = 1;

#ifdef IS_QT5
	fxTable->horizontalHeader()->setSectionResizeMode(resize_col,QHeaderView::Stretch);
#else
	fxTable->horizontalHeader()->setResizeMode(resize_col,QHeaderView::Stretch);
#endif
	autoProceedCheck->setChecked(fxlist->autoProceedSequence());
	loopCheck->setChecked(fxlist->isLooped());
	randomCheckBox->setChecked(fxlist->isRandomized());
}

void FxListWidget::setAutoProceedSequence(bool state)
{
	if (myfxlist) {
		autoProceedCheck->setChecked(state);
		myfxlist->setAutoProceedSequence(state);
	}
}

void FxListWidget::setLoop(int loops)
{
	if (myfxlist) {
		if (loops > 0) {
			loopCheck->setChecked(true);
		} else {
			loopCheck->setChecked(false);
		}
	}
	if (origin_fxitem) {
		origin_fxitem->setLoopValue(loops);
	}
}

QList<FxListWidgetItem *> FxListWidget::getItemListForRow(int row)
{
	WidItemList list;
	if (row >=0 && row < fxTable->rowCount()) {
		for (int col=0; col<fxTable->columnCount(); col++) {
			FxListWidgetItem *item = qobject_cast<FxListWidgetItem*>(fxTable->cellWidget(row,col));
			if (item) {
				list.append(item);
			}
		}
	}
	return list;
}

int FxListWidget::getRowThatContainsFxItem(FxItem *fx)
{
	for (int row=0; row < fxTable->rowCount(); row++) {
		FxListWidgetItem *item = qobject_cast<FxListWidgetItem*>(fxTable->cellWidget(row,0));
		if (item && item->linkedFxItem == fx) {
			return row;
		}
	}
	return -1;
}

FxItem *FxListWidget::getFxItemAtRow(int row) const
{
	if (row < 0 || row >= fxTable->rowCount()) return 0;
	FxListWidgetItem *item = qobject_cast<FxListWidgetItem*>(fxTable->cellWidget(row,0));

	return item->linkedFxItem;
}

void FxListWidget::setOriginFx(FxItem *fx)
{
	origin_fxitem = fx;

	QRect rect = QtStaticTools::stringToQRect(fx->widgetPosition());
	if (!rect.isNull()) {
		setGeometry(rect);
	}
}

FxListWidgetItem *FxListWidget::getFxListItemAtPos(QPoint pos)
{
	pos = fxTable->mapFromParent(pos);
	if (fxTable->horizontalHeader()) {
		pos.setY(pos.y()-fxTable->horizontalHeader()->height());
	}
	if (fxTable->verticalHeader()) {
		pos.setX(pos.x()-fxTable->verticalHeader()->width());
	}
	for (int col=0; col<fxTable->columnCount(); col++) {
		for (int row=0; row<fxTable->rowCount(); row++) {
			FxListWidgetItem *item = reinterpret_cast<FxListWidgetItem*>(fxTable->cellWidget(row,col));
			if (item->geometry().contains(pos)) return item;
		}
	}

	return 0;
}

FxListWidgetItem *FxListWidget::getFxListWidgetItemFor(FxItem *fx)
{
	for (int row=0; row < fxTable->rowCount(); row++) {
		FxListWidgetItem *item = qobject_cast<FxListWidgetItem*>(fxTable->cellWidget(row,1));
		if (item && item->linkedFxItem == fx) {
			return item;
		}
	}
	return 0;
}

void FxListWidget::setStandAlone(bool state)
{
	closeButton->setHidden(!state);
	titleLabel->setHidden(!state);
	loopCheck->setHidden(!state);
	autoProceedCheck->setHidden(state);
	randomCheckBox->setHidden(!state);
	is_standalone_f = state;
}

/**
 * @brief Check if FxItem can be inserted in this FxList instance
 * @param fx
 * @return true, if fx can fit into FxList parent Fx Type
 */
bool FxListWidget::isFxItemPossibleHere(FxItem *fx)
{
	if (!FxItem::exists(fx))
		return false;

	if (!origin_fxitem)
		return true;

	int fxtype = fx->fxType();

	switch (origin_fxitem->fxType()) {
	case FX_SEQUENCE:
		if (fxtype == FX_AUDIO || fxtype == FX_SCENE )
			return true;
		break;
	case FX_AUDIO_PLAYLIST:
		if (fxtype == FX_AUDIO)
			return true;
		break;
	default:
		return false;
	}

	return false;
}


void FxListWidget::generateDropAllowedFxTypeList(FxItem *fx)
{
	QList<int>list;

	if (FxItem::exists(fx)) {
		switch (fx->fxType()) {
		case FX_SEQUENCE:
			list.append(FX_SCENE);
		case FX_AUDIO_PLAYLIST:
			list.append(FX_AUDIO);
			break;
		default:
			break;
		}
	}

	fxTable->setDropAllowedIndices(list);
}



FxListWidget *FxListWidget::findFxListWidget(PTableWidget *tableWidget)
{
	FxListWidget *wid = 0;
	globalFxListWidgetList.lock();
	QListIterator<FxListWidget*>it(globalFxListWidgetList);
	while (it.hasNext() && !wid) {
		FxListWidget *cur = it.next();
		if (cur->fxTable == tableWidget)
			wid = cur;
	}

	globalFxListWidgetList.unlock();
	return wid;
}

FxListWidget *FxListWidget::findFxListWidget(FxList *fxList)
{
	FxListWidget *wid = 0;
	globalFxListWidgetList.lock();
	QListIterator<FxListWidget*>it(globalFxListWidgetList);
	while (it.hasNext() && !wid) {
		FxListWidget *cur = it.next();
		if (cur->myfxlist == fxList)
			wid = cur;
	}

	globalFxListWidgetList.unlock();
	return wid;

}

/**
 * @brief Find a FxList instance in all FxListWidgets or create a new on
 * @param fxList Pointer to searched FxList instance
 * @param fxItem Pointer to fxItem that contains the list
 * @param created Maybe a Pointer to bool that indicates if the widget was found or new created
 * @return Pointer to FxListWidget containing the fxList
 */
FxListWidget *FxListWidget::getCreateFxListWidget(FxList *fxList, FxItem *fxItem, bool *created)
{
	// First Search for FxListWidget for a widget containing fxList
	FxListWidget *wid = 0;
	globalFxListWidgetList.lock();
	QListIterator<FxListWidget*>it(globalFxListWidgetList);
	while (it.hasNext() && !wid) {
		FxListWidget *cur = it.next();
		if (cur->myfxlist == fxList)
			wid = cur;
	}
	globalFxListWidgetList.unlock();

	// 2nd if we did not found the fxList we create a a new FxListWidget and assign the list to it
	if (!wid) {
		wid = new FxListWidget();
		wid->setOriginFx(fxItem);
		wid->setFxList(fxList);
		wid->setStandAlone(true);
		wid->setWindowTitle(fxItem->name());
		if (fxItem->fxType() == FX_AUDIO_PLAYLIST) {
			wid->setWindowIcon(QPixmap(":/gfx/icons/playlist.png"));
		}
		else if (fxItem->fxType() == FX_SEQUENCE) {
			wid->setWindowIcon(QPixmap(":/gfx/icons/sequence.png"));
		}

		if (created)
			*created = true;
	} else {
		if (created)
			*created = false;
	}

	// Now return the widget pointer
	return wid;
}

FxListWidget *FxListWidget::findParentFxListWidget(FxItem *fx)
{
	FxListWidget *wid = 0;
	if (fx->parentFxList()) {
		wid = findFxListWidget(fx->parentFxList());
	}
	return wid;
}

bool FxListWidget::destroyFxListWidget(FxListWidget *wid)
{
	for (int t=globalFxListWidgetList.size()-1; t>=0; t--) {
		if (wid == globalFxListWidgetList.at(t)) {
			if (wid->isStandAlone()) {
				wid->setAttribute(Qt::WA_DeleteOnClose);
				wid->close();
				globalFxListWidgetList.removeAt(t);
				return true;
			}
		}
	}
	return false;
}

void FxListWidget::destroyAllFxListWidgets()
{
	for (int t=globalFxListWidgetList.size()-1; t>=0; t--) {
		FxListWidget *wid = globalFxListWidgetList.at(t);
		if (wid->isStandAlone()) {
			wid->close();
			delete globalFxListWidgetList.takeAt(t);
		}
	}
}

void FxListWidget::selectFx(FxItem *fx)
{
	if (!fx) {
		unselectRows();
	}

	bool found = false;
	int row=0;
	while (fx && row < fxTable->rowCount() && !found) {
		FxListWidgetItem * item = (FxListWidgetItem*)fxTable->cellWidget(row,0);
		if (item->linkedFxItem == fx) {
			found = true;
			if (cur_selected_item != fx) {
				cur_selected_item = fx;
				setSingleRowSelected(item->myRow);
				emit fxItemSelected(fx);
			}
		}
		row++;
	}
	if (!found) {
		emit fxItemSelected(0);
	}
}

void FxListWidget::markFx(FxItem *fx)
{
	// qDebug("mark fx %s",fx?fx->name().toLocal8Bit().data():"empty");
	int row=0;
	while (row < fxTable->rowCount()) {
		FxListWidgetItem * item = (FxListWidgetItem*)fxTable->cellWidget(row,0);
		WidItemList items = getItemListForRow(row);
		bool mark = false;
		if (item->linkedFxItem == fx) {
			mark = true;
		}

		for (int i=0; i<items.size(); i++) {
			items.at(i)->setMarked(mark);
		}

		row++;
	}
}

void FxListWidget::setCurrentFx(FxItem *newfx, FxItem *oldfx)
{
	Q_UNUSED(oldfx);
	markFx(newfx);
}

void FxListWidget::initRowDrag(FxListWidgetItem *item)
{
	QDrag *drag = new QDrag(this);
	// Now we create a spechial MimeData Object with Information for the origin of the fxListWidgetItem
	ExtMimeData *mdata = new ExtMimeData();
	mdata->fxListWidgetItem = item;
	mdata->originFxListWidget = this;
	mdata->originFxList = fxList();
	mdata->originPTableWidget = fxTable;
	mdata->setText(item->linkedFxItem->name());
	mdata->tableRow = item->myRow;
	mdata->tableCol = item->myColumn;
	mdata->dragObject = drag;
	if (fxList()->isProtected())
		mdata->moveDisabled = true;

	QPixmap pixmap(size().width(), item->height());

	QList<FxListWidgetItem*>items = getItemListForRow(item->myRow);
	int w = 0;
	int hotspot_w = 0;
	if (items.size() < 3) {
		pixmap.fill(Qt::white);
		item->render(&pixmap);
	} else {
		pixmap.fill(QColor(0,0,0,0));
		for (int i=0; i<3; i++) {
			items.at(i)->render(&pixmap,QPoint(w,0));
			if (item == items.at(i)) {
				hotspot_w = w;
			}
			w += items.at(i)->width();
		}
	}

	QPainter p;
	p.begin(&pixmap);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.fillRect(pixmap.rect(), QColor(0, 0, 0, 140));
	p.end();

	drag->setMimeData(mdata);
	drag->setPixmap(pixmap);
	drag->setHotSpot(item->dragBeginPos() + QPoint(hotspot_w,0));

	fxTable->saveScrollPos();
	fxTable->removeRow(item->myRow);
	fxTable->setOldScrollPos();

	Qt::DropAction dropaction = drag->exec();
	qDebug() << "Dropaction:" << dropaction;
	if (dropaction != Qt::MoveAction) {
		refreshList();
	}
}

void FxListWidget::init()
{
	is_modified_f = false;
	is_editable_f = false;
	cur_selected_item = 0;
	cur_clicked_item = 0;
	origin_fxitem = 0;
	upper_context_menu_split = 60;
}

void FxListWidget::closeEvent(QCloseEvent *)
{
	if (origin_fxitem) {
		origin_fxitem->setWidgetPosition(QtStaticTools::qRectToString(geometry()));
	}
}


FxListWidgetItem *FxListWidget::new_fxlistwidgetitem(FxItem *fx, const QString &text, int coltype)
{
	FxListWidgetItem * item = new FxListWidgetItem(fx,text,static_cast<FxListWidgetItem::ColumnType>(coltype));
	connect(item,SIGNAL(itemClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_clicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemDoubleClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_doubleclicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemTextEdited(FxListWidgetItem*,QString)),this,SLOT(if_fxitemwidget_edited(FxListWidgetItem*,QString)));
	connect(this,SIGNAL(editableChanged(bool)),item,SLOT(setEditable(bool)));
	connect(item,SIGNAL(draged(FxListWidgetItem*)),this,SLOT(initRowDrag(FxListWidgetItem*)),Qt::QueuedConnection);
	connect(item,SIGNAL(seekToPerMille(FxListWidgetItem*,int)),this,SLOT(if_fxitemwidget_seeked(FxListWidgetItem*,int)),Qt::QueuedConnection);
	connect(item,SIGNAL(tabPressedInEdit(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_tab_pressed(FxListWidgetItem*)));
	connect(item,SIGNAL(enterPressedInEdit(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_enter_pressed(FxListWidgetItem*)));

	item->setEditable(is_editable_f);
	return item;
}

void FxListWidget::refreshList()
{
	// qDebug("FxListWidget refresh");
	if (myfxlist) {
		setFxList(myfxlist);
	}
}

void FxListWidget::setEditable(bool state)
{
	if (state != is_editable_f) {
		unselectRows();
		is_editable_f = state;
		emit editableChanged(state);
	}
	if (state != editButton->isChecked()) {
		editButton->setChecked(state);
	}
}

/**
 * @brief Set selection status of specified row
 * @param row The row number
 * @param state true: select, false: unselect
 */
void FxListWidget::setRowSelected(int row, bool state)
{
	if (state) {
		if (!selected_rows.contains(row)) {
			selected_rows.append(row);
			fxTable->setRangeSelected(QTableWidgetSelectionRange(row,0,row,1),true);
		}
	} else {
		fxTable->setRangeSelected(QTableWidgetSelectionRange(row,0,row,1),false);
		selected_rows.removeAll(row);
	}

	foreach(FxListWidgetItem* item, getItemListForRow(row)) {
		item->setSelected(state);			///ooo
	}
}

void FxListWidget::setSingleRowSelected(int row)
{
	bool not_selected_yet = true;

	foreach(int selrow, selected_rows) {
		if (selrow != row) {
			setRowSelected(selrow,false);
		} else {
			not_selected_yet = false;
		}
	}

	if (not_selected_yet) setRowSelected(row,true);
}

/**
 * @brief Unselect all rows
 */
void FxListWidget::unselectRows()
{
	while (selected_rows.size()) {
		setRowSelected(selected_rows.takeFirst(),false);
	}
	cur_selected_item = 0;
}

void FxListWidget::propagateSceneStatus(FxSceneItem *scene)
{
	Q_UNUSED(scene);
//	int row = getRowThatContainsFxItem(scene);
//	WidItemList widlist = getItemListForRow(row);
//	if (widlist.size()) {
//		FxListWidgetItem *item = widlist.at(1);
//		if (scene->isActive()) {
//			item->setActivationProgress(1,1);
//		}
//		else if (scene->isVisible()) {
//			if (scene->isOnStageIntern())
//				item->setActivationProgressA(1000);
//			if (scene->isOnStageExtern())
//				item->setActivationProgressB(1000);
//		}
//		else {
//			item->setActivationProgress(0,0);
//		}
//	}

}

void FxListWidget::propagateSceneFadeProgress(FxSceneItem *scene, int perMilleA, int perMilleB)
{
	int row = getRowThatContainsFxItem(scene);
	WidItemList widlist = getItemListForRow(row);
	if (widlist.size()) {
		int column = 0;
		if (fxList()->showColumnKeyFlag)
			column = 1;
		FxListWidgetItem *item = widlist.at(column);
		if (scene->isActive()) {
			// qDebug("propagateSceneFadeProgress is active");
			item->setActivationProgress(perMilleA,perMilleB);
		}
		else if (scene->isVisible()) {
			// qDebug("propagateSceneFadeProgress is visible");
			item->setActivationProgress(perMilleA,perMilleB);
			if (scene->isOnStageIntern())
				item->setActivationProgressA(1000);
		}
		else {
			item->setActivationProgress(0,0);
		}
	}
}

void FxListWidget::propagateAudioStatus(AudioCtrlMsg msg)
{
	if (msg.ctrlCmd == CMD_STATUS_REPORT || msg.ctrlCmd == CMD_AUDIO_STATUS_CHANGED) {
		int row = getRowThatContainsFxItem(msg.fxAudio);
		WidItemList widlist = getItemListForRow(row);
		if (widlist.size()) {
			int column = 0;
			if (fxList()->showColumnKeyFlag)
				column = 1;
			FxListWidgetItem *item = widlist.at(column);

			switch (msg.currentAudioStatus) {
			case AUDIO_IDLE:
			case AUDIO_ERROR:
				item->setActivationProgressA(0);
				break;
			default:
				if (msg.progress >= 0) {
					item->setActivationProgressA(msg.progress);
				}
				break;
			}
		}
	}
}

void FxListWidget::cloneRowFromPTable(PTableWidget *srcPtable, int srcRow, int destRow, bool removeSrc)
{
	if (debug > 1) qDebug("%s clone row: %d to row: %d remove:%d",Q_FUNC_INFO,srcRow, destRow, removeSrc);

	FxListWidget *srcwidget = FxListWidget::findFxListWidget(srcPtable);
	if (srcwidget) {
		srcwidget->refreshList();

		FxItem *srcFx = srcwidget->fxList()->getFxByListIndex(srcRow);
		if (!FxItem::exists(srcFx)) return;

		if (!isFxItemPossibleHere(srcFx)) {
			MessageDialog *dialog = new MessageDialog(this);
			dialog->showInformation(tr("Destination not suitable for this FX")
								,tr("Copying or moving this FX is not allowed"));
			return;
		}

		FxItem *destFx;
		if (removeSrc) {
			destFx = srcFx;
		} else {
			destFx = FxItemTool::cloneFxItem(srcFx);
		}

		if (destRow >= 0 && destRow < fxList()->size()) {
			fxList()->insert(destRow,destFx);
		} else {
			fxList()->append(destFx);
		}
		refreshList();

		if (removeSrc) {
			srcwidget->fxList()->removeOne(srcFx);
			srcwidget->refreshList();
		}
	}

}



void FxListWidget::onFxItemSelectedInChildWidget(FxItem *fx)
{
	if (debug) qDebug("Selected FxItem in Child: %s",fx?fx->name().toLocal8Bit().data():"NULL");
	emit fxItemSelectedInChildFxListWidget(fx);
}

void FxListWidget::on_fxTable_itemClicked(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	if_fxitemwidget_clicked(myitem);
}

void FxListWidget::on_fxTable_itemDoubleClicked(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	if_fxitemwidget_doubleclicked(myitem);
}

void FxListWidget::moveRowFromTo(int srcrow, int destrow)
{
	if (debug > 1) qDebug("FxListWidget::moveRowFromTo: %d to %d",srcrow,destrow);
	if (myfxlist) {
		FxItem *cur = 0;
		if (srcrow >= 0 && srcrow < myfxlist->size()) cur = myfxlist->at(srcrow);
		myfxlist->moveFromTo(srcrow,destrow);
		// setFxList(myfxlist);
		if (cur) {
			qDebug() << "select current:" << cur->name();
			cur_selected_item = cur;
			selectFx(cur);
		}
		emit listModified();
	}
}


void FxListWidget::on_autoProceedCheck_clicked(bool checked)
{
	if (myfxlist) {
		myfxlist->setAutoProceedSequence(checked);
	}
}

void FxListWidget::on_loopCheck_clicked(bool checked)
{
	if (myfxlist) {
		FxItem *parentfx = myfxlist->parentFx();
		myfxlist->setLoopList(checked);
		if (checked) {
			if (parentfx) {
				parentfx->setLoopValue(10000);
			} else {
				myfxlist->setLoopTimes(100000);
			}
			myfxlist->setLoopList(true);
		} else {
			if (parentfx) {
				parentfx->setLoopValue(1);
			} else {
				myfxlist->setLoopTimes(1);
			}
			myfxlist->setLoopList(false);
		}
	}
}

void FxListWidget::on_randomCheckBox_clicked(bool checked)
{
	if (myfxlist) {
		myfxlist->setRandomized(checked);
	}
}


void FxListWidget::on_fxTable_itemChanged(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	FxItem *fx = myitem->linkedFxItem;
	switch (myitem->columnType) {
	case FxListWidgetItem::CT_NAME:
		fx->setName(myitem->itemText);
		emit listModified();
		break;
	case FxListWidgetItem::CT_KEY:
		{
			int old_code = fx->keyCode();
			int code = 0;
			QString text = myitem->text();
			if (text.size()) {
				code = QtStaticTools::stringToKey(text);
			}
			if (old_code != code) {
				fx->setKeyCode(code);
				emit listModified();
			}
			myitem->itemEdit->setText(QtStaticTools::keyToString(code));
		}
	default:
		break;
	}
}

void FxListWidget::if_fxitemwidget_clicked(FxListWidgetItem *listitem)
{
	if (!listitem) return;
	// qDebug("FxListWidget -> clicked -> coltype %d -> %s",listitem->columnType,listitem->text().toLocal8Bit().data());
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	switch (listitem->columnType) {
	case FxListWidgetItem::CT_KEY:
	case FxListWidgetItem::CT_NAME:
		myfxlist->setNextFx(fx);
		if (cur_selected_item != fx) {
			cur_selected_item = fx;
			emit fxItemSelected(fx);
		}
		unselectRows();
		setRowSelected(listitem->myRow,true);
		cur_selected_item = listitem->linkedFxItem;
		break;
	default:
		qDebug("Click on this column not implemented yet");

	}
	cur_clicked_item = listitem;
}

void FxListWidget::column_name_double_clicked(FxItem *fx)
{
	switch (fx->fxType()) {
	case FX_SCENE:
		selectFx(fx);
		emit fxCmdActivated(fx, CMD_FX_START,0);
		break;
	case FX_AUDIO:
		selectFx(fx);
		emit fxCmdActivated(fx,CMD_FX_START,0);
		break;
	case FX_AUDIO_PLAYLIST:
		selectFx(fx);
		emit fxCmdActivated(fx, CMD_FX_START,0);
		break;
	case FX_SEQUENCE:
		selectFx(fx);
		emit fxCmdActivated(fx,CMD_FX_START,0);
		break;
	}
}

void FxListWidget::column_type_double_clicked(FxItem *fx)
{
	emit fxTypeColumnDoubleClicked(fx);
}

void FxListWidget::contextMenuEvent(QContextMenuEvent *event)
{
	FxListWidgetItem *item = getFxListItemAtPos(event->pos());
	// int x = event->x();
	int y = event->y();

	if (!item) {
		FxList *fxl = fxList();
		if (!fxl) return;

		QMenu menu(this);
		QAction *act;

		if (y > upper_context_menu_split) {
			act = menu.addAction(tr("Add Audio Fx"));
			act->setObjectName("20");

			act = menu.addAction(tr("New Audio Playlist"));
			act->setObjectName("21");

			act = menu.addAction(tr("New Scene"));
			act->setObjectName("22");

			act = menu.addAction(tr("New Sequence"));
			act->setObjectName("23");


			act = menu.addAction("--------------------");
			if (isEditable()) {
				act = menu.addAction(tr("Deactivate Edit Mode"));
			} else {
				act = menu.addAction(tr("Activate Edit Mode"));
			}
			act->setObjectName("2");
		}

		if (y < upper_context_menu_split) {
			if (fxList()->showColumnIdFlag) {
				act = menu.addAction(tr("Hide ID Column"));
			} else {
				act = menu.addAction(tr("Show ID Column"));
			}
			act->setObjectName("3");
			if (fxList()->showColumnPredelayFlag) {
				act = menu.addAction(tr("Hide PreDelay Column"));
			} else {
				act = menu.addAction(tr("Show PreDelay Column"));
			}
			act->setObjectName("4");
			if (fxList()->showColumnFadeinFlag) {
				act = menu.addAction(tr("Hide FadeIN time column"));
			} else {
				act = menu.addAction(tr("Show FadeIN time column"));
			}
			act->setObjectName("5");
			if (fxList()->showColumnHoldFlag) {
				act = menu.addAction(tr("Hide Hold time Column"));
			} else {
				act = menu.addAction(tr("Show Hold time Column"));
			}
			act->setObjectName("6");
			if (fxList()->showColumnFadeoutFlag) {
				act = menu.addAction(tr("Hide FadeOut time column"));
			} else {
				act = menu.addAction(tr("Show FadeOut time column"));
			}
			act->setObjectName("7");
			if (fxList()->showColumnPostdelayFlag) {
				act = menu.addAction(tr("Hide PostDelay column"));
			} else {
				act = menu.addAction(tr("Show PostDelay column"));
			}
			act->setObjectName("8");
		}


		act = menu.exec(event->globalPos());
		if (!act) return;

		switch (act->objectName().toInt()) {
		case 20:
			AppCentral::instance()->addFxAudioDialog(fxList(),this);
			refreshList();
			break;
		case 21:
			fxList()->addFxAudioPlayList();
			refreshList();
			break;
		case 22:
			AppCentral::instance()->addDefaultSceneToFxList(fxList());
			refreshList();
			break;
		case 23:
			fxList()->addFxSequence();
			refreshList();
			break;
		case 2:
			setEditable(!isEditable());
			break;
		case 3:
			fxl->showColumnIdFlag = !fxl->showColumnIdFlag;
			refreshList();
			break;
		case 4:
			fxl->showColumnPredelayFlag = !fxl->showColumnPredelayFlag;
			refreshList();
			break;
		case 5:
			fxl->showColumnFadeinFlag = !fxl->showColumnFadeinFlag;
			refreshList();
			break;
		case 6:
			fxl->showColumnHoldFlag = !fxl->showColumnHoldFlag;
			refreshList();
			break;
		case 7:
			fxl->showColumnFadeoutFlag = !fxl->showColumnFadeoutFlag;
			refreshList();
			break;
		case 8:
			fxl->showColumnPostdelayFlag = !fxl->showColumnPostdelayFlag;
			refreshList();
			break;

		default:
			break;
		}

	} else {
		if (!FxItem::exists(item->linkedFxItem))
			return;
		FxItem *fx = item->linkedFxItem;
		FxType fxtype = FxType(fx->fxType());

		QMenu menu(this);
		QAction *act;
		act = menu.addAction(tr("Unselect"));
		act->setObjectName("2");
		act = menu.addAction(tr("Edit Fx name"));
		act->setObjectName("9");
		act = menu.addAction(tr("Open Fx properties"));
		act->setObjectName("4");
		if (fxtype == FX_SCENE || fxtype == FX_AUDIO_PLAYLIST || fxtype == FX_SEQUENCE) {
			act = menu.addAction(tr("Open Fx panel"));
			act->setObjectName("5");
		}
		act = menu.addAction(tr("Add Audio Fx"));
		act->setObjectName("3");
		act = menu.addAction(tr("------------"));
		act = menu.addAction(tr("Delete Fx"));
		act->setObjectName("1");
		if (fxtype == FX_SCENE) {
			act = menu.addAction(tr("Clone Fx Scene"));
			act->setObjectName("6");
		}
		act = menu.addAction(tr("------------"));
		if (fxtype == FX_SEQUENCE) {
			if (AppCentral::instance()->execCenter->findExecuter(fx)) {
				act = menu.addAction(tr("Stop Sequence"));
				act->setObjectName("7");
				act = menu.addAction(tr("Stop and BLACK Sequence"));
				act->setObjectName("8");
			}
		}

		act = menu.exec(event->globalPos());
		if (!act) return;

		switch (act->objectName().toInt()) {
		case 1:
			fxList()->deleteFx(item->linkedFxItem);
			break;
		case 2:
			setRowSelected(item->myRow,false);
			cur_clicked_item = 0;
			break;
		case 3:
			AppCentral::instance()->addFxAudioDialog(fxList(),this,item->myRow);
			refreshList();
			break;
		case 4:
			{
				FxItemPropertyWidget *editor = FxItemPropertyWidget::openFxPropertyEditor(item->linkedFxItem);
				if (editor) connect(editor,SIGNAL(modified()),this,SLOT(refreshList()));
			}
			break;
		case 5:
			emit fxTypeColumnDoubleClicked(item->linkedFxItem);
			break;
		case 6:
			fxList()->cloneSelectedSceneItem();
			refreshList();
			break;
		case 7:
			AppCentral::instance()->unitFx->stopFxSequence(reinterpret_cast<FxSeqItem*>(fx));
			break;
		case 8:
			AppCentral::instance()->unitFx->stopFxSequence(reinterpret_cast<FxSeqItem*>(fx));
			AppCentral::instance()->unitLight->blackFxItem(fx,200);
			break;
		case 9:
			fx->setName(QInputDialog::getText(this
											  ,tr("Edit")
											  ,tr("Enter name label for Fx")
											  ,QLineEdit::Normal
											  ,fx->name()));
			refreshList();
			break;
		default:
			break;
		}
	}
}


void FxListWidget::if_fxitemwidget_doubleclicked(FxListWidgetItem *listitem)
{
	if (!listitem) return;

	FxItem *fx = listitem->linkedFxItem;
	// bool editmode = AppCentral::instance()->isEditMode();

	if (fx) {
		switch(listitem->columnType) {
		case FxListWidgetItem::CT_NAME:
			return column_name_double_clicked(fx);
		case FxListWidgetItem::CT_FX_TYPE:
			return column_type_double_clicked(fx);
		default:
			break;
		}
	}

}

void FxListWidget::if_fxitemwidget_tab_pressed(FxListWidgetItem *listitem)
{
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	int newcol = listitem->myColumn+1;
	int newrow = listitem->myRow;

	if (listitem->columnType == FxListWidgetItem::CT_NAME)
		newcol++;

	FxListWidgetItem *nextitem = static_cast<FxListWidgetItem*>(fxTable->cellWidget(newrow,newcol));
	if (nextitem) {
		nextitem->itemEdit->selectAll();
		nextitem->itemEdit->setFocus();
	}

}

void FxListWidget::if_fxitemwidget_enter_pressed(FxListWidgetItem *listitem)
{
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	qDebug("Enter pressed in fx: %s : coltype: %d"
		   ,fx->name().toLocal8Bit().data(),listitem->columnType);

	int newcol = listitem->myColumn;
	int newrow = listitem->myRow+1;

	FxListWidgetItem *nextitem = static_cast<FxListWidgetItem*>(fxTable->cellWidget(newrow,newcol));
	if (nextitem) {
		nextitem->itemEdit->selectAll();
		nextitem->itemEdit->setFocus();
	}

}

void FxListWidget::if_fxitemwidget_edited(FxListWidgetItem *listitem, const QString &text)
{
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	switch(listitem->columnType) {
	case FxListWidgetItem::CT_NAME:
		fx->setName(text);
		break;
	case FxListWidgetItem::CT_KEY:
		fx->setKeyCode(QtStaticTools::stringToKey(text));
		break;
	case FxListWidgetItem::CT_PRE_DELAY:
		fx->setPreDelay(QtStaticTools::timeStringToMS(text));
		break;
	case FxListWidgetItem::CT_POST_DELAY:
		fx->setPostDelay(QtStaticTools::timeStringToMS(text));
		break;
	case FxListWidgetItem::CT_FADEIN_TIME:
		fx->setFadeInTime(QtStaticTools::timeStringToMS(text));
		break;
	case FxListWidgetItem::CT_FADEOUT_TIME:
		fx->setFadeOutTime(QtStaticTools::timeStringToMS(text));
		break;
	case FxListWidgetItem::CT_HOLD_TIME:
		fx->setHoldTime(QtStaticTools::timeStringToMS(text));
		break;

	default:
		break;
	}
}

void FxListWidget::if_fxitemwidget_seeked(FxListWidgetItem *listitem, int perMille)
{
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	switch(listitem->columnType) {
	case FxListWidgetItem::CT_NAME:
		if (fx->fxType() == FX_AUDIO) {
			/// @todo: Do this via AudioCmd Message
			AppCentral::instance()->unitAudio->seekPosPerMilleFxAudio(reinterpret_cast<FxAudioItem*>(fx),perMille);
		}
		break;
	case FxListWidgetItem::CT_KEY:
		break;
	case FxListWidgetItem::CT_PRE_DELAY:
		break;
	case FxListWidgetItem::CT_POST_DELAY:
		break;
	case FxListWidgetItem::CT_FADEIN_TIME:
		break;
	case FxListWidgetItem::CT_FADEOUT_TIME:
		break;
	case FxListWidgetItem::CT_HOLD_TIME:
		break;

	default:
		break;
	}

}

void FxListWidget::drop_event_receiver(QString str, int row)
{
	if (!str.startsWith("file://")) {
		DEBUGERROR("Add Drag'n'Drop File: %s not valid",str.toLatin1().data());
		return;
	} else {
		LOGTEXT(tr("Add Drag'n'Drop Fx: %1").arg(str));
	}

	QString path = str.mid(7);
	if (path.size()) {
		fxList()->addFxAudioSimple(path,row);
		refreshList();
	}
}


void FxListWidget::on_closeButton_clicked()
{
	FxListWidget::destroyFxListWidget(this);
}

void FxListWidget::on_editButton_clicked(bool checked)
{
	setEditable(checked);
}


#include "fxlistwidget.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "fxaudioitem.h"
#include "appcentral.h"
#include "scenedeskwidget.h"
#include "qtstatictools.h"
#include "customwidget/pslineedit.h"
#include "fxlistwidgetitem.h"
#include "customwidget/extmimedata.h"
#include "fxplaylistitem.h"
#include "fxplaylistwidget.h"

#include <QDebug>
#include <QLineEdit>
#include <QDrag>
#include <QPainter>


FxListWidget::FxListWidget(QWidget *parent) :
	QWidget(parent)
{
	init();

	setupUi(this);
	fxTable->setDragEnabled(true);
	fxTable->setSelectionMode(QAbstractItemView::SingleSelection);
	fxTable->setDragDropMode(QAbstractItemView::InternalMove);
	fxTable->setDropIndicatorShown(true);
	fxTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	fxTable->setSelectionMode(QAbstractItemView::NoSelection);

	QPalette pal = fxTable->palette();
	pal.setBrush(QPalette::Highlight,Qt::darkGreen);
	fxTable->setPalette(pal);

	// fxTable->viewport()->acceptDrops();
	connect(fxTable,SIGNAL(dropEventReceived(QString,int)),this,SIGNAL(dropEventReceived(QString,int)),Qt::QueuedConnection);
	connect(fxTable,SIGNAL(rowMovedFromTo(int,int)),this,SLOT(moveRowFromTo(int,int)),Qt::QueuedConnection);
}

void FxListWidget::setFxList(FxList *fxlist)
{
	qDebug() << "Set FxList";
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

	fxTable->setRowCount(rows);
	fxTable->setColumnCount(3);

	QStringList header;
	header << tr("Key") << tr("Name") << tr("Function") << tr("Id");
	fxTable->setHorizontalHeaderLabels(header);

	QFont key_font;
	key_font.setPointSize(14);
	key_font.setBold(true);

	for (int t=0; t<rows; t++) {
		int col = 0;
		FxItem *fx = fxlist->at(t);
		FxListWidgetItem *item;


		QString key;
		if (fx->keyCode() != Qt::Key_Space) {
			key = QtStaticTools::keyToString(fx->keyCode());
		}

		item = new_fxlistwidgetitem(fx,key,FxListWidgetItem::CT_KEY);
		item->itemEdit->setMinimized(true);
		item->itemEdit->setSingleKeyEditEnabled(true);
		item->itemEdit->setFont(key_font);
		item->itemEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		item->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Preferred);
		// item->setMaximumWidth(60);
		item->myRow = t;
		item->myColumn = col;
		fxTable->setCellWidget(t,col++,item);

		item = new_fxlistwidgetitem(fx,fx->name(),FxListWidgetItem::CT_NAME);
		item->itemEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		item->myRow = t;
		item->myColumn = col;
		fxTable->setCellWidget(t,col++,item);

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
		default:
			break;
		}
		item->itemEdit->hide();
		item->myRow = t;
		item->myColumn = col;
		fxTable->setCellWidget(t,col++,item);

		item = new_fxlistwidgetitem(fx,QString::number(fx->id()),FxListWidgetItem::CT_ID);
		item->setNeverEditable(true);
		item->myRow = t;
		item->myColumn = col;
		fxTable->setCellWidget(t,col++,item);
	}
	fxTable->resizeColumnsToContents();
	// fxTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
#ifdef IS_QT5
	fxTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
#else
	fxTable->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
#endif
	autoProceedCheck->setChecked(fxlist->autoProceedSequence());
}

void FxListWidget::setAutoProceedSequence(bool state)
{
	if (myfxlist) {
		autoProceedCheck->setChecked(state);
		myfxlist->setAutoProceedSequence(state);
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

void FxListWidget::selectFx(FxItem *fx)
{
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

void FxListWidget::initRowDrag(FxListWidgetItem *item)
{
	QDrag *drag = new QDrag(parentWidget());
	ExtMimeData *mdata = new ExtMimeData();
	mdata->fxListWidgetItem = item;
	mdata->setText(item->linkedFxItem->name());
	mdata->tableRow = item->myRow;
	mdata->tableCol = item->myColumn;

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
	qDebug("dropaction: %d",dropaction);
	if (dropaction != Qt::MoveAction) {
		refreshList();
	}
}

void FxListWidget::init()
{
	is_modified_f = false;
	is_editable_f = false;
	cur_selected_item = 0;
}

void FxListWidget::open_scence_desk(FxSceneItem *fx)
{

	SceneDeskWidget *desk = SceneDeskWidget::openSceneDesk(fx);

	if (desk) {
		connect(desk,SIGNAL(modified()),this,SLOT(refreshList()));
		desk->show();
	}
}

void FxListWidget::open_audio_list_widget(FxPlayListItem *fx)
{
	FxPlayListWidget *play = new FxPlayListWidget(fx);
	play->show();
}

FxListWidgetItem *FxListWidget::new_fxlistwidgetitem(FxItem *fx, const QString &text, int coltype)
{
	FxListWidgetItem * item = new FxListWidgetItem(fx,text,static_cast<FxListWidgetItem::ColumnType>(coltype));
	connect(item,SIGNAL(itemClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_clicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemDoubleClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_doubleclicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemTextEdited(FxListWidgetItem*,QString)),this,SLOT(if_fxitemwidget_edited(FxListWidgetItem*,QString)));
	connect(this,SIGNAL(editableChanged(bool)),item,SLOT(setEditable(bool)));
	connect(item,SIGNAL(draged(FxListWidgetItem*)),this,SLOT(initRowDrag(FxListWidgetItem*)),Qt::QueuedConnection);

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
		FxListWidgetItem *item = widlist.at(1);
		if (scene->isActive()) {
			item->setActivationProgress(perMilleA,perMilleB);
		}
		else if (scene->isVisible()) {
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
	if (msg.ctrlCmd == CMD_STATUS_REPORT) {
		int row = getRowThatContainsFxItem(msg.fxAudio);
		WidItemList widlist = getItemListForRow(row);
		if (widlist.size()) {
			FxListWidgetItem *item = widlist.at(1);

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
	qDebug("FxListWidget -> clicked -> coltype %d -> %s",listitem->columnType,listitem->text().toLocal8Bit().data());
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


		break;
	default:
		qDebug("Click on this column not implemented yet");

	}
}

void FxListWidget::column_name_double_clicked(FxItem *fx)
{
	switch (fx->fxType()) {
	case FX_SCENE:
		selectFx(fx);
		emit fxCmdActivated(fx, CMD_FX_START);
		break;
	case FX_AUDIO:
		selectFx(fx);
		emit fxCmdActivated(fx,CMD_FX_START);
		break;
	}
}

void FxListWidget::column_type_double_clicked(FxItem *fx)
{
	switch (fx->fxType()) {
	case FX_SCENE:
		open_scence_desk(static_cast<FxSceneItem*>(fx));
		break;
	case FX_AUDIO:
		selectFx(fx);
		emit fxCmdActivated(fx,CMD_FX_START);
		break;
	case FX_AUDIO_PLAYLIST:
		open_audio_list_widget(static_cast<FxPlayListItem*>(fx));

		break;
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

void FxListWidget::if_fxitemwidget_edited(FxListWidgetItem *listitem, const QString &text)
{
	FxItem *fx = listitem->linkedFxItem;
	if (!FxItem::exists(fx)) return;

	switch(listitem->columnType) {
	case FxListWidgetItem::CT_NAME:
		fx->setName(text);
		fx->setModified(true);
		break;
	case FxListWidgetItem::CT_KEY:
		fx->setKeyCode(QtStaticTools::stringToKey(text));
		break;
	default:
		break;
	}
}


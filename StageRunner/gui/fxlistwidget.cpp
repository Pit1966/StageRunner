#include "fxlistwidget.h"
#include "fxlist.h"
#include "fxitem.h"
#include "fxsceneitem.h"
#include "appcentral.h"
#include "scenedeskwidget.h"
#include "qtstatictools.h"
#include "customwidget/pslineedit.h"
#include "fxlistwidgetitem.h"

#include <QDebug>


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
	// fxTable->viewport()->acceptDrops();
	connect(fxTable,SIGNAL(dropEventReceived(QString,int)),this,SIGNAL(dropEventReceived(QString,int)),Qt::QueuedConnection);
	connect(fxTable,SIGNAL(rowMovedFromTo(int,int)),this,SLOT(moveRowFromTo(int,int)),Qt::QueuedConnection);
}

void FxListWidget::setFxList(FxList *fxlist)
{
	cur_selected_item = 0;
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
		item->itemEdit->setSingleKeyEditEnabled(true);
		item->itemEdit->setFont(key_font);
		item->setMaximumWidth(50);
		fxTable->setCellWidget(t,col++,item);

		item = new_fxlistwidgetitem(fx,fx->name(),FxListWidgetItem::CT_NAME);
		item->itemEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		fxTable->setCellWidget(t,col++,item);

		item = new_fxlistwidgetitem(fx,QString::number(fx->fxType()),FxListWidgetItem::CT_FX_TYPE);
		item->setNeverEditable(true);
		fxTable->setCellWidget(t,col++,item);

		item = new_fxlistwidgetitem(fx,QString::number(fx->id()),FxListWidgetItem::CT_ID);
		item->setNeverEditable(true);
		fxTable->setCellWidget(t,col++,item);
	}
	fxTable->resizeColumnsToContents();
	autoProceedCheck->setChecked(fxlist->autoProceedSequence());
}

void FxListWidget::setAutoProceedSequence(bool state)
{
	if (myfxlist) {
		autoProceedCheck->setChecked(state);
		myfxlist->setAutoProceedSequence(state);
	}
}

void FxListWidget::selectFx(FxItem *fx)
{
	bool found = false;
	int row=0;
	while (fx && row < fxTable->rowCount() && !found) {
		FxListWidgetItem * item = (FxListWidgetItem*)fxTable->cellWidget(row,0);
		if (item->linkedFxItem == fx) {
			fxTable->clearSelection();
			fxTable->setRangeSelected(QTableWidgetSelectionRange(row,0,row,1),true);
			found = true;
			if (cur_selected_item != fx) {
				cur_selected_item = fx;
				emit fxItemSelected(fx);
			}
		}
		row++;
	}
	if (!found) {
		emit fxItemSelected(0);
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

	SceneDeskWidget *desk = new SceneDeskWidget(fx);
	desk->show();
}

FxListWidgetItem *FxListWidget::new_fxlistwidgetitem(FxItem *fx, const QString &text, int coltype)
{
	FxListWidgetItem * item = new FxListWidgetItem(fx,text,static_cast<FxListWidgetItem::ColumnType>(coltype));
	connect(item,SIGNAL(itemClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_clicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemDoubleClicked(FxListWidgetItem*)),this,SLOT(if_fxitemwidget_doubleclicked(FxListWidgetItem*)));
	connect(item,SIGNAL(itemTextEdited(FxListWidgetItem*,QString)),this,SLOT(if_fxitemwidget_edited(FxListWidgetItem*,QString)));
	connect(this,SIGNAL(editableChanged(bool)),item,SLOT(setEditable(bool)));

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
		is_editable_f = state;
		emit editableChanged(state);
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

	// qDebug("FxListWidget -> clicke -> coltype %d -> %s",listitem->columnType,listitem->text().toLocal8Bit().data());
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
	}
}



void FxListWidget::if_fxitemwidget_doubleclicked(FxListWidgetItem *listitem)
{
	if (!listitem) return;

	FxItem *fx = listitem->linkedFxItem;
	bool editmode = AppCentral::instance()->isEditMode();

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
		break;
	default:
		fx->setKeyCode(QtStaticTools::stringToKey(text));
		break;
	}
}

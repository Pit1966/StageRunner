#include "fxlistwidget.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"

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
	fxTable->setColumnCount(2);

	QStringList header;
	header << tr("Name") << tr("Id");
	fxTable->setHorizontalHeaderLabels(header);

	for (int t=0; t<rows; t++) {
		FxItem *fx = fxlist->at(t);
		FxListWidgetItem *item;

		item = new FxListWidgetItem(fx,fx->displayName());
		item->columnType = FxListWidgetItem::CT_NAME;
		fxTable->setItem(t,0,item);

		item = new FxListWidgetItem(fx,QString::number(fx->fxID()));
		item->columnType = FxListWidgetItem::CT_ID;
		fxTable->setItem(t,1,item);
	}
	fxTable->resizeColumnsToContents();
	autoProceedCheck->setChecked(fxlist->autoProceedSequence());
	qDebug() << "setFxList";

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
	while (row < fxTable->rowCount() && !found) {
		FxListWidgetItem * item = (FxListWidgetItem*)fxTable->item(row,0);
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
}

void FxListWidget::init()
{
	is_modified_f = false;
	cur_selected_item = 0;
}

void FxListWidget::refreshList()
{
	qDebug("FxListWidget refresh");
	if (myfxlist) {
		setFxList(myfxlist);
	}
}

FxListWidgetItem::FxListWidgetItem(FxItem *fxitem, const QString &text)
	: QTableWidgetItem(text)
{
	columnType = CT_UNDEF;
	linkedFxItem = fxitem;

}

void FxListWidget::on_fxTable_itemClicked(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	FxItem *fx = myitem->linkedFxItem;

	if (fx) {
		qDebug() << "clicked:" << fx->displayName() << "ColType:" << myitem->columnType;
		myfxlist->setNextFx(fx);
		if (cur_selected_item != fx) {
			cur_selected_item = fx;
			emit fxItemSelected(fx);
		}
	}
}

void FxListWidget::on_fxTable_itemDoubleClicked(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	FxItem *fx = myitem->linkedFxItem;

	if (fx) {
		qDebug() << "double clicked:" << fx->displayName() << "ColType:" << myitem->columnType;
		emit fxCmdActivated(fx,CMD_FX_START);
	}
}

void FxListWidget::moveRowFromTo(int srcrow, int destrow)
{
	if (myfxlist) {
		FxItem *cur = 0;
		if (srcrow >= 0 && srcrow < myfxlist->size()) cur = myfxlist->at(srcrow);
		myfxlist->moveFromTo(srcrow,destrow);
		// setFxList(myfxlist);
		if (cur) {
			qDebug() << "select current:" << cur->displayName();
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
	if (myitem->columnType == FxListWidgetItem::CT_NAME) {
		fx->setDisplayName(myitem->text());
		emit listModified();
	}
}

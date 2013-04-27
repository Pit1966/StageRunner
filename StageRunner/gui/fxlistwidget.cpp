#include "fxlistwidget.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"
#include "appcontrol/appcentral.h"

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
	fxTable->setColumnCount(3);

	QStringList header;
	header << tr("Key") << tr("Name") << tr("Id");
	fxTable->setHorizontalHeaderLabels(header);

	QFont key_font;
	key_font.setPointSize(14);
	key_font.setBold(true);

	for (int t=0; t<rows; t++) {
		int col = 0;
		FxItem *fx = fxlist->at(t);
		FxListWidgetItem *item;

		QChar key;
		if (fx->keyCode() > 0) {
			key = fx->keyCode();
		} else {
			key = ' ';
		}
		item = new FxListWidgetItem(fx,key);
		item->setFont(key_font);
		item->columnType = FxListWidgetItem::CT_KEY;
		fxTable->setItem(t,col++,item);

		item = new FxListWidgetItem(fx,fx->displayName());
		item->columnType = FxListWidgetItem::CT_NAME;
		fxTable->setItem(t,col++,item);

		item = new FxListWidgetItem(fx,QString::number(fx->fxID()));
		item->columnType = FxListWidgetItem::CT_ID;
		fxTable->setItem(t,col++,item);


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
	if (!found) {
		emit fxItemSelected(0);
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
	} else {
		myfxlist->setNextFx(0);
	}
}

void FxListWidget::on_fxTable_itemDoubleClicked(QTableWidgetItem *item)
{
	FxListWidgetItem *myitem = reinterpret_cast<FxListWidgetItem*>(item);
	FxItem *fx = myitem->linkedFxItem;

	if (fx) {
		qDebug() << "double clicked:" << fx->displayName() << "ColType:" << myitem->columnType;
		if (!AppCentral::instance()->isEditMode()) {
			emit fxCmdActivated(fx,CMD_FX_START);
		}
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
	switch (myitem->columnType) {
	case FxListWidgetItem::CT_NAME:
		fx->setDisplayName(myitem->text());
		emit listModified();
		break;
	case FxListWidgetItem::CT_KEY:
		{
			ushort old_code = fx->keyCode();
			ushort code = 0;
			QString text = myitem->text();
			if (text.size()) {
				code = text.at(0).toUpper().unicode();
			}
			if (old_code != code) {
				fx->setKeyCode(code);
				emit listModified();
			}
			myitem->setText(QString(QChar(code)));
		}
	default:
		break;
	}
}

#include "fxlistwidget.h"
#include "fx/fxlist.h"
#include "fx/fxitem.h"

#include <QDebug>




FxListWidget::FxListWidget(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
	fxTable->setDragDropMode(QAbstractItemView::InternalMove);
	fxTable->setDragEnabled(true);
	fxTable->setDropIndicatorShown(true);

	is_modified_f = false;
}

void FxListWidget::setFxList(const FxList *fxlist)
{
	fxTable->clear();
	is_modified_f = true;

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
		emit fxCmdActivated(fx,CMD_FX_START);
	}

}

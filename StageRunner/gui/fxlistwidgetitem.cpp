#include "fxlistwidgetitem.h"
#include "customwidget/pslineedit.h"
#include "customwidget/extmimedata.h"
#include "fxitem.h"

#include <QMouseEvent>
#include <QDrag>

FxListWidgetItem::FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype) :
	QWidget()
  ,linkedFxItem(fx)
  ,itemText(text)
  ,columnType(coltype)
  ,myRow(-1)
  ,myColumn(-1)
  ,is_editable_f(false)
  ,is_never_editable_f(false)
{
	init();

}

FxListWidgetItem::FxListWidgetItem(const FxListWidgetItem &other) :
	QWidget()
  ,linkedFxItem(other.linkedFxItem)
  ,itemText(other.itemText)
  ,columnType(other.columnType)
  ,myRow(other.myRow)
  ,myColumn(other.myColumn)
  ,is_editable_f(other.is_editable_f)
  ,is_never_editable_f(other.is_never_editable_f)
{
	init();
	itemEdit->setReadOnly(other.itemEdit->isReadOnly());
}

FxListWidgetItem::~FxListWidgetItem()
{
}

void FxListWidgetItem::setText(const QString &txt)
{
	itemText = txt;
	itemEdit->setText(txt);
}

void FxListWidgetItem::init()
{
	current_button = 0;

	setupUi(this);
	itemLabel->clear();
	itemEdit->setText(itemText);
	itemEdit->setReadOnly(true);
	itemEdit->setEventsShared(true);

	connect(itemEdit,SIGNAL(clicked()),this,SLOT(if_edit_item_clicked()));
	connect(itemEdit,SIGNAL(doubleClicked()),this,SLOT(if_edit_item_doubleclicked()));
	connect(itemEdit,SIGNAL(textEdited(QString)),this,SLOT(if_edit_item_edited(QString)));
	connect(itemLabel,SIGNAL(doubleClicked()),this,SLOT(if_label_item_doubleclicked()));
}

void FxListWidgetItem::mousePressEvent(QMouseEvent *event)
{
	current_button = event->button();
	if (current_button == Qt::LeftButton) {
		drag_begin_pos = event->pos();
	}
}

void FxListWidgetItem::mouseReleaseEvent(QMouseEvent *)
{
	current_button = 0;
}

void FxListWidgetItem::mouseMoveEvent(QMouseEvent *event)
{
	QPoint dist = event->pos() - drag_begin_pos;

	if (current_button == Qt::LeftButton && dist.manhattanLength() > 10) {

		FxListWidgetItem *temp = new FxListWidgetItem(*this);
		temp->setObjectName("TestTest");

		ExtDrag *drag = new ExtDrag(temp,true);
		ExtMimeData *mdata = new ExtMimeData();
		QPixmap pixmap(size());
		render(&pixmap);

		mdata->fxListWidgetItem = this;
		mdata->setText(this->linkedFxItem->name());
		mdata->tableRow = myRow;
		mdata->tableCol = myColumn;
		drag->setMimeData(mdata);
		drag->setPixmap(pixmap);
		drag->setHotSpot(drag_begin_pos);
		Qt::DropAction dropaction = drag->exec();
		qDebug("DropAction: %d", dropaction);

		// temp->deleteLater();
	}
}

void FxListWidgetItem::setEditable(bool state)
{
	if (is_never_editable_f) state = false;

	if (state != is_editable_f) {
		is_editable_f = state;
		itemEdit->setReadOnly(!state);
		if (state) {
			itemEdit->setStyleSheet("background-color: #ffe0e0");
		} else {
			itemEdit->setStyleSheet("");
		}
	}
}

void FxListWidgetItem::setNeverEditable(bool state)
{
	if (state != is_never_editable_f) {
		is_never_editable_f = state;
		if (state) setEditable(false);
	}
}

void FxListWidgetItem::if_edit_item_clicked()
{
	// qDebug("FxlistWigetItem -> clicked");
	emit itemClicked(this);
}

void FxListWidgetItem::if_edit_item_doubleclicked()
{
	emit itemDoubleClicked(this);
}

void FxListWidgetItem::if_edit_item_edited(const QString &text)
{
	emit itemTextEdited(this, text);
}

void FxListWidgetItem::if_label_item_doubleclicked()
{
	emit itemDoubleClicked(this);
}


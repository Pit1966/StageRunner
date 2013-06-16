#include "fxlistwidgetitem.h"
#include "customwidget/pslineedit.h"
#include "fxitem.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QtWidgets>

FxListWidgetItem::FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype) :
	QWidget()
  ,linkedFxItem(fx)
  ,itemText(text)
  ,columnType(coltype)
  ,myRow(-1)
  ,myColumn(-1)
  ,is_editable_f(false)
  ,is_never_editable_f(false)
  ,is_selected_f(false)
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
	activation_indicator = 0;

	setupUi(this);
	itemLabel->clear();
	itemEdit->setText(itemText);
	itemEdit->setReadOnly(true);
	itemEdit->setEventsShared(true);

	connect(itemEdit,SIGNAL(clicked()),this,SLOT(if_edit_item_clicked()));
	connect(itemEdit,SIGNAL(doubleClicked()),this,SLOT(if_edit_item_doubleclicked()));
	connect(itemEdit,SIGNAL(textEdited(QString)),this,SLOT(if_edit_item_edited(QString)));
	connect(itemLabel,SIGNAL(doubleClicked()),this,SLOT(if_label_item_doubleclicked()));

	org_palette = palette();
	org_palette.setBrush(QPalette::Base,Qt::NoBrush);
	setPalette(org_palette);

	setAutoFillBackground(false);
	itemEdit->setAutoFillBackground(false);
	itemLabel->setAutoFillBackground(false);
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
		emit draged(this);
		current_button = 0;

	}
}

void FxListWidgetItem::paintEvent(QPaintEvent *event)
{
	if (activation_indicator) {
		QPainter p(this);
		p.setPen(Qt::red);
		p.setBrush(Qt::red);
		int w = event->rect().width();
		int h = event->rect().height();
		int wp = activation_indicator * w / 1000;
		p.drawRect(0,h-4,wp,4);
	}

}

void FxListWidgetItem::setEditable(bool state)
{
	if (is_never_editable_f) state = false;

	if (state != is_editable_f) {

		is_editable_f = state;
		itemEdit->setReadOnly(!state);
		itemEdit->setWarnColor(state);
		itemEdit->setEditable(state);

		if (state) {
			QPalette pal = palette();
			pal.setBrush(QPalette::Base, itemEdit->palette().base());
			setPalette(pal);
		} else {
			setPalette(org_palette);
		}
		update();
	}
}

void FxListWidgetItem::setNeverEditable(bool state)
{
	if (state != is_never_editable_f) {
		is_never_editable_f = state;
		if (state) setEditable(false);
	}
}

void FxListWidgetItem::setSelected(bool state)
{
	if (is_selected_f != state) {
		is_selected_f = state;
		if (state) {

		} else {

		}
	}

}

void FxListWidgetItem::setActivationProgress(int perMille)
{
	if (perMille != activation_indicator) {
		activation_indicator = perMille;
		update();
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


//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "fxlistwidgetitem.h"
#include "customwidget/pslineedit.h"
#include "fxitem.h"
#include "tool/qt6_qt5_compat.h"

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
  ,is_marked_f(false)
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
  ,is_selected_f(false)
  ,is_marked_f(false)
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

QSize FxListWidgetItem::sizeHint() const
{
	// qDebug("sizeHint");
	if (columnType == CT_ID)
		return QSize(itemText.size() * 14, 20);

	return QSize(20,20);
}

QSize FxListWidgetItem::minimumSizeHint() const
{
	// qDebug("minimumSizeHint");
	return QSize();
}

void FxListWidgetItem::setTextColor(const QString &col)
{
	QPalette pal = palette();
	pal.setColor(QPalette::Text, QColor(col));
	setPalette(pal);
}

void FxListWidgetItem::setTextCentered(bool state)
{
	if (state) {
		itemLabel->show();
		// itemExtra->show();
	} else {
		itemLabel->hide();
	}
}

void FxListWidgetItem::init()
{
	current_button = 0;
	activation_indicator_a = 0;
	activation_indicator_b = 0;
	indicator_a_color = QColor(QColor(220,180,0));   // orange
	indicator_b_color = QColor(Qt::darkRed);
	marked_color = QColor(QColor(20,20,200,80));

	seek_mode_f = false;

	setupUi(this);
	itemLabel->clear();
	itemExtra->clear();
	itemEdit->setText(itemText);
	itemEdit->setReadOnly(true);
	itemEdit->setEventsShared(true);

	connect(itemEdit,SIGNAL(clicked()),this,SLOT(if_edit_item_clicked()));
	connect(itemEdit,SIGNAL(doubleClicked()),this,SLOT(if_edit_item_doubleclicked()));
	connect(itemEdit,SIGNAL(textEdited(QString)),this,SLOT(if_edit_item_edited(QString)));
	connect(itemEdit,SIGNAL(tabPressed()),this,SLOT(if_edit_item_tab_pressed()));
	connect(itemEdit,SIGNAL(enterPressed()),this,SLOT(if_edit_item_enter_pressed()));
	connect(itemLabel,SIGNAL(doubleClicked()),this,SLOT(if_label_item_doubleclicked()));

	org_palette = palette();
	org_palette.setBrush(QPalette::Base,Qt::NoBrush);
	setPalette(org_palette);

	setAutoFillBackground(false);
	itemEdit->setAutoFillBackground(false);
	itemLabel->setAutoFillBackground(false);
	itemExtra->setAutoFillBackground(false);

	itemEdit->setContextMenuPolicy(Qt::NoContextMenu);
	itemLabel->setContextMenuPolicy(Qt::NoContextMenu);
	itemExtra->setContextMenuPolicy(Qt::NoContextMenu);

	itemExtra->hide();
}

void FxListWidgetItem::mousePressEvent(QMouseEvent *event)
{
	current_button = event->button();
	if (current_button == Qt::LeftButton) {
		drag_begin_pos = event->pos();
	}
	else if (current_button == Qt::MiddleButton) {
		seek_mode_f = true;
	}
}

void FxListWidgetItem::mouseReleaseEvent(QMouseEvent *)
{
	current_button = 0;
	seek_mode_f = false;
}

void FxListWidgetItem::mouseMoveEvent(QMouseEvent *event)
{
	QPoint dist = event->pos() - drag_begin_pos;

	if (seek_mode_f) {
		int pMille = 1000 * event->x() / width();
		if (pMille < 0) {
			emit seekToPerMille(this,0);
		}
		else if (pMille > 1000) {
			emit seekToPerMille(this,1000);
		}
		else {
			emit seekToPerMille(this,pMille);
		}
	}
	else if (current_button == Qt::LeftButton && dist.manhattanLength() > 10) {
		emit draged(this);
		current_button = 0;
	}
}

void FxListWidgetItem::paintEvent(QPaintEvent *event)
{
	if (is_marked_f) {
		QPainter p(this);
		p.setPen(Qt::NoPen);
		p.setBrush(marked_color);
		p.drawRect(event->rect());
	}
	else if (linkedFxItem->playedRandom()) {
		QPainter p(this);
		p.setPen(Qt::NoPen);
		p.setBrush(marked_color);
		p.drawRect(event->rect());
	}
	if (activation_indicator_a) {
		QPainter p(this);
		p.setPen(indicator_a_color);
		p.setBrush(indicator_a_color);
		int w = event->rect().width();
		int h = event->rect().height();
		int wp = activation_indicator_a * w / 1000;
		p.drawRect(0,h-2,wp,2);
	}
	if (activation_indicator_b) {
		QPainter p(this);
		p.setPen(indicator_b_color);
		p.setBrush(indicator_b_color);
		int w = event->rect().width();
		int h = event->rect().height();
		int wp = activation_indicator_b * w / 1000;
		p.drawRect(0,h-5,wp,2);
	}

}

void FxListWidgetItem::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();

	if (key == Qt::Key_F2) {
		linkedFxItem->setName(QInputDialog::getText(this
													,tr("Edit")
													,tr("Enter name for Fx")
													,QLineEdit::Normal
													,linkedFxItem->name()));
		setText(linkedFxItem->name());
		update();
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
		update();
	}

}

void FxListWidgetItem::setMarked(bool state)
{
	if (state != is_marked_f) {
		is_marked_f = state;
		update();
	}
}

void FxListWidgetItem::setActivationProgress(int perMilleA, int perMilleB)
{
	setActivationProgressA(perMilleA);
	setActivationProgressB(perMilleB);
}

void FxListWidgetItem::setActivationProgressB(int perMilleB)
{
	if (perMilleB < 0)
		return;

	if (perMilleB != activation_indicator_b) {
		activation_indicator_b = perMilleB;
		update();
	}
}

void FxListWidgetItem::setActivationProgressA(int perMilleA)
{
	if (perMilleA < 0)
		return;

	if (perMilleA != activation_indicator_a) {
		activation_indicator_a = perMilleA;
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

void FxListWidgetItem::if_edit_item_tab_pressed()
{
	emit tabPressedInEdit(this);
}

void FxListWidgetItem::if_edit_item_enter_pressed()
{
	emit enterPressedInEdit(this);
}

void FxListWidgetItem::if_label_item_doubleclicked()
{
	emit itemDoubleClicked(this);
}


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

#include <QtWidgets>

FxListWidgetItem::FxListWidgetItem(FxItem *fx, const QString &text, ColumnType coltype) :
	QWidget()
  ,linkedFxItem(fx)
  ,itemText(text)
  ,columnType(coltype)
  ,myRow(-1)
  ,myColumn(-1)
  ,m_isEditable(false)
  ,m_isNeverEditable(false)
  ,m_isSelected(false)
  ,m_isMarked(false)
  ,m_isSeeked(false)
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
  ,m_isEditable(other.m_isEditable)
  ,m_isNeverEditable(other.m_isNeverEditable)
  ,m_isSelected(false)
  ,m_isMarked(false)
  ,m_colorMarked(other.m_colorMarked)
  ,m_statusFont(other.m_statusFont)
  ,m_statusText(other.m_statusText)
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
	m_currentButton = 0;
	m_activationIndicatorA = 0;
	m_activationIndicatorB = 0;
	m_colorIndicatorA = QColor(QColor(220,180,0));   // orange
	m_colorIndicatorB = QColor(Qt::darkRed);
	m_colorMarked = QColor(QColor(20,20,200,80));
	m_colorStatus = QColor(Qt::darkYellow);

	m_isSeeked = false;

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

	m_orginalPalette = palette();
	m_orginalPalette.setBrush(QPalette::Base,Qt::NoBrush);
	setPalette(m_orginalPalette);

	setAutoFillBackground(false);
	itemEdit->setAutoFillBackground(false);
	itemLabel->setAutoFillBackground(false);
	itemExtra->setAutoFillBackground(false);

	itemEdit->setContextMenuPolicy(Qt::NoContextMenu);
	itemLabel->setContextMenuPolicy(Qt::NoContextMenu);
	itemExtra->setContextMenuPolicy(Qt::NoContextMenu);

	itemExtra->hide();

	m_statusFont = font();
	m_statusFont.setPointSize(8);
}

void FxListWidgetItem::mousePressEvent(QMouseEvent *event)
{
	m_currentButton = event->button();
	if (m_currentButton == Qt::LeftButton) {
		m_dragBeginPos = event->pos();
	}
	else if (m_currentButton == Qt::MiddleButton) {
		m_isSeeked = true;
	}
}

void FxListWidgetItem::mouseReleaseEvent(QMouseEvent *)
{
	m_currentButton = 0;
	m_isSeeked = false;
}

void FxListWidgetItem::mouseMoveEvent(QMouseEvent *qevent)
{
	PMouseEvent *event = static_cast<PMouseEvent*>(qevent);
	QPoint dist = event->pos() - m_dragBeginPos;

	if (m_isSeeked) {
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
	else if (m_currentButton == Qt::LeftButton && dist.manhattanLength() > 10) {
		emit draged(this);
		m_currentButton = 0;
	}
}

void FxListWidgetItem::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	if (m_isMarked) {
		p.setPen(Qt::NoPen);
		p.setBrush(m_colorMarked);
		p.drawRect(event->rect());
	}
	else if (linkedFxItem->playedRandom()) {
		// QPainter p(this);
		p.setPen(Qt::NoPen);
		p.setBrush(m_colorMarked);
		p.drawRect(event->rect());
	}
	if (m_activationIndicatorA) {
		// QPainter p(this);
		p.setPen(m_colorIndicatorA);
		p.setBrush(m_colorIndicatorA);
		int w = event->rect().width();
		int h = event->rect().height();
		int wp = m_activationIndicatorA * w / 1000;
		p.drawRect(0,h-2,wp,2);
	}
	if (m_activationIndicatorB) {
		// QPainter p(this);
		p.setPen(m_colorIndicatorB);
		p.setBrush(m_colorIndicatorB);
		int w = event->rect().width();
		int h = event->rect().height();
		int wp = m_activationIndicatorB * w / 1000;
		p.drawRect(0,h-5,wp,2);
	}

	if (m_statusText.size()) {
		// QPainter p(this);
		p.setFont(m_statusFont);
		p.setPen(m_colorStatus);
		p.drawText(rect(), m_statusText, QTextOption(Qt::AlignRight));
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
	if (m_isNeverEditable) state = false;

	if (state != m_isEditable) {

		m_isEditable = state;
		itemEdit->setReadOnly(!state);
		itemEdit->setWarnColor(state);
		itemEdit->setEditable(state);

		if (state) {
			QPalette pal = palette();
			pal.setBrush(QPalette::Base, itemEdit->palette().base());
			setPalette(pal);
		} else {
			setPalette(m_orginalPalette);
		}
		update();
	}
}

void FxListWidgetItem::setNeverEditable(bool state)
{
	if (state != m_isNeverEditable) {
		m_isNeverEditable = state;
		if (state) setEditable(false);
	}
}

void FxListWidgetItem::setSelected(bool state)
{
	if (m_isSelected != state) {
		m_isSelected = state;
		update();
	}

}

void FxListWidgetItem::setMarked(bool state)
{
	if (state != m_isMarked) {
		m_isMarked = state;
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

	if (perMilleB != m_activationIndicatorB) {
		m_activationIndicatorB = perMilleB;
		update();
	}
}

void FxListWidgetItem::setActivationProgressA(int perMilleA)
{
	if (perMilleA < 0)
		return;

	if (perMilleA != m_activationIndicatorA) {
		m_activationIndicatorA = perMilleA;
		update();
	}
}

void FxListWidgetItem::setStatusText(const QString &txt)
{
	m_statusText = txt;
	update();
}

void FxListWidgetItem::setFxStatus(FxItem */*fx*/, const QString &msg)
{
	m_statusText = msg;
	update();
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


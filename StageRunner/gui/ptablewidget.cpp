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

#include "ptablewidget.h"
#include "customwidget/extmimedata.h"
#include "fxitem.h"
#include "fxlistwidgetitem.h"
#include "qt6_qt5_compat.h"

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QModelIndex>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QScrollBar>
#include <QPoint>
#include <QToolTip>

PTableWidget::PTableWidget(QWidget *parent) :
	QTableWidget(parent)
{
	drag_start_row = -1;
	drag_dest_row = -1;
	drag_temp_row = -1;
	current_vert_scrollpos = 0;

	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(if_scrolled_vertical(int)));

}

void PTableWidget::clearDragAndDropAction()
{
	drag_temp_row = -1;
}

void PTableWidget::clear()
{
	clearDragAndDropAction();
	QTableWidget::clear();
}

void PTableWidget::saveScrollPos()
{
	current_vert_scrollpos = verticalScrollBar()->value();
}

void PTableWidget::setOldScrollPos()
{
	verticalScrollBar()->setValue(current_vert_scrollpos);
}

void PTableWidget::setDropAllowedIndices(QList<int> list)
{
	allowed_indices = list;
}

void PTableWidget::dragEnterEvent(QDragEnterEvent *qevent)
{
	PDragEnterEvent* event = static_cast<PDragEnterEvent*>(qevent);
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);
	QObject * src = event->source();
	QPoint dragpos = event->pos();
	int current_row = indexAt(dragpos).row();
	// qDebug() << event->possibleActions();

	if (extmime)
		extmime->dragObject->setDragCursor(QPixmap(),Qt::MoveAction);

	if (extmime && extmime->fxListWidgetItem) { // note: this functionality can be done by event->source() too
		qDebug("PTableWidget::dragEnterEvent: Mime:'%s': ObjectName:%s, row:%d, col:%d "
			   ,mime->text().toLocal8Bit().data(),src->objectName().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);

		// event->setDropAction(Qt::MoveAction);
		event->accept();
		drag_start_row = extmime->tableRow;

		saveScrollPos();
		insertRow(current_row);
		setOldScrollPos();
		drag_temp_row = current_row;

	} else {
		qDebug ("PTableWidget::dragEnterEvent: external");
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
}

void PTableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	Q_UNUSED(event);
	qDebug("PTableWidget::dragLeaveEvent");

	if (drag_temp_row >= 0) {
		saveScrollPos();
		removeRow(drag_temp_row);
		setOldScrollPos();
		drag_temp_row = -1;
	}
}

void PTableWidget::dragMoveEvent(QDragMoveEvent *qevent)
{
	PDragMoveEvent* event = static_cast<PDragMoveEvent*>(qevent);
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	if (!extmime || !extmime->fxListWidgetItem)
		return;	// external Source, not from PTableWidget

	QPoint dragpos = event->pos();
	// int current_row = indexAt(dragpos).row();

	// This event is only for interest if its origin came from other PTableWidget
	if (extmime->originPTableWidget != this) {
//		qDebug("PTableWidget::dragMoveEvent: Mime:'%s': ObjectName:%s, row:%d, col:%d -> current row:%d"
//			   ,mime->text().toLocal8Bit().data()
//			   ,src->objectName().toLocal8Bit().data()
//			   ,extmime->tableRow,extmime->tableCol,current_row);
		// qDebug() << event->possibleActions();
		if (allowed_indices.size() && !allowed_indices.contains(extmime->fxListWidgetItem->linkedFxItem->fxType())) {
			event->ignore();
		}
		if (dragpos.x() > width()/2 || extmime->moveDisabled) {
			extmime->dragObject->setDragCursor(QPixmap(":/gfx/icons/editcopy_32.png"),Qt::MoveAction);
			event->accept();
		} else {
			extmime->dragObject->setDragCursor(QPixmap(),Qt::MoveAction);
			event->accept();
		}
	}
}

Qt::DropActions PTableWidget::supportedDropActions() const
{
	/// @todo this function is never called
	qDebug() << Q_FUNC_INFO << "A wonder has happend. I have not seen this function called before";
	return Qt::MoveAction | Qt::LinkAction | Qt::CopyAction;
}



void PTableWidget::dropEvent(QDropEvent *qevent)
{
	PDropEvent* event = static_cast<PDropEvent*>(qevent);
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	QPoint droppos = event->pos();
	drag_dest_row = indexAt(droppos).row();

	if (extmime && extmime->fxListWidgetItem) {
		// The DragObject is an fxListWidgetItem
		qDebug("PTableWidget::dropEvent: Mime:'%s': from row:%d, col:%d "
			   ,mime->text().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);
		qDebug() << "  Dropaction:" << event->dropAction();

		event->setDropAction(Qt::MoveAction);
		event->accept();
		if (extmime->originPTableWidget != this) {
			qDebug("PTableWidget::dropEvent: Received Row from foreign Widget");
			if (extmime->originPTableWidget) {
				if (event->pos().x() > width()/2  || extmime->moveDisabled) {
					emit rowClonedFrom(extmime->originPTableWidget, extmime->tableRow, drag_dest_row, false);
				} else {
					emit rowClonedFrom(extmime->originPTableWidget, extmime->tableRow, drag_dest_row, true);
				}
			}
		} else {
			qDebug("   Entry moved from row %d to %d",extmime->tableRow, drag_dest_row);
			emit rowMovedFromTo(extmime->tableRow,drag_dest_row);
		}
	} else {
		event->setDropAction(Qt::CopyAction);
		event->accept();
		QList<QUrl> urls = mime->urls();
		int row = drag_dest_row;
		for (int t=0; t<urls.size();t++) {
			if (drag_dest_row >= 0)
				row = drag_dest_row + t;
			emit dropEventReceived(urls.at(t).toString(),row);
		}
	}

	drag_temp_row = -1;
}


void PTableWidget::if_scrolled_vertical(int val)
{
	Q_UNUSED(val);
}

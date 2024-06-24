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

#ifndef PTABLEWIDGET_H
#define PTABLEWIDGET_H

#include <QTableWidget>
#include <QList>
#include <QtWidgets>

#include "tool/toolclasses.h"
#include "tool/qt6_qt5_compat.h"

class PTableWidget : public QTableWidget
{
	Q_OBJECT
private:
	int drag_start_row;
	int drag_dest_row;
	int drag_temp_row;
	int current_vert_scrollpos;
	QList<int>allowed_indices;

public:
	PTableWidget(QWidget *parent = 0);
	void clearDragAndDropAction();
	void clear();
	void saveScrollPos();
	void setOldScrollPos();

	void setDropAllowedIndices(QList<int>list);

protected:
	Qt::DropActions supportedDropActions() const;

private:
	void dragEnterEvent(QDragEnterEvent *qevent) override;
	void dropEvent(QDropEvent *qevent) override;
	void dragLeaveEvent(QDragLeaveEvent *qevent) override;
	void dragMoveEvent(QDragMoveEvent *event) override;

signals:
	void dropEventReceived(QString text, int row);
	void rowMovedFromTo(int srcrow, int destrow);
	void rowClonedFrom(PTableWidget *originPTable, int srcRow, int destRow, bool removeSrc);

public slots:

private slots:
	void if_scrolled_vertical(int val);

};

#endif // PTABLEWIDGET_H

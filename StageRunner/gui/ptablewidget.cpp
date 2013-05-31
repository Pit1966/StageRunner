#include "ptablewidget.h"
#include "customwidget/extmimedata.h"

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QModelIndex>

PTableWidget::PTableWidget(QWidget *parent) :
	QTableWidget(parent)
{
	drag_start_row = -1;
	drag_dest_row = -1;
	drag_temp_row = -1;
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

void PTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);
	QObject * src = event->source();
	QPoint dragpos = event->pos();
	int current_row = indexAt(dragpos).row();

	if (extmime && extmime->fxListWidgetItem) {
		qDebug("Drag enter event Mime:'%s': ObjectName:%s, row:%d, col:%d "
			   ,mime->text().toLocal8Bit().data(),src->objectName().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);

		event->setDropAction(Qt::MoveAction);
		event->accept();
		drag_start_row = extmime->tableRow;
		if (drag_temp_row < 0) {
			if (uint(drag_start_row) < uint(rowCount()))
				removeRow(drag_start_row);
		} else {
			if (uint(drag_temp_row) < uint(rowCount()))
				removeRow(drag_temp_row);
		}
		insertRow(current_row);
		drag_temp_row = current_row;

	} else {
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
}

void PTableWidget::dropEvent(QDropEvent *event)
{
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);

	QPoint droppos = event->pos();
	drag_dest_row = indexAt(droppos).row();

	if (extmime && extmime->fxListWidgetItem) {
		qDebug("Drop event Mime:'%s': from row:%d, col:%d "
			   ,mime->text().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);
		qDebug("   Entry moved from row %d to %d",extmime->tableRow, drag_dest_row);
		event->setDropAction(Qt::MoveAction);
		event->accept();
		emit rowMovedFromTo(extmime->tableRow,drag_dest_row);
	} else {
		event->setDropAction(Qt::CopyAction);
		event->accept();
		emit dropEventReceived(mime->text(),drag_dest_row);
	}

	drag_temp_row = -1;
}

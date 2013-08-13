#include "ptablewidget.h"
#include "customwidget/extmimedata.h"

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QModelIndex>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QScrollBar>
#include <QPoint>

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

void PTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData * mime = event->mimeData();
	const ExtMimeData * extmime = qobject_cast<const ExtMimeData*>(mime);
	QObject * src = event->source();
	QPoint dragpos = event->pos();
	int current_row = indexAt(dragpos).row();

	if (extmime && extmime->fxListWidgetItem) {
		qDebug("PTableWidget::dragEnterEvent: Mime:'%s': ObjectName:%s, row:%d, col:%d "
			   ,mime->text().toLocal8Bit().data(),src->objectName().toLocal8Bit().data(),extmime->tableRow,extmime->tableCol);

		QPoint mouse = event->pos();
		qDebug() << mouse;
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


void PTableWidget::dropEvent(QDropEvent *event)
{
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
				if (event->pos().x() > width()/2) {
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

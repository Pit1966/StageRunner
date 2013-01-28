#include "ptablewidget.h"

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QModelIndex>

PTableWidget::PTableWidget(QWidget *parent) :
	QTableWidget(parent)
{
	drag_start_row = -1;
	drag_dest_row = -1;
}

void PTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData * mime = event->mimeData();
	qDebug("Drag enter event '%s'",mime->text().toLatin1().data());
//	QList<QUrl>urls = mime->urls();
//	for (int t=0; t<urls.size();t++) {
//		qDebug() << "url:" << urls.at(t).toDisplayString();
//	}

	QPoint dragpos = event->pos();

	if (event->source() == this) {
		drag_start_row = indexAt(dragpos).row();
		qDebug ("Drag source = row %d",drag_start_row);

		event->setDropAction(Qt::MoveAction);
		event->accept();
	} else {
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
}

void PTableWidget::dropEvent(QDropEvent *event)
{
	// QTableWidget::dropEvent(event);
	const QMimeData * mime = event->mimeData();
	QPoint droppos = event->pos();

	qDebug("Drop event '%s'",mime->text().toLatin1().data());

	if (event->source() == this) {
		drag_dest_row = indexAt(droppos).row();
		qDebug("Moved from row %d to %d",drag_start_row, drag_dest_row);

		event->setDropAction(Qt::MoveAction);
		event->ignore();
		emit rowMovedFromTo(drag_start_row,drag_dest_row);
	} else {
		event->setDropAction(Qt::CopyAction);
		event->accept();
		emit dropEventReceived(mime->text());
	}
}

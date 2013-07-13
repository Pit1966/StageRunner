#ifndef PTABLEWIDGET_H
#define PTABLEWIDGET_H

#include <QTableWidget>
#ifdef IS_QT5
#include <QtWidgets>
#endif

class PTableWidget : public QTableWidget
{
	Q_OBJECT
private:
	int drag_start_row;
	int drag_dest_row;
	int drag_temp_row;
	int current_vert_scrollpos;

public:
	PTableWidget(QWidget *parent = 0);
	void clearDragAndDropAction();
	void clear();
	void saveScrollPos();
	void setOldScrollPos();

private:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);


signals:
	void dropEventReceived(QString text, int row);
	void rowMovedFromTo(int srcrow, int destrow);

public slots:

private slots:
	void if_scrolled_vertical(int val);

};

#endif // PTABLEWIDGET_H

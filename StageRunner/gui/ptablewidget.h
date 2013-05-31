#ifndef PTABLEWIDGET_H
#define PTABLEWIDGET_H

#include <QTableWidget>

class PTableWidget : public QTableWidget
{
	Q_OBJECT
private:
	int drag_start_row;
	int drag_dest_row;
	int drag_temp_row;

public:
	PTableWidget(QWidget *parent = 0);
	void clearDragAndDropAction();
	void clear();

private:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);


signals:
	void dropEventReceived(QString text, int row);
	void rowMovedFromTo(int srcrow, int destrow);

public slots:

};

#endif // PTABLEWIDGET_H

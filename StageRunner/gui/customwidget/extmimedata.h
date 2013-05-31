#ifndef EXTMIMEDATA_H
#define EXTMIMEDATA_H

#include <QWidget>
#include <QMimeData>
#include <QDrag>

class FxListWidgetItem;

class ExtMimeData : public QMimeData
{
	Q_OBJECT
public:
	FxListWidgetItem *fxListWidgetItem;
	int tableRow;
	int tableCol;

public:
	ExtMimeData();

signals:

public slots:

};


class ExtDrag : public QDrag
{
private:
	QWidget * drag_source;
	bool delete_drag_source_f;
public:
	ExtDrag(QWidget *dragSource, bool deleteDragSource = false);
	~ExtDrag();


};

#endif // EXTMIMEDATA_H

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


#endif // EXTMIMEDATA_H

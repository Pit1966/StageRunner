#ifndef EXTMIMEDATA_H
#define EXTMIMEDATA_H

#include <QWidget>
#include <QMimeData>
#include <QDrag>

class FxListWidget;
class FxListWidgetItem;
class MixerChannel;
class FxList;
class PTableWidget;
class QDrag;

class ExtMimeData : public QMimeData
{
	Q_OBJECT
public:
	FxListWidgetItem *fxListWidgetItem;
	FxListWidget *originFxListWidget;
	FxList *originFxList;
	PTableWidget *originPTableWidget;
	MixerChannel *mixerChannel;
	QDrag *dragObject;
	int tableRow;
	int tableCol;

public:
	ExtMimeData();

signals:

public slots:

};


#endif // EXTMIMEDATA_H

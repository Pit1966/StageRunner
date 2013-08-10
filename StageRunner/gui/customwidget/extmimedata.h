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

class ExtMimeData : public QMimeData
{
	Q_OBJECT
public:
	FxListWidgetItem *fxListWidgetItem;
	FxListWidget *originFxListWidget;
	FxList *originFxList;
	PTableWidget *originPTableWidget;
	MixerChannel *mixerChannel;
	int tableRow;
	int tableCol;

public:
	ExtMimeData();

signals:

public slots:

};


#endif // EXTMIMEDATA_H

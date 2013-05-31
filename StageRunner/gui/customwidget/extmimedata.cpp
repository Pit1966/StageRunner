#include "extmimedata.h"

ExtMimeData::ExtMimeData() :
	QMimeData()
  ,fxListWidgetItem(0)
  ,tableRow(-1)
  ,tableCol(-1)
{
}


ExtDrag::ExtDrag(QWidget *dragSource, bool deleteDragSource)
	: QDrag(dragSource)
	,drag_source(dragSource)
	,delete_drag_source_f(deleteDragSource)
{
}

ExtDrag::~ExtDrag()
{
	if (delete_drag_source_f) {
		drag_source->deleteLater();
		drag_source = 0;
	}
	// qDebug("ExtDrag destroyed");
}

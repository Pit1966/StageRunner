#ifndef TIMELINE_CLASSES_H
#define TIMELINE_CLASSES_H

#include <QString>

namespace PS_TL {

class TimeLineContextMenuEntry
{
public:
	QString menuLabel;
	QString menuId;
	void (*staticCmdFunc)(void);

	TimeLineContextMenuEntry(const QString &label = {}, const QString & id = {})
		: menuLabel(label)
		, menuId(id)
		, staticCmdFunc(nullptr)
	{}
};

}
#endif // TIMELINE_CLASSES_H


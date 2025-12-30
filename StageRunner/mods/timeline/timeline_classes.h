#ifndef TIMELINE_CLASSES_H
#define TIMELINE_CLASSES_H

#include <QString>

namespace PS_TL {

class TimeLineContextMenuEntry
{
public:
	QString menuLabel;
	QString menuId;
	void *paraPointer;
	void (*staticCmdFunc)(void);
	void (*staticCmdFuncPara)(void *paraPtr);

	TimeLineContextMenuEntry(const QString &label = {}, const QString & id = {})
		: menuLabel(label)
		, menuId(id)
		, paraPointer(nullptr)
		, staticCmdFunc(nullptr)
		, staticCmdFuncPara(nullptr)
	{}
};

}
#endif // TIMELINE_CLASSES_H


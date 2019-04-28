#ifndef FXITEMTOOL_H
#define FXITEMTOOL_H

#include <QHash>

class FxItem;
class FxList;
class FxSeqItem;



/**
 * @brief This class implements a mapping from old FxID to new FxID used by copy functions
 */
class FxIdMap : public QHash<qint32,qint32>
{
};

class FxItemTool
{
public:
	FxItemTool();

	static FxItem* cloneFxItem(FxItem *srcItem, bool renameItem = true, int exactClone = 0, FxIdMap *oldNewMap = nullptr);
	static void setClonedFxName(FxItem *srcItem, FxItem *destItem, FxList *fxList = 0);
};

#endif // FXITEMTOOL_H

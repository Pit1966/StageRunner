#ifndef FXITEMTOOL_H
#define FXITEMTOOL_H

class FxItem;
class FxList;
class FxSeqItem;

class FxItemTool
{
public:
	FxItemTool();

	static FxItem* cloneFxItem(FxItem *srcItem, bool renameItem = true, int exactClone = 0);
	static void setClonedFxName(FxItem *srcItem, FxItem *destItem, FxList *fxList = 0);
};

#endif // FXITEMTOOL_H

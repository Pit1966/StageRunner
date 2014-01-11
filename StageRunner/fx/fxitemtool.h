#ifndef FXITEMTOOL_H
#define FXITEMTOOL_H

class FxItem;
class FxList;

class FxItemTool
{
public:
	FxItemTool();

	static FxItem* cloneFxItem(FxItem *srcItem);
	static void setClonedFxName(FxItem *srcItem, FxItem *destItem, FxList *fxList = 0);

};

#endif // FXITEMTOOL_H

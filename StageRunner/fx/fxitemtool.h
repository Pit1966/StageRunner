#ifndef FXITEMTOOL_H
#define FXITEMTOOL_H

class FxItem;

class FxItemTool
{
public:
	FxItemTool();

	static FxItem* cloneFxItem(FxItem *srcItem);

};

#endif // FXITEMTOOL_H

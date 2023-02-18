#ifndef COLORSETTINGS_H
#define COLORSETTINGS_H

#include "../tool/varset.h"

class ColorSettings : public VarSet
{
public:
	pstring pListNumColumn;
	pstring pKeyColumn;

public:
	ColorSettings();
	~ColorSettings();

private:
	void init();
};

#endif // COLORSETTINGS_H

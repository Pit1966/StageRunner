#include "colorsettings.h"

ColorSettings::ColorSettings()
	: VarSet()
{
	init();
	readFromPref();
}

ColorSettings::~ColorSettings()
{
	writeToPref();
}

void ColorSettings::init()
{
	setClass(PrefVarCore::USER_CONFIG,"ColorSettings");
	registerQSetGlobal("Color settings for StageRunner Application");

	addExistingVar(pListNumColumn,"ColorListNumColumn", QStringLiteral("default"));
	addExistingVar(pKeyColumn,"ColorKeyColumn", QStringLiteral("#dcb400"));

}

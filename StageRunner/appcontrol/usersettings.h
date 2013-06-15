#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include "../tool/varset.h"

class UserSettings : public VarSet
{
public:
	pstring pApplicationGuiStyle;
	pstring pDialKnobStyle;
	pstring pLastAudioFxImportPath;
	pstring pLastProjectSavePath;
	pstring pLastProjectLoadPath;
	pint32 pAudioBufferSize;
	pbool pNoInterfaceDmxFeedback;

public:
	UserSettings();
	~UserSettings();

private:
	void init();
};

#endif // USERSETTINGS_H

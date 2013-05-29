#ifndef PLUGINMAPPING_H
#define PLUGINMAPPING_H

#include "varset.h"
#include "varsetlist.h"

class PluginConfig : public VarSet
{
public:
	PluginConfig();
	~PluginConfig();

	pstring pPluginName;
	pstring pLineName;
	pint32 pUniverse;

private:
	void init();
};


class PluginMapping : public VarSet
{
public:
	PluginMapping();
	~PluginMapping();

	pstring pluginConfigName;
	PluginConfig *defaultLineConfig;
	VarSetList<PluginConfig*>pluginLineConfigs;

	bool saveToDefaultFile();
	bool loadFromDefaultFile();

	PluginConfig * getCreatePluginLineConfig(const QString &plugin_name, const QString &line_name);

private:
	void init();
};

#endif // PLUGINMAPPING_H

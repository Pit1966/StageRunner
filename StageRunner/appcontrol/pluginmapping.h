#ifndef PLUGINMAPPING_H
#define PLUGINMAPPING_H

#include "varset.h"
#include "varsetlist.h"

class QLCIOPlugin;

class PluginConfig : public VarSet
{
public:
	pstring pPluginName;
	pstring pLineName;						///< This is the string returned from plugin->inputs() or outputs()
	pbool pIsUsed;
	pint32 pUniverse;
	pint32 pResponseTime;					///< This is the fade delay time for incoming DMX Data when it is forwarded to a scene
	pstring pParameters;					///< Plugin Parameters for this Line

	QLCIOPlugin *plugin;
	int deviceNumber;
	int deviceIoType;

public:
	PluginConfig();
	~PluginConfig();

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

#include "pluginmapping.h"
#include "config.h"
#include <QDir>


PluginConfig::PluginConfig()
{
	init();
}

PluginConfig::~PluginConfig()
{
}

void PluginConfig::init()
{
	setClass(PrefVarCore::PLUGIN_CONFIG,"Plugin config");
	setDescription("This VarSet holds the configuration for an input or output of a plugin");

	addExistingVar(pPluginName,"PluginName");
	addExistingVar(pLineName,"LineName");
	addExistingVar(pUniverse,"Universe");
}


// ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------


PluginMapping::PluginMapping()
{
	init();
}

PluginMapping::~PluginMapping()
{
}

bool PluginMapping::saveToDefaultFile()
{
	QString path = QString("%1/.config/%2/%3.pluginmapping")
			.arg(QDir::homePath()).arg(APP_ORG_STRING).arg(APP_NAME);
	return fileSave(path);
}

bool PluginMapping::loadFromDefaultFile()
{
	QString path = QString("%1/.config/%2/%3.pluginmapping")
			.arg(QDir::homePath()).arg(APP_ORG_STRING).arg(APP_NAME);
	return fileLoad(path);
}

PluginConfig *PluginMapping::getCreatePluginLineConfig(const QString &plugin_name, const QString &line_name)
{
	PluginConfig *lineconfig = 0;
	int t = 0;
	while (lineconfig == 0 && t<pluginLineConfigs.size()) {
		PluginConfig *config = pluginLineConfigs.at(t);
		if (config->pPluginName == plugin_name && config->pLineName == line_name) {
			lineconfig = config;
		}
		t++;
	}
	if (!lineconfig) {
		lineconfig = new PluginConfig;
		lineconfig->pPluginName = plugin_name;
		lineconfig->pLineName = line_name;
		pluginLineConfigs.append(lineconfig);
	}
	return lineconfig;
}

void PluginMapping::init()
{
	setClass(PrefVarCore::PLUGIN_MAPPING,"Plugin mapping");
	setDescription("This VarSet holds the configuration for discovered plugins");

	addExistingVar(pluginConfigName,"PluginConfigName");
	addExistingVarSetList(pluginLineConfigs,"PluginLineConfigs",PrefVarCore::PLUGIN_CONFIG);
}

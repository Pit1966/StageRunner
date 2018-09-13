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
	plugin = 0;
	deviceNumber = 0;
	deviceIoType = 0;

	setClass(PrefVarCore::PLUGIN_CONFIG,"Plugin config");
	setDescription("This VarSet holds the configuration for an input or output of a plugin");

	addExistingVar(pPluginName,"PluginName");
	addExistingVar(pLineName,"LineName");
	addExistingVar(pIsUsed,"LineUsed",false);
	addExistingVar(pUniverse,"Universe");
	addExistingVar(pResponseTime,"ResponseTime");
	addExistingVar(pParameters,"Parameters");
}


// ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------


PluginMapping::PluginMapping()
{
	init();
}

PluginMapping::~PluginMapping()
{
	delete defaultLineConfig;
}

bool PluginMapping::saveToDefaultFile()
{
	QString path = QString("%1/.config/%2/%3.pluginmapping")
			.arg(QDir::homePath()).arg(APP_CONFIG_PATH).arg(APPNAME);

	qDebug() << "save pluginmap" << path;
	return fileSave(path,false,true);
}

bool PluginMapping::loadFromDefaultFile()
{
	QString path = QString("%1/.config/%2/%3.pluginmapping")
			.arg(QDir::homePath()).arg(APP_CONFIG_PATH).arg(APPNAME);
	qDebug() << "save pluginmap" << path;
	bool ok = fileLoad(path);
	return ok;
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
	defaultLineConfig = new PluginConfig;

	setClass(PrefVarCore::PLUGIN_MAPPING,"Plugin mapping");
	setDescription("This VarSet holds the configuration for discovered plugins");

	addExistingVar(pluginConfigName,"PluginConfigName");
	addExistingVar(*defaultLineConfig,"PluginConfigDummy");
	addExistingVarSetList(pluginLineConfigs,"PluginLineConfigs",PrefVarCore::PLUGIN_CONFIG);
}

#include "ioplugincentral.h"
#include "log.h"
#include "qlcioplugin.h"
#include "config.h"
#include "pluginmapping.h"

#include <QDir>
#include <QPluginLoader>

IOPluginCentral::IOPluginCentral(QObject *parent) :
	QObject(parent)
  , pluginMapping(new PluginMapping())
{
}

IOPluginCentral::~IOPluginCentral()
{
	delete pluginMapping;
}

QLCIOPlugin *IOPluginCentral::getQLCPluginByName(const QString &name)
{
	QListIterator <QLCIOPlugin*> it(qlc_plugins);
	while (it.hasNext()) {
		QLCIOPlugin* plugin(it.next());
		if (plugin->name() == name)
			return plugin;
	}

	return 0;
}

void IOPluginCentral::loadQLCPlugins(const QString &dir_str)
{
	if (debug) LOGTEXT(tr("Load QLC plugins from directory '%1'").arg(dir_str));

	QDir dir(dir_str);
	dir.setFilter(QDir::Files);

	if (!dir.exists() || !dir.isReadable()) return;

	// Check all files in the given directory
	QStringListIterator it(dir.entryList());
	while (it.hasNext()) {
		// determine complete Path
		QString path(dir.absoluteFilePath(it.next()));
		QString suf = QFileInfo(path).suffix().toLower();
		if (suf != "so" && suf != "dll") continue;

		// and load plugin
		QPluginLoader load(path, this);
		QObject *obj = load.instance();
		QLCIOPlugin *plugin = qobject_cast<QLCIOPlugin*> (obj);
		if (plugin) {
			// Check if plugin is already loaded
			if (0 == getQLCPluginByName(plugin->name())) {
				LOGTEXT(tr("QLC plugin '%1' loaded")
						.arg(plugin->name()));

				qlc_plugins.append(plugin);

				plugin->init();

			} else {
				DEBUGERROR("'%s' QLC I/O plugin in %s is already loaded -> unload again"
						   ,plugin->name().toLocal8Bit().data(), path.toLocal8Bit().data());
				load.unload();
			}

		} else {
			DEBUGERROR("%s: %s doesn't contain an QLC I/O plugin:"
					   ,Q_FUNC_INFO, path.toLocal8Bit().data(), load.errorString().toLocal8Bit().data());
			load.unload();
		}

	}
}

bool IOPluginCentral::updatePluginMappingInformation()
{
	pluginMapping->pluginConfigName = "Default Plugin Config";

	int count = 0;

	// Discover plugin lines (input/outputs) and update configuration for each
	foreach(QLCIOPlugin *plugin, qlcPlugins()) {
		QString plugin_name = plugin->name();
		QStringList outputs = plugin->outputs();
		QStringList inputs = plugin->inputs();

		for (int t=0; t<outputs.size(); t++) {
			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin_name,outputs.at(t));
			lineconf->plugin = plugin;
			lineconf->deviceNumber = t;
			lineconf->deviceIoType = QLCIOPlugin::Output;
			qDebug() << (++count) << lineconf->pLineName << int(lineconf->pUniverse);
		}
		for (int t=0; t<inputs.size(); t++) {
			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin_name,inputs.at(t));
			lineconf->plugin = plugin;
			lineconf->deviceNumber = t;
			lineconf->deviceIoType = QLCIOPlugin::Input;
			qDebug() << (++count) << lineconf->pLineName << int(lineconf->pUniverse);
		}
	}

	// Clear current fast access tables
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		fastMapInUniverse[t].plugin = 0;
		fastMapOutUniverse[t].plugin = 0;
	}

	// Create fast access tables for universe to plugin mapping
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		int univ = lineconf->pUniverse;

		if (lineconf->deviceIoType == QLCIOPlugin::Output) {
			if (fastMapOutUniverse[univ].plugin) {
				POPUPERRORMSG(tr("Plugin Settings")
							  ,tr("Output universe %1 is assigned to multiple plugin lines -> this is not implemted yet")
						 .arg(univ+1));
			} else {
				fastMapOutUniverse[univ].deviceUniverse = univ;
				fastMapOutUniverse[univ].plugin = lineconf->plugin;
				fastMapOutUniverse[univ].deviceNumber = lineconf->deviceNumber;
				fastMapOutUniverse[univ].responseTime = lineconf->pResponseTime;
			}
		}
		else if (lineconf->deviceIoType == QLCIOPlugin::Input) {
			if (fastMapInUniverse[univ].plugin) {
				POPUPERRORMSG(tr("Plugin Settings")
							  ,tr("Input universe %1 is assigned to multiple plugin lines -> this is not implemented yet")
						 .arg(univ+1));
			} else {
				fastMapInUniverse[univ].deviceUniverse = univ;
				fastMapInUniverse[univ].plugin = lineconf->plugin;
				fastMapInUniverse[univ].deviceNumber = lineconf->deviceNumber;
				fastMapInUniverse[univ].responseTime = lineconf->pResponseTime;
			}
		}

	}

	return true;
}

bool IOPluginCentral::openPlugins()
{
	bool one_opened = false;

	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		int cap = plugin->capabilities();
		QString capstr;
		if (cap & QLCIOPlugin::Output)
			capstr += tr(" Output");
		if (cap & QLCIOPlugin::Input)
			capstr += tr(" Input");

		LOGTEXT(tr("Open Plugin '%1' with capabilities: %2").arg(plugin->name(),capstr));

		QStringList outputs = plugin->outputs();
		for (int o=0; o<outputs.size(); o++) {
			LOGTEXT(tr("   Open Output: %1").arg(outputs.at(o)));
			plugin->openOutput(o);
			one_opened = true;
		}

		QStringList inputs = plugin->inputs();
		for (int o=0; o<inputs.size(); o++) {
			LOGTEXT(tr("   Open Input: %1").arg(inputs.at(o)));
			plugin->openInput(o);
			one_opened = true;
			// Lets connect to inputChanged Signal
			connect(plugin,SIGNAL(valueChanged(quint32,quint32,uchar)),this,SLOT(onInputValueChanged(quint32,quint32,uchar)));
		}

	}
	return one_opened;
}

void IOPluginCentral::closePlugins()
{
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		QStringList outputs = plugin->outputs();
		for (int o=0; o<outputs.size(); o++) {
			LOGTEXT(tr("Close Plugin: %1, Output: %2").arg(plugin->name(),outputs.at(o)));
			plugin->closeOutput(o);
			plugin->disconnect();
		}
	}
}

QString IOPluginCentral::sysPluginDir()
{
	QString dir;
#ifdef WIN32
	dir = "../plugins";
#elif __APPLE__
	dir = QString("%1/../%2").arg(QCoreApplication::applicationDirPath())
								   .arg(PLUGINDIR));
#elif unix
	dir = PLUGINDIR;
#endif

	return dir;
}

bool IOPluginCentral::getPluginAndOutputForDmxUniverse(int universe, QLCIOPlugin *&plugin, int &output)
{
	if (fastMapOutUniverse[universe].plugin) {
		plugin = fastMapOutUniverse[universe].plugin;
		output = fastMapOutUniverse[universe].deviceNumber;
		return true;
	} else {
		plugin = 0;
		output = 0;
		return false;
	}
}

bool IOPluginCentral::getPluginAndInputForDmxUniverse(int universe, QLCIOPlugin *&plugin, int &input)
{
	if (fastMapInUniverse[universe].plugin) {
		plugin = fastMapInUniverse[universe].plugin;
		input = fastMapInUniverse[universe].deviceNumber;
		return true;
	} else {
		plugin = 0;
		input = 0;
		return false;
	}
}

bool IOPluginCentral::getInputUniverseForPlugin(QLCIOPlugin *plugin, int input, int &universe) const
{
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		if (lineconf->plugin == plugin && lineconf->deviceNumber == input && lineconf->deviceIoType == QLCIOPlugin::Input) {
			universe = lineconf->pUniverse;
			return true;
		}
	}
	return false;
}

bool IOPluginCentral::getOutputUniverseForPlugin(QLCIOPlugin *plugin, int output, int &universe) const
{
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		if (lineconf->plugin == plugin && lineconf->deviceNumber == output && lineconf->deviceIoType == QLCIOPlugin::Output) {
			universe = lineconf->pUniverse;
			return true;
		}
	}
	return false;
}

/**
 * @brief Handle "valueChanged" Signal from plugins
 * @param input
 * @param channel
 * @param value
 *
 * This function is the interface from input plugins to stagerunner application
 * The function determines the plugin that has emitted the signal and maps the input number
 * to the configured universe. Than the univeresChannelChanged Signal will be emitted
 *
 */
void IOPluginCentral::onInputValueChanged(quint32 input, quint32 channel, uchar value)
{
	QLCIOPlugin *sendby = qobject_cast<QLCIOPlugin*>(sender());
	if(sendby) {
		int universe;
		if (getInputUniverseForPlugin(sendby,input,universe)) {
			emit universeValueChanged(universe,channel,value);
			if (debug > 4) DEBUGTEXT("%s: Value changed in universe %d -> ch:%d=%d",universe,channel,value);
		}
	}
}

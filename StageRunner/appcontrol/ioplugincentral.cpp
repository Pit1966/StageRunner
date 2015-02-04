#include "ioplugincentral.h"
#include "log.h"
#include "qlcioplugin.h"
#include "config.h"
#include "pluginmapping.h"
#include "messagedialog.h"

#include <QDir>
#include <QPluginLoader>
#include <QMetaObject>

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
				connect(plugin,SIGNAL(configurationChanged()),this,SLOT(onPluginConfigurationChanged()));
				if (obj->metaObject()->indexOfSignal("errorMsgEmitted(QString)") >= 0) {
					connect(plugin,SIGNAL(errorMsgEmitted(QString)),this,SLOT(onErrorMessageReceived(QString)));
				}

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

void IOPluginCentral::unloadPlugins()
{
	QObjectList plugins = QPluginLoader::staticInstances();

	for (int t=0; t<plugins.size(); t++) {
		delete plugins.at(t);
	}
	qlc_plugins.clear();
}

bool IOPluginCentral::updatePluginMappingInformation()
{
	pluginMapping->pluginConfigName = "Default Plugin Config";

	allInputNames.clear();
	allOutputNames.clear();

	int count = 0;

	// Discover plugin lines (input/outputs) and update configuration for each
	foreach(QLCIOPlugin *plugin, qlcPlugins()) {
		QString plugin_name = plugin->name();
		QStringList outputs = plugin->outputs();
		QStringList inputs = plugin->inputs();

		for (int t=0; t<outputs.size(); t++) {
			// Check if line is valid
			if (outputs.at(t).endsWith("!!!"))
				continue;
			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin_name,outputs.at(t));
			lineconf->plugin = plugin;
			lineconf->deviceNumber = t;
			lineconf->deviceIoType = QLCIOPlugin::Output;
			if (debug > 1) qDebug() << (++count) << lineconf->pLineName << int(lineconf->pUniverse);

			allOutputNames.append(outputs.at(t));

		}
		for (int t=0; t<inputs.size(); t++) {
			if (inputs.at(t).endsWith("!!!"))
				continue;
			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin_name,inputs.at(t));
			lineconf->plugin = plugin;
			lineconf->deviceNumber = t;
			lineconf->deviceIoType = QLCIOPlugin::Input;
			if (debug > 1) qDebug() << (++count) << lineconf->pLineName << int(lineconf->pUniverse);

			allInputNames.append(inputs.at(t));

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

		if (!lineconf->pIsUsed || univ < 0 || univ >= MAX_DMX_UNIVERSE)
			continue;

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
	/// @todo: implement Plugin Enable/Disable Flag in Config
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
			int universe;
			getOutputUniverseForPlugin(plugin,o,universe);
			if (universe < 0) {
				LOGTEXT(tr("   Output %1 not used").arg(outputs.at(o)));
			} else {
				LOGTEXT(tr("   <font color=darkgreen>Open Output: %1</font>").arg(outputs.at(o)));
				plugin->openOutput(o);
				one_opened = true;
			}
		}

		QStringList inputs = plugin->inputs();
		for (int i=0; i<inputs.size(); i++) {
			int universe;
			getInputUniverseForPlugin(plugin,i,universe);
			if (universe < 0) {
				LOGTEXT(tr("   Input %1 not used").arg(inputs.at(i)));
			} else {
				LOGTEXT(tr("   <font color=darkgreen>Open Input: %1</font>").arg(inputs.at(i)));
				plugin->openInput(i);
				one_opened = true;
			}
			// Lets connect to inputChanged Signal
			connect(plugin,SIGNAL(valueChanged(quint32,quint32,uchar)),this,SLOT(onInputValueChanged(quint32,quint32,uchar)),Qt::UniqueConnection);
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
		QStringList inputs = plugin->inputs();
		for (int i=0; i<inputs.size(); i++) {
			LOGTEXT(tr("Close Plugin: %1, Input: %2").arg(plugin->name(),inputs.at(i)));
			plugin->closeInput(i);
			plugin->disconnect();
		}
	}
}

QStringList IOPluginCentral::getAllAvailableInputNames() const
{
	QStringList input_names;
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		input_names += plugin->inputs();
	}
	return input_names;
}

QStringList IOPluginCentral::getAllAvailableOutputNames() const
{
	QStringList output_names;
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		output_names += plugin->outputs();
	}
	return output_names;
}

QString IOPluginCentral::sysPluginDir()
{
	QString dir;
#ifdef WIN32
	dir = "../plugins";
#elif __APPLE__
	dir = QString("%1/../%2").arg(QCoreApplication::applicationDirPath())
								   .arg(PLUGINDIR);
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

/**
 * @brief Finds the Input Universe number for an input number of a plugin
 * @param plugin Pointer to QLCIOPlugin instance to search in
 * @param input The desired input number
 * @param universe This is a return by reference parameter. The variable will get the universe number or -1 if not enabled
 * @return false, if input was not found;
 */
bool IOPluginCentral::getInputUniverseForPlugin(QLCIOPlugin *plugin, int input, int &universe) const
{
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		if (lineconf->plugin == plugin && lineconf->deviceNumber == input && lineconf->deviceIoType == QLCIOPlugin::Input) {
			if (lineconf->pIsUsed) {
				universe = lineconf->pUniverse;
			} else {
				universe = -1;
			}
			return true;
		}
	}
	universe = -1;
	return false;
}

/**
* @brief Finds the output Universe number for an input number of a plugin
* @param plugin Pointer to QLCIOPlugin instance to search in
* @param output The desired output number
* @param universe This is a return by reference parameter. The variable will get the universe number or -1 if not enabled
* @return false, if output was not found;
*/
bool IOPluginCentral::getOutputUniverseForPlugin(QLCIOPlugin *plugin, int output, int &universe) const
{
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		if (lineconf->plugin == plugin && lineconf->deviceNumber == output && lineconf->deviceIoType == QLCIOPlugin::Output) {
			if (lineconf->pIsUsed) {
				universe = lineconf->pUniverse;
			} else {
				universe = -1;
			}
			return true;
		}
	}
	universe = -1;
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
			if (universe < 0) {
				if (debug > 4) DEBUGTEXT("Input universe event from plugin ignored: Universe: %d -> ch:%d=%d (Line is not configured)",universe+1,channel,value);
			} else {
				emit universeValueChanged(universe,channel,value);
				if (debug > 4) DEBUGTEXT("Input event in uiverse %d -> ch:%d=%d",universe+1,channel,value);
			}
		}
	}
}

void IOPluginCentral::onPluginConfigurationChanged()
{
	QByteArray pluginname = reinterpret_cast<QLCIOPlugin*>(sender())->name().toLocal8Bit();

	qDebug("Plugin Configuration Changed: In plugin: %s",pluginname.data());

	qDebug() << "Input plugin:" << getAllAvailableInputNames() << "allInputNames" << allInputNames;
	qDebug() << "Output plugin:" << getAllAvailableOutputNames() << "allOutputNames" << allOutputNames;

	QStringList vanished_inouts;
	QStringList added_inouts;

	QStringList current_inputs = getAllAvailableInputNames();
	foreach(QString input, allInputNames) {
		if (!current_inputs.contains(input)) {
			vanished_inouts += input;
			DEBUGERROR("Plugin %s: Lost input %s",pluginname.data(),input.toLocal8Bit().data());
		}
	}
	foreach (QString input, current_inputs) {
		if (input.endsWith("!!!")) {
			DEBUGERROR("Plugin %s: Input %s is offline",pluginname.data(),input.toLocal8Bit().data());
		}
		else if (!allInputNames.contains(input)) {
			added_inouts += input;
			DEBUGTEXT("Plugin %s: Input %s appeared",pluginname.data(),input.toLocal8Bit().data());
		}
	}

	QStringList current_outputs = getAllAvailableOutputNames();
	foreach(QString output, allOutputNames) {
		if (!current_outputs.contains(output)) {
			vanished_inouts += output;
			DEBUGERROR("Plugin %s: Lost output %s",pluginname.data(),output.toLocal8Bit().data());
		}
	}
	foreach (QString output, current_outputs) {
		if (output.endsWith("!!!")) {
			DEBUGERROR("Plugin %s: Output %s is offline",pluginname.data(),output.toLocal8Bit().data());
		}
		else if (!allOutputNames.contains(output)) {
			added_inouts += output;
			DEBUGTEXT("Plugin %s: Output %s appeared",pluginname.data(),output.toLocal8Bit().data());
		}
	}

	if (vanished_inouts.size()) {
		MessageDialog *dialog = new MessageDialog;
		QString maintext = tr("One or more Input or Output line(s) has vanished!\n");
		QString subtext;
		for (int t=0; t<vanished_inouts.size(); t++) {
			subtext += QString("Line: %1\n").arg(vanished_inouts.at(t));
		}
		dialog->connectSpecialFunction(this,"reOpenPlugins");
		dialog->showMessage(maintext,subtext);
	}

	if (added_inouts.size()) {
		openPlugins();
//		MessageDialog *dialog = new MessageDialog;
//		QString maintext = tr("One or more Input or Output line(s) has appeared!\n");
//		QString subtext;
//		for (int t=0; t<added_inouts.size(); t++) {
//			subtext += QString("Line: %1\n").arg(added_inouts.at(t));
//		}
//		dialog->connectSpecialFunction(this,"reOpenPlugins");
//		dialog->showMessage(maintext,subtext);
	}

	updatePluginMappingInformation();
}

void IOPluginCentral::onErrorMessageReceived(QString msg)
{
	LOGERROR(tr("Plugin: %1").arg(msg));
}

void IOPluginCentral::reOpenPlugins()
{
	DEBUGTEXT("Reopen plugins");

	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		plugin->init();
	}

	openPlugins();
}

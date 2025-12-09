//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "ioplugincentral.h"
#include "log.h"
#include "qlcioplugin.h"
#include "config.h"
#include "configrev.h"
#include "pluginmapping.h"
#include "messagedialog.h"
#include "variantmapserializer.h"

#include <QDir>
#include <QPluginLoader>
#include <QMetaObject>
#include <QSettings>

IOPluginCentral::IOPluginCentral(QObject *parent)
	: QObject(parent)
	, pluginMapping(new PluginMapping())
{
}

IOPluginCentral::~IOPluginCentral()
{
	if (!qlc_plugins.isEmpty())
		unloadPlugins();
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

	return nullptr;
}

int IOPluginCentral::loadQLCPlugins(const QString &dir_str)
{
	// Read enable / disable status of plugin from Qt config
	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("Plugins");

	int loadcnt = 0;

	QString path = dir_str;
	if (path.startsWith("~"))
		path = QDir::homePath() + path.mid(1);

	if (debug)
		LOGTEXT(tr("Load QLC plugins from directory '%1'").arg(path));

	QDir dir(path);
	dir.setFilter(QDir::Files);

	if (!dir.exists() || !dir.isReadable())
		return 0;

	// Check all files in the given directory
	QStringListIterator it(dir.entryList());
	while (it.hasNext()) {
		// determine complete Path
		QString path(dir.absoluteFilePath(it.next()));
		QString suf = QFileInfo(path).suffix().toLower();
		if (suf != "so" && suf != "dll" && suf != "dylib")
			continue;

		// and load plugin
		QPluginLoader load(path, this);
		QObject *obj = load.instance();
		QLCIOPlugin *plugin = qobject_cast<QLCIOPlugin*> (obj);
		if (plugin) {
			// check if plugin is disabled
			if (set.value(plugin->name(), true).toBool() == false) {
				LOGTEXT(tr("<font color=info>QLC plugin '%1' is disabled</font>").arg(plugin->name()));
				qlc_plugins.append(plugin);
				plugin->setProperty("isDisabled", true);
				loadcnt++;
			}
			// Check if plugin is already loaded
			else if (nullptr == getQLCPluginByName(plugin->name())) {
				LOGTEXT(tr("QLC plugin '%1' loaded").arg(plugin->name()));
				qlc_plugins.append(plugin);

				if (obj->metaObject()->indexOfSignal("errorMsgEmitted(QString)") >= 0) {
					connect(plugin,SIGNAL(errorMsgEmitted(QString)),this,SLOT(onErrorMessageReceived(QString)));
				}
				if (obj->metaObject()->indexOfSignal("statusMsgEmitted(QString)") >= 0) {
					connect(plugin,SIGNAL(statusMsgEmitted(QString)),this,SLOT(onStatusMessageReceived(QString)));
				}

				plugin->init();
				loadcnt++;

				connect(plugin,SIGNAL(configurationChanged()),this,SLOT(onPluginConfigurationChanged()));
			}
			else {
				DEBUGERROR("'%s' QLC I/O plugin in %s is already loaded -> unload again"
						   , plugin->name().toLocal8Bit().constData(), path.toLocal8Bit().constData());
				load.unload();
			}

		} else {
			DEBUGERROR("%s: %s doesn't contain an QLC I/O plugin:"
					   , Q_FUNC_INFO, path.toLocal8Bit().data(), load.errorString().toLocal8Bit().data());
			load.unload();
		}

	}

	return loadcnt;
}

void IOPluginCentral::unloadPlugins()
{
	//	QObjectList plugins = QPluginLoader::staticInstances();

	//	for (int t=0; t<plugins.size(); t++) {
	//		delete plugins.at(t);
	//	}
	//  qlc_plugins.clear();

	if (debug)
		qDebug() << "unload plugins";

	while (!qlc_plugins.isEmpty())
		delete qlc_plugins.takeFirst();
}

bool IOPluginCentral::updatePluginMappingInformation()
{
	pluginMapping->pluginConfigName = "Default Plugin Config";

	allInputNames.clear();
	allOutputNames.clear();

	int count = 0;

	// Discover plugin lines (input/outputs) and update configuration for each
	for (QLCIOPlugin *plugin : qlcPlugins()) {
		// skip, if plugin is disabled and skip
		if (plugin->property("isDisabled").toBool())
			continue;

		QString plugin_name = plugin->name();
		QStringList outputs = IOPluginCentral::outputsOf(plugin);
		QStringList inputs = IOPluginCentral::inputsOf(plugin);

		for (int t=0; t<outputs.size(); t++) {
			// Check if line is valid
			if (outputs.at(t).endsWith("!!!"))
				continue;
			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin_name,outputs.at(t));
			lineconf->plugin = plugin;
			lineconf->deviceNumber = t;
			lineconf->deviceIoType = QLCIOPlugin::Output;
			// lineconf->pIsUsed = true;
			if (debug > 1)
				qDebug() << (++count) << lineconf->pLineName << int(lineconf->pUniverse);

			allOutputNames.append(outputs.at(t));

			// set parameters in plugin
			// Möglicherweise ist zu diesem Zeitpunkt das Plugin noch nicht initialisiert
			// Beispiel: ArtNet hat noch keinen Controller, wenn setParameter(..) direkt nach dem Laden aufgerufen wird
			setPluginParametersFromLineConf(plugin, lineconf);
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

			// set parameters in plugin
			setPluginParametersFromLineConf(plugin, lineconf);
		}
	}

	// Clear current fast access tables
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		fastMapInUniverse[t].plugin = nullptr;
		mapOutUniverse[t].clear();
	}

	// Create fast access tables for universe to plugin mapping
	for (int t=0; t<pluginMapping->pluginLineConfigs.size(); t++) {
		PluginConfig *lineconf = pluginMapping->pluginLineConfigs.at(t);
		int univ = lineconf->pUniverse-1;

		if (!lineconf->pIsUsed || univ < 0 || univ >= MAX_DMX_UNIVERSE)
			continue;

		if (lineconf->deviceIoType == QLCIOPlugin::Output) {
			PluginEntry uniEntry;
			uniEntry.deviceUniverse = univ;
			uniEntry.plugin = lineconf->plugin;
			uniEntry.deviceNumber = lineconf->deviceNumber;
			uniEntry.responseTime = lineconf->pResponseTime;
			mapOutUniverse[univ].append(uniEntry);
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
		if (plugin->property("isDisabled").toBool() == true) {
			LOGTEXT(tr("Plugin '%1' not opened, since it is disabled").arg(plugin->name()));
			continue;
		}
		int cap = plugin->capabilities();
		QString capstr;
		if (cap & QLCIOPlugin::Output)
			capstr += tr(" Output");
		if (cap & QLCIOPlugin::Input)
			capstr += tr(" Input");

		LOGTEXT(tr("Open Plugin '%1' with capabilities: %2").arg(plugin->name(),capstr));

		QStringList outputs = outputsOf(plugin);
		for (int o=0; o<outputs.size(); o++) {
			int universe;
			getOutputUniverseForPlugin(plugin,o,universe);
			if (universe < 0) {
				LOGTEXT(tr("   Output %1 not used").arg(outputs.at(o)));
			} else {
				LOGTEXT(tr("   <font color=darkgreen>Open Output: %1</font>").arg(outputs.at(o)));
				plugin->openOutput(uint(o), uint(universe));
				one_opened = true;
			}

			PluginConfig *lineconf = pluginMapping->getCreatePluginLineConfig(plugin->name(),outputs.at(o));
			setPluginParametersFromLineConf(plugin, lineconf);

		}

		QStringList inputs = inputsOf(plugin);
		for (int i=0; i<inputs.size(); i++) {
			int universe;
			getInputUniverseForPlugin(plugin,i,universe);
			if (universe < 0) {
				LOGTEXT(tr("   Input %1 not used").arg(inputs.at(i)));
			} else {
				LOGTEXT(tr("   <font color=darkgreen>Open Input: %1</font>").arg(inputs.at(i)));
				plugin->openInput(uint(i),uint(universe));
				one_opened = true;
			}
			// Lets connect to inputChanged Signal
			connect(plugin,SIGNAL(valueChanged(quint32,quint32,quint32,uchar,QString))
					,this,SLOT(onInputValueChanged(quint32,quint32,quint32,uchar,QString)),Qt::ConnectionType(Qt::UniqueConnection|Qt::DirectConnection));
		}
	}
	return one_opened;
}

void IOPluginCentral::closePlugins()
{
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		if (plugin->property("isDisabled").toBool())
			continue;

		QStringList outputs = outputsOf(plugin);
		for (int o=0; o<outputs.size(); o++) {
			LOGTEXT(tr("Close Plugin: %1, Output: %2").arg(plugin->name(),outputs.at(o)));
			plugin->closeOutput(uint(o),0);
			plugin->disconnect();
		}
		QStringList inputs = inputsOf(plugin);
		for (int i=0; i<inputs.size(); i++) {
			LOGTEXT(tr("Close Plugin: %1, Input: %2").arg(plugin->name(),inputs.at(i)));
			plugin->closeInput(uint(i),0);
			plugin->disconnect();
		}
	}
}

QStringList IOPluginCentral::getAllAvailableInputNames() const
{
	QStringList input_names;
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		input_names += inputsOf(plugin);
	}
	return input_names;
}

QStringList IOPluginCentral::getAllAvailableOutputNames() const
{
	QStringList output_names;
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		output_names += outputsOf(plugin);
	}
	return output_names;
}

QString IOPluginCentral::sysPluginDir()
{
	QString dir;
#ifdef WIN32
	dir = "../plugins";
#elif __APPLE__
	//	dir = QString("%1/../%2").arg(QCoreApplication::applicationDirPath())
	//			.arg("PlugIns/stagerunner");
	dir = QString("%1/Contents/%2")
			.arg("~/StageRunner.app")
			.arg(PLUGINDIR);
#elif __unix__
	dir = PLUGINDIR;
#endif

	return dir;
}

bool IOPluginCentral::hasOutputs(int universe) const
{
	if (universe < 0 || universe >= MAX_DMX_UNIVERSE)
		return false;

	if (mapOutUniverse[universe].size() > 0)
		return true;

	return false;
}

/**
 * @brief Get pointer to plugin and output number for a given universe and output index.
 * @param universe
 * @param connectNumber
 * @param plugin
 * @param output
 * @return true, if found.
 *
 * since Aug. 2019 more than one single output can be connected to a universe. Therefor you have to call this function
 * with increasing connectNumber (beginning with 0) until it returns false
 */
bool IOPluginCentral::getPluginAndOutputForDmxUniverse(int universe, int connectNumber, QLCIOPlugin *&plugin, int &output)
{
	if (connectNumber >= 0 && connectNumber < mapOutUniverse[universe].size()) {
		plugin = mapOutUniverse[universe].at(connectNumber).plugin;
		output = mapOutUniverse[universe].at(connectNumber).deviceNumber;
		return true;
	} else {
		plugin = nullptr;
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
		plugin = nullptr;
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
				universe = lineconf->pUniverse-1;
				if (universe < 0)
					lineconf->pIsUsed = false;
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
				universe = lineconf->pUniverse-1;
				if (universe < 0)
					lineconf->pIsUsed = false;
			} else {
				universe = -1;
			}
			return true;
		}
	}
	universe = -1;
	return false;
}

QLCIOPlugin *IOPluginCentral::yadiPlugin()
{
	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		if (plugin->name().startsWith("YADI"))
			return plugin;
	}
	return 0;
}

QStringList IOPluginCentral::outputsOf(QLCIOPlugin *plugin)
{
	QStringList outnames = plugin->outputs();
	for (QString &name : outnames) {
		if (!name.startsWith("TX:"))
			name.prepend("TX:");
	}
	return  outnames;
}

QString IOPluginCentral::outputOf(int line, QLCIOPlugin *plugin)
{
	QStringList outputs = outputsOf(plugin);
	if (line < 0 || line >= outputs.size())
		return QString();

	return outputs.at(line);
}

QStringList IOPluginCentral::inputsOf(QLCIOPlugin *plugin)
{
	QStringList innames = plugin->inputs();
	for (QString &name : innames) {
		if (!name.startsWith("RX:"))
			name.prepend("RX:");
	}
	return innames;
}

QString IOPluginCentral::inputOf(int line, QLCIOPlugin *plugin)
{
	QStringList inputs = inputsOf(plugin);
	if (line < 0 || line >= inputs.size())
		return QString();

	return inputs.at(line);
}


/**
 * @brief Set plugin parameters from given PluginConfig object
 * @param plugin Pointer to QLCIOPLugin
 * @param lineConf
 * @return true if at least one parameter was set
 *
 * Actually this wraps calls to QLCIOPlugin::setParameter( ...... )
 */
bool IOPluginCentral::setPluginParametersFromLineConf(QLCIOPlugin *plugin, PluginConfig *lineConf)
{
	QString paras = lineConf->pParameters;
	if (!paras.isEmpty()) {
		QVariantMap paramap = VariantMapSerializer::toMap(paras);
		for (const QString &key : paramap.keys()) {
			if (lineConf->pUniverse < 1)
				continue;
			plugin->setParameter(uint(lineConf->pUniverse-1)
								 , uint(lineConf->deviceNumber)
								 , QLCIOPlugin::Capability(lineConf->deviceIoType)
								 , key
								 , paramap.value(key));
		}
		return true;
	}
	return false;
}

/**
 * @brief Handle "valueChanged" Signal from plugins
 * @param universe  used universe or maybe UNIT_MAX, if not providec by plugin
 * @param input		LineID for input
 * @param channel
 * @param value
 *
 * This function is the interface from input plugins to stagerunner application
 * The function determines the plugin that has emitted the signal and maps the input number
 * to the configured universe. Than the univeresChannelChanged Signal will be emitted
 *
 *
 */
void IOPluginCentral::onInputValueChanged(quint32 universe, quint32 input, quint32 channel, uchar value, const QString &key)
{
	Q_UNUSED(key)
	if (universe > 10) {
		qDebug() << "Input universe event received. Universe" << universe << "input" << input
				 << "channel" << channel << value << "sender" << sender();

		QLCIOPlugin *sendby = qobject_cast<QLCIOPlugin*>(sender());
		if(sendby) {
			int i_universe;
			if (getInputUniverseForPlugin(sendby,int(input),i_universe)) {
				if (i_universe < 0) {
					if (debug > 4)
						DEBUGTEXT("Input universe event from plugin ignored: Universe: %d -> ch:%d=%d (Line is not configured)",universe+1,channel,value);
				} else {
					universe = i_universe;
					if (debug > 4)
						DEBUGTEXT("Input event in uiverse %d -> ch:%d=%d",i_universe+1,channel,value);
				}
			}
		}
	}

	if (universe < MAX_DMX_UNIVERSE) {
		emit universeValueChanged(universe,channel,value);
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

void IOPluginCentral::onErrorMessageReceived(const QString &msg)
{
	LOGERROR(tr("Plugin: %1").arg(msg));
}

void IOPluginCentral::onStatusMessageReceived(const QString &msg)
{
	LOGTEXT(tr("Plugin: %1").arg(msg));
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

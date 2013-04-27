#include "ioplugincentral.h"
#include "log.h"
#include "qlcioplugin.h"
#include "config.h"

#include <QDir>
#include <QPluginLoader>

IOPluginCentral::IOPluginCentral(QObject *parent) :
	QObject(parent)
{
}

IOPluginCentral::~IOPluginCentral()
{
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
		// and load plugin
		QPluginLoader load(path, this);
		QLCIOPlugin *plugin = qobject_cast<QLCIOPlugin*> (load.instance());
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

bool IOPluginCentral::openPlugins()
{
	bool one_opened = false;

	for (int t=0; t<qlc_plugins.size(); t++) {
		QLCIOPlugin *plugin = qlc_plugins.at(t);
		QStringList outputs = plugin->outputs();
		for (int o=0; o<outputs.size(); o++) {
			LOGTEXT(tr("Open Plugin: %1, Output: %2").arg(plugin->name(),outputs.at(o)));
			plugin->openOutput(o);
			one_opened = true;
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
		}
	}
}

QString IOPluginCentral::sysPluginDir()
{
	QString dir;

#ifdef __APPLE__
	dir = QString("%1/../%2").arg(QCoreApplication::applicationDirPath())
								   .arg(PLUGINDIR));
#else
	dir = PLUGINDIR;
#endif

	return dir;
}
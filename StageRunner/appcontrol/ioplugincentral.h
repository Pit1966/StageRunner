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

#ifndef IOPLUGINCENTRAL_H
#define IOPLUGINCENTRAL_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

#include "config.h"

class QLCIOPlugin;
class PluginMapping;
class PluginConfig;

class IOPluginCentral : public QObject
{
	Q_OBJECT
public:

	class PluginEntry {
	public:
		QLCIOPlugin * plugin;						///< Pointer to loaded Plugin
		int deviceNumber;							///< The output/input number from plugin for the universe
		int deviceUniverse;
		int responseTime;
		int dummy;
	public:
		PluginEntry()
			: plugin(nullptr)
			,deviceNumber(0)
			,deviceUniverse(0)
			,responseTime(0)
			,dummy(0)
		{
		}
	};

	PluginMapping *pluginMapping;						///< This VarSet holds the configuration of plugins and DMX universe mapping
	PluginEntry fastMapOutUniverse[MAX_DMX_UNIVERSE];	///< Array that connects output universe Number to configured Plugin and Output
	PluginEntry fastMapInUniverse[MAX_DMX_UNIVERSE];	///< Array that connects input universe Number to configured Plugin and Output

private:
	QList<QLCIOPlugin*>qlc_plugins;						///< A list of loaded QLCIOPlugin type plugins
	QStringList allInputNames;							///< A list witch contains all available inputs in every plugin
	QStringList allOutputNames;							///< A list witch contains all available outputs in every plugin

public:
	explicit IOPluginCentral(QObject *parent = nullptr);
	~IOPluginCentral();

	QLCIOPlugin * getQLCPluginByName(const QString & name);
	int loadQLCPlugins(const QString & dir_str);
	void unloadPlugins();
	bool updatePluginMappingInformation();
	void closePlugins();
	QStringList getAllAvailableInputNames() const;
	QStringList getAllAvailableOutputNames() const;
	const QList<QLCIOPlugin*> & qlcPlugins() const {return qlc_plugins;}
	/**
	 * @brief Determine system default plugin directory path
	 *
	 */
	static QString sysPluginDir();

	bool getPluginAndOutputForDmxUniverse(int universe, QLCIOPlugin *&plugin, int & output);
	bool getPluginAndInputForDmxUniverse(int universe, QLCIOPlugin *&plugin, int & input);
	bool getInputUniverseForPlugin(QLCIOPlugin *plugin, int input, int &universe) const;
	bool getOutputUniverseForPlugin(QLCIOPlugin *plugin, int output, int &universe) const;

	// Yadi specific
	QLCIOPlugin * yadiPlugin();


	static QStringList outputsOf(QLCIOPlugin *plugin);
	static QString outputOf(int line, QLCIOPlugin *plugin);
	static QStringList inputsOf(QLCIOPlugin *plugin);
	static QString inputOf(int line, QLCIOPlugin *plugin);

	static bool setPluginParametersFromLineConf(QLCIOPlugin *plugin, PluginConfig *lineConf);


signals:
	void universeValueChanged(quint32 universe, quint32 channel, uchar value);

public slots:
	bool openPlugins();
	void reOpenPlugins();

private slots:
	void onInputValueChanged(quint32 universe, quint32 input, quint32 channel, uchar value, const QString &key = nullptr);
	void onPluginConfigurationChanged();
	void onErrorMessageReceived(const QString &msg);
	void onStatusMessageReceived(const QString &msg);

public slots:




};

#endif // IOPLUGINCENTRAL_H

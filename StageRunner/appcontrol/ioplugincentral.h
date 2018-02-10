#ifndef IOPLUGINCENTRAL_H
#define IOPLUGINCENTRAL_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

#include "config.h"

class QLCIOPlugin;
class PluginMapping;

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
	public:
		PluginEntry()
			: plugin(0)
			,deviceNumber(0)
			,deviceUniverse(0)
			,responseTime(0)
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
	explicit IOPluginCentral(QObject *parent = 0);
	~IOPluginCentral();

	QLCIOPlugin * getQLCPluginByName(const QString & name);
	void loadQLCPlugins(const QString & dir_str);
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


signals:
	void universeValueChanged(quint32 universe, quint32 channel, uchar value);

public slots:
	bool openPlugins();
	void reOpenPlugins();

private slots:
    void onInputValueChanged(quint32 universe, quint32 input, quint32 channel, uchar value, const QString &key = 0);
	void onPluginConfigurationChanged();
	void onErrorMessageReceived(QString msg);

public slots:




};

#endif // IOPLUGINCENTRAL_H

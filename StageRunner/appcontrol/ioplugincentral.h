#ifndef IOPLUGINCENTRAL_H
#define IOPLUGINCENTRAL_H

#include <QObject>
#include <QList>
#include <QString>

class QLCIOPlugin;
class PluginMapping;

class IOPluginCentral : public QObject
{
	Q_OBJECT
public:
	PluginMapping *pluginMapping;					///< This VarSet holds the configuration of plugins and DMX universe mapping

private:
	QList<QLCIOPlugin*>qlc_plugins;					///< A list of loaded QLCIOPlugin type plugins

public:
	explicit IOPluginCentral(QObject *parent = 0);
	~IOPluginCentral();

	QLCIOPlugin * getQLCPluginByName(const QString & name);
	void loadQLCPlugins(const QString & dir_str);
	bool updatePluginMappingInformation();
	bool openPlugins();
	void closePlugins();
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

private slots:
	void onInputValueChanged(quint32 input, quint32 channel, uchar value);

public slots:




};

#endif // IOPLUGINCENTRAL_H

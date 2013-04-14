#ifndef IOPLUGINCENTRAL_H
#define IOPLUGINCENTRAL_H

#include <QObject>
#include <QList>
#include <QString>

class QLCIOPlugin;

class IOPluginCentral : public QObject
{
	Q_OBJECT

private:
	QList<QLCIOPlugin*>qlc_plugins;					///< A list of loaded QLCIOPlugin type plugins

public:
	explicit IOPluginCentral(QObject *parent = 0);
	~IOPluginCentral();

	QLCIOPlugin * getQLCPluginByName(const QString & name);
	void loadQLCPlugins(const QString & dir_str);
	/**
	 * @brief Determine system default plugin directory path
	 *
	 */
	static QString sysPluginDir();


signals:

public slots:

};

#endif // IOPLUGINCENTRAL_H

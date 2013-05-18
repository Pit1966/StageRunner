#include <QApplication>

#include "config.h"
#include "appcontrol/appcentral.h"
#include "gui/stagerunnermainwin.h"
#include "system/log.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setApplicationName(APP_NAME);
	app.setOrganizationName(APP_ORG_STRING);

	logThread = new Log;
	logThread->initLog(0);

	AppCentral *myapp = AppCentral::instance();
	StageRunnerMainWin *mywin = new StageRunnerMainWin(myapp);

	logThread->initLog(mywin->logWidget);

	// Init GUI
	mywin->show();
	mywin->initConnects();
	mywin->initAppDefaults();



	// Load Plugins
	myapp->loadPlugins();
	myapp->openPlugins();

	// Start the world :-)
	myapp->setLightLoopEnabled(true);

	// and run
	int ret =  app.exec();

	// Stop the world
	myapp->setLightLoopEnabled(false);

	// Clean up plugins
	myapp->closePlugins();

	logThread->stopLog();
	delete mywin;

	AppCentral::destroyInstance();
	delete logThread;

	return ret;
}

#include <QApplication>

#include "config.h"
#include "appcentral.h"
#include "stagerunnermainwin.h"
#include "usersettings.h"
#include "log.h"
#include "scapplication.h"

// #include "ioplugincentral.h"

int main(int argc, char *argv[])
{
	PrefVarCore::registerVarClasses();

	ScApplication app(argc, argv);

	app.setApplicationName(APP_NAME);
	app.setOrganizationName(APP_ORG_STRING);

	logThread = new Log;
	logThread->initLog(0);

	AppCentral *myapp = AppCentral::instance();
	StageRunnerMainWin *mywin = new StageRunnerMainWin(myapp);

	logThread->initLog(mywin->logWidget);

	// Init GUI
	mywin->setApplicationGuiStyle(myapp->userSettings->pApplicationGuiStyle);
	mywin->updateButtonStyles(myapp->userSettings->pDialKnobStyle);
	mywin->show();
	mywin->initConnects();
	// This also loads the last project
	mywin->initAppDefaults();

	// Load Plugins
	myapp->loadPlugins();
	myapp->openPlugins();

	// Start the world :-)
	myapp->setLightLoopEnabled(true);
	myapp->setFxExecLoopEnabled(true);

	// and run
	int ret =  app.exec();

	// Stop the world
	myapp->setFxExecLoopEnabled(false);
	myapp->setLightLoopEnabled(false);

	// Clean up plugins
	myapp->closePlugins();
	// AppCentral::instance()->pluginCentral->unloadPlugins();

	logThread->stopLog();
	delete mywin;

	AppCentral::destroyInstance();
	delete logThread;

	return ret;
}

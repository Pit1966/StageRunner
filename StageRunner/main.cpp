#include <QApplication>

#include "config.h"
#include "main/appcentral.h"
#include "gui/stagerunnermainwin.h"
#include "system/log.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setApplicationName(APP_NAME);
	app.setOrganizationName(APP_ORG_STRING);

	logThread = new Log;

	AppCentral *myapp = AppCentral::instance();
	StageRunnerMainWin *mywin = new StageRunnerMainWin(myapp);

	mywin->show();
	mywin->initConnects();
	logThread->initLog(mywin->logWidget);

	int ret =  app.exec();


	logThread->stopLog();
	delete mywin;
	AppCentral::destroyInstance();
	delete logThread;

	return ret;
}

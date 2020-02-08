/***********************************************************************************
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
************************************************************************************/


#include <QApplication>
#include "configrev.h"
#include "config.h"
#include "appcentral.h"
#include "stagerunnermainwin.h"
#include "usersettings.h"
#include "log.h"
#include "scapplication.h"
#include "runguard.h"
#include "ioplugincentral.h"

int main(int argc, char *argv[])
{
	RunGuard guard( "Stonechip StageRunner" );
	if ( !guard.tryToRun() ) {
		fprintf(stderr, "Tried to run StageRunner twice -> Canceled!");
		return 0;
	}

	PrefVarCore::registerVarClasses();

	ScApplication app(argc, argv);

	app.setApplicationName(APPNAME);
	app.setOrganizationName(APP_ORG_STRING);
	app.setOrganizationDomain(APP_ORG_DOMAIN);

	logThread = new Log;
	logThread->initLog(nullptr);

	AppCentral *myapp = AppCentral::instance();
	StageRunnerMainWin *mywin = new StageRunnerMainWin(myapp);

	logThread->initLog(mywin->logWidget);
	logThread->emitShadowLog(false);

	// Init GUI
	mywin->setApplicationGuiStyle(myapp->userSettings->pApplicationGuiStyle);
	mywin->updateButtonStyles(myapp->userSettings->pDialKnobStyle);
	mywin->show();
	mywin->initConnects();
	// Bring to top
	mywin->raise();
	// This also loads the last project
	mywin->initAppDefaults();

	// Load Plugins
	myapp->loadPlugins();
	myapp->openPlugins();

	// Start the world :-)
	myapp->setLightLoopEnabled(true);
	myapp->setFxExecLoopEnabled(true);

	// Bring to top
	mywin->raise();

	// Show startup log messages (if something happened)
	if (AppCentral::ref().hasModuleError())
		mywin->showModuleError();
	else
		mywin->showShadowLog();

	// and run
	int ret =  app.exec();

	// Stop the world
	myapp->setFxExecLoopEnabled(false);
	myapp->setLightLoopEnabled(false);

	// Clean up plugins
    myapp->closePlugins();
	AppCentral::instance()->pluginCentral->unloadPlugins();

	logThread->stopLog();
	delete mywin;

	AppCentral::destroyInstance();
	delete logThread;

	return ret;
}

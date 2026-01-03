//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#include "scapplication.h"

#ifdef __unix__
# include "system/unixsignalcatcher.h"
#endif

#include <QApplication>
#include <QFontDatabase>
#include <QThread>
// #include <typeinfo>

ScApplication::ScApplication(int &argc, char **argv)
	: QApplication(argc,argv)
{
#ifdef __unix__
	int ret = UnixSignalCatcher::installUnixSignalHandlers();
	qDebug("Install signal handler: %d",ret);

	mySignalCatcher = new UnixSignalCatcher;
#endif

	QThread::currentThread()->setObjectName("StageRunnerMain");

	QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
	QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Regular.otf");
	QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Bold.otf");
	QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Semibold.otf");
	QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-ExtraLight.otf");
}

ScApplication::~ScApplication()
{
#ifdef __unix__
	delete mySignalCatcher;
#endif
}

//bool ScApplication::notify(QObject *receiver, QEvent *event)
//{
//	try {
//		return QApplication::notify(receiver, event);
//	} catch (std::exception &e) {
//		qFatal("Error %s sending event %s to object %s (%s)",
//			   e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
//			   typeid(*receiver).name());
//	}
//	catch (...) {
//		qFatal("Error <unknown> sending event %s to object %s (%s)",
//			   typeid(*event).name(), qPrintable(receiver->objectName()),
//			   typeid(*receiver).name());
//	}

//	// qFatal aborts, so this isn't really necessary
//	// but you might continue if you use a different logging lib
//	return false;
//}

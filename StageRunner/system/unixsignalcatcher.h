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

#ifndef UNIXSIGNALCATCHER_H
#define UNIXSIGNALCATCHER_H

#include <QObject>
#include <QSocketNotifier>

#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

class UnixSignalCatcher : public QObject
{
	Q_OBJECT
private:
	static int fdSigHup[2];
	static int fdSigCont[2];

	QSocketNotifier *notifierSigHup;
	QSocketNotifier *notifierSigCont;

public:
	UnixSignalCatcher(QObject *parent = 0);
	~UnixSignalCatcher();

	/**
	 * @brief Install Unix signal handlers [static]
	 * @return 0 on success
	 */
	static int installUnixSignalHandlers();
	/**
	 * @brief handlerSignalHup
	 * @param unused
	 */
	static void handlerSignalHup(int unused);
	/**
	 * @brief handlerSignalCont
	 * @param unused
	 */
	static void handlerSignalCont(int unused);

public slots:
	// Qt signal handlers (called indirect from unix signal handlers)
	void qtHandleSignal(int signum);
	void qtHandleSigHup();
	void qtHandleSigCont();

signals:

};

#endif // UNIXSIGNALCATCHER_H

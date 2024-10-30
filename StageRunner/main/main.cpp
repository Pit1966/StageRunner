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

#ifdef __unix__
#include <signal.h>
#include <execinfo.h>

typedef void (*sighandler_t) (int);

static sighandler_t sys_signal (int sig_nr, sighandler_t signalhandler)
{
	struct sigaction new_sig,old_sig;

	new_sig.sa_handler = signalhandler;
	sigemptyset (&new_sig.sa_mask);
	new_sig.sa_flags = SA_RESTART;

	if (sigaction (sig_nr, &new_sig, &old_sig) < 0) return SIG_ERR;
	return old_sig.sa_handler;
}

static sighandler_t sys_signal_add (int sig_nr, sighandler_t signalhandler)
{
	struct sigaction add_sig;

	if (sigaction (sig_nr, NULL, &add_sig) < 0) return SIG_ERR;

	add_sig.sa_handler = signalhandler;
	sigaddset (&add_sig.sa_mask, sig_nr);
	add_sig.sa_flags = SA_RESTART;

	if (sigaction (sig_nr, &add_sig, NULL) < 0) return SIG_ERR;
	return add_sig.sa_handler;
}

static sighandler_t sys_signal_del (int sig_nr)
{
	struct sigaction del_sig;

	if (sigaction (sig_nr, NULL, &del_sig) <0) return SIG_ERR;

	del_sig.sa_handler = SIG_DFL;
	sigdelset (&del_sig.sa_mask, sig_nr);
	del_sig.sa_flags = SA_RESTART;

	if (sigaction (sig_nr, &del_sig, NULL) < 0) return SIG_ERR;
	return del_sig.sa_handler;
}

#ifdef USE_LIBUNWIND_EXCEPTIONS

int getFileAndLine (unw_word_t addr, char *file, size_t flen, int *line)
{
	Q_UNUSED(flen);

	static char buf[512];
	static char dir[512];

	// prepare command to be executed
	// our program need to be passed after the -e parameter
	readlink("/proc/self/exe", dir, 512);
	sprintf (buf, "/usr/bin/addr2line -C -e %s/analyzer4D -f -i %lx", dirname(dir), addr);
	FILE* f = popen (buf, "r");

	if (f == NULL)
	{
		perror (buf);
		return 0;
	}

	// get function name
	fgets (buf, 512, f);

	// get file and line
	fgets (buf, 512, f);

	if (buf[0] != '?')
	{
		char *p = buf;

		// file name is until ':'
		while (*p != ':')
		{
			p++;
		}

		*p++ = 0;
		// after file name follows line number
		strcpy (file , buf);
		sscanf (p,"%d", line);
	}
	else
	{
		strcpy (file,"unkown");
		*line = 0;
	}

	pclose(f);

	return true;
}

static void show_stack_backtrace(bool isNoCrash = false, bool showfullfilepath = false)
{
	if (isNoCrash) {
		DEVELTEXT("DEVELOPER: stack backtrace -----------------------------------------------------");
	}
	else {
		DEBUGERROR("CRASH! The analyzer application terminated! Here is a backtrace:");
		DEBUGERROR("The logfile in '/tmp/optimizer.log' should contain more information.");
	}

	char name[256];
	unw_cursor_t cursor;
	unw_word_t ip, sp, offp;
	unw_context_t uc;

	unw_getcontext(&uc);

	QString callstackstring;

	if ( unw_init_local(&cursor, &uc) != 0)
	{
		DEBUGERROR("unw_init_local failed");
		return;
	}

	int iSkip =0;
	while (unw_step(&cursor) > 0)
	{
		iSkip++;
		char file[512];
		int line = 0;

		name[0] = '\0';
		int iResult = unw_get_proc_name(&cursor, name, 256, &offp);

		if ( iResult != UNW_ESUCCESS )
		{
			DEBUGERROR("no proc name");
		}

		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);

		char * demangled = cplus_demangle (name, 0);

		getFileAndLine((long)ip, file, 512, &line);
		QString filestr = QString::fromLocal8Bit(file);
		if (!showfullfilepath)
			filestr = filestr.section("analyzer4D/", -1);

		// Aus irgendeinem Grund, stimmt die zurückgegebene Zeilennummer nicht, deshalb
		// wird hier die vorhergehende Zeilennummer ausgegeben
		sprintf (name, "%s in file %s line %d", demangled, filestr.toLocal8Bit().constData(), line);

		if (iSkip>=3)  // skip first two entries (garbage callstack)
		{
			if (isNoCrash) {
				DEVELTEXT("%s",name);
			} else {
				DEBUGERROR("%s",name);
			}
			callstackstring.append(name);
			callstackstring.append("\n");
		}
	}

	DEBUGERROR("---- show stack backtrace ----");

	// QMessageBox::critical(0,"Signal catched",callstackstring);
}

#else // USE_LIBUNWIND_EXCEPTIONS

static void show_stack_backtrace(bool isNoCrash = false) {
	void *trace[60];
	char **messages = (char **)NULL;
	int i, trace_size = 0;

	trace_size = backtrace(trace, 60);
	messages = backtrace_symbols(trace, trace_size);
	if (isNoCrash) {
		DEVELTEXT("DEVELOPER: stack backtrace -----------------------------------------------------");
	}
	else {
		DEBUGERROR("CRASH! The stagerunner application terminated! Here is a backtrace:");
		DEBUGERROR("The logfile in '/tmp/stagerunner.log' should contain more information.");
	}

	if (trace_size > 2) {
		if (isNoCrash) {
			for (i=2; i<trace_size; ++i) {
				DEVELTEXT("[trace] %s",messages[i]);
			}
		}
		else {
			for (i=2; i<trace_size; ++i) {
				DEBUGERROR("[trace] %s",messages[i]);
			}
		}
	}
	free(messages);
}

#endif  // USE_LIBUNWIND_EXCEPTIONS

QString getStackBacktrace() {
	QString backtraceStr;
	void *trace[60];
	char **messages = (char **)NULL;
	int i, trace_size = 0;

	trace_size = backtrace(trace, 60);
	messages = backtrace_symbols(trace, trace_size);

	if (trace_size > 2) {
			for (i=2; i<trace_size; ++i) {
				backtraceStr += messages[i];
				backtraceStr += "\n";
			}
	}

	free(messages);
	return backtraceStr;
}

QString getSmallBacktrace() {
	QString backtraceStr;
	void *trace[60];
	char **messages = (char **)NULL;
	int i, trace_size = 0;

	trace_size = backtrace(trace, 10);
	messages = backtrace_symbols(trace, trace_size);

	if (trace_size > 2) {
			for (i=2; i<trace_size; ++i) {
				backtraceStr += messages[i];
				backtraceStr += "\n";
			}
	}

	free(messages);
	return backtraceStr;
}


static void check_sys_sig ( int sig_nr )
{
	if (sig_nr != SIGTERM)
		DEVELTEXT("System:: Exception!");

	if (sig_nr == SIGINT) {
		DEBUGERROR("System:: SIGINT captured");
	}
	else if (sig_nr == SIGKILL) {
		DEBUGERROR("System:: SIGKILL catched!");
		sys_signal_del(SIGKILL);
	}
	else if (sig_nr == SIGFPE) {
		DEBUGERROR("System:: SIGFPE catched! Someone divided by NULL ?");
	}
	else if (sig_nr == SIGSEGV) {
		DEBUGERROR("System:: SIGSEGV catched! Segmentation Fault");
	}
	else if (sig_nr == SIGTERM) {
		// SIGTERM is not necessarily a bad signal
		// The XFCE desktop for instance uses it to terminate the application on shut down
		LOGTEXT("<font color=blue>SIGTERM</font> catched!");
		return;
	}
	else if (sig_nr == SIGHUP) {
		// SIGTERM is not necessarily a bad signal
		// The XFCE desktop for instance uses it to terminate the application on shut down
		LOGTEXT("<font color=blue>SIGHUP<font> catched!");
		return;
	}
	else if (sig_nr == SIGABRT) {
		DEBUGERROR("System:: SIGABRT catched! Application received abnormal termination signal");
	}
	else {
		DEBUGERROR("System:: Signal No %d captured",sig_nr);
	}

	show_stack_backtrace();

	logThread->flushLogfile();
	logThread->closeLogfile();

	raise (SIGKILL);
}

bool register_sys_signal_handler()
{
	sighandler_t ret = 0;
	ret = sys_signal (SIGINT, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGSEGV, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGALRM, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGFPE, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGILL, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGSEGV, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGSYS, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGHUP, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGTERM, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGQUIT, check_sys_sig);
	if (ret != SIG_ERR) ret = sys_signal_add (SIGABRT, check_sys_sig);
	// if (ret != SIG_ERR) ret = sys_signal_add (SIGKILL, check_sys_sig);


	if (ret == SIG_ERR) {
		qDebug("System:: Unable to install signal handler");
		DEBUGERROR("System:: Unable to install signal handler");
		return false;
	} else {
		LOGTEXT("System:: Signal handler successfully installed.");
		return true;
	}

}
#endif // ifdef __unix__


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
#ifdef __unix__
	register_sys_signal_handler();
#endif

	AppCentral *myapp = AppCentral::instance();
	StageRunnerMainWin *mywin = new StageRunnerMainWin(myapp);

	// logThread->initLog(mywin->logWidget);
	// logThread->emitShadowLog(false);

	// Init GUI
	mywin->setApplicationGuiStyle(myapp->userSettings->pApplicationGuiStyle);
	mywin->updateButtonStyles(myapp->userSettings->pDialKnobStyle);
	mywin->initModules();
	mywin->show();
	mywin->initConnects();

	// Bring to top
	mywin->raise();

	// init Audio and media players
	myapp->initAudioControl();

	logThread->initLog(mywin->logWidget);
	logThread->emitShadowLog(false);

	// This also loads the last project
	mywin->initAppDefaults();

	/// @todo should this be done before the last project is loaded
	// Load Plugins
	myapp->loadPlugins();
	myapp->openPlugins();

	// start TCP server
	myapp->startTcpServer();

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
	myapp->startupReady();
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


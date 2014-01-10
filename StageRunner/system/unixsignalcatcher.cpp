#include "unixsignalcatcher.h"


int UnixSignalCatcher::fdSigHup[2] = {0,0};
int UnixSignalCatcher::fdSigCont[2] = {0,0};

UnixSignalCatcher::UnixSignalCatcher(QObject *parent) :
	QObject(parent)
{
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fdSigHup))
		qFatal("Couldn't create HUP socketpair");

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fdSigCont))
		qFatal("Couldn't create TERM socketpair");

	notifierSigHup = new QSocketNotifier(fdSigHup[1],QSocketNotifier::Read,this);
	connect(notifierSigHup,SIGNAL(activated(int)),this,SLOT(qtHandleSignal(int)));
	connect(notifierSigHup,SIGNAL(activated(int)),this,SLOT(qtHandleSigHup()));


	notifierSigCont = new QSocketNotifier(fdSigCont[1],QSocketNotifier::Read,this);
	connect(notifierSigCont,SIGNAL(activated(int)),this,SLOT(qtHandleSignal(int)));
	connect(notifierSigCont,SIGNAL(activated(int)),this,SLOT(qtHandleSigCont()));
}

UnixSignalCatcher::~UnixSignalCatcher()
{
	delete notifierSigCont;
	delete notifierSigHup;
}

int UnixSignalCatcher::installUnixSignalHandlers()
{
	struct sigaction hup, term;

	hup.sa_handler = UnixSignalCatcher::handlerSignalHup;
	sigemptyset(&hup.sa_mask);
	hup.sa_flags = 0;
	hup.sa_flags |= SA_RESTART;

	if (sigaction(SIGHUP, &hup, 0) > 0)
	   return 1;

	term.sa_handler = UnixSignalCatcher::handlerSignalCont;
	sigemptyset(&term.sa_mask);
	term.sa_flags |= SA_RESTART;

	if (sigaction(SIGCONT, &term, 0) > 0)
	   return 2;

	return 0;
}

void UnixSignalCatcher::handlerSignalHup(int unused)
{
	char a = 1;
	::write(fdSigHup[0], &a, sizeof(a));

	qDebug("Unix: catched SigHup: %d",unused);
}

void UnixSignalCatcher::handlerSignalCont(int unused)
{
	char a = 1;
	::write(fdSigCont[0], &a, sizeof(a));

	qDebug("Unix: catched SigCont: %d",unused);
}

void UnixSignalCatcher::qtHandleSignal(int signum)
{
	qDebug("Qt: Catched signal: %d",signum);
}

void UnixSignalCatcher::qtHandleSigHup()
{
	notifierSigHup->setEnabled(false);
	char tmp;
	::read(fdSigHup[1],&tmp,sizeof(tmp));


	notifierSigHup->setEnabled(true);
}

void UnixSignalCatcher::qtHandleSigCont()
{
	notifierSigCont->setEnabled(false);
	char tmp;
	::read(fdSigCont[1],&tmp,sizeof(tmp));


	notifierSigCont->setEnabled(true);
}

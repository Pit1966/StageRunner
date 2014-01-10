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

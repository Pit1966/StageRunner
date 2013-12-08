#ifndef EXECLOOPTHREADINTERFACE_H
#define EXECLOOPTHREADINTERFACE_H

#include <QThread>

class FxList;
class FxExecLoop;
class ExecCenter;
class FxControl;

// template <class T> class MutexQList;

class ExecLoopThreadInterface : public QThread
{
	Q_OBJECT
private:
	FxControl & fxControlRef;
	FxExecLoop *execLoop;

public:
	ExecLoopThreadInterface(FxControl & unit_fx);
	bool startThread();
	bool stopThread();

private:
	void run();

signals:

public slots:

};

#endif // EXECLOOPTHREADINTERFACE_H

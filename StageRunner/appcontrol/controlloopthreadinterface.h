#ifndef CONTROLLOOPTHREADINTERFACE_H
#define CONTROLLOOPTHREADINTERFACE_H

#include <QThread>

class ControlLoop;

class ControlLoopThreadInterface : public QThread
{
	Q_OBJECT
private:
	ControlLoop * worker;

public:
	ControlLoopThreadInterface(QObject *parent = 0);
	~ControlLoopThreadInterface();

	bool startThread();
	bool stopThread();

private:
	void run();

signals:

public slots:

};

#endif // CONTROLLOOPTHREADINTERFACE_H

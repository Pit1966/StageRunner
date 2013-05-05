#ifndef CONTROLLOOPTHREADINTERFACE_H
#define CONTROLLOOPTHREADINTERFACE_H

#include <QThread>

class LightLoop;
class FxList;
class LightControl;

template <class T> class MutexQList;

class LightLoopThreadInterface : public QThread
{
	Q_OBJECT
private:
	LightLoop * lightLoop;
	LightControl * unitLightRef;

public:
	LightLoopThreadInterface(LightControl *unit_light);
	~LightLoopThreadInterface();

	bool startThread(MutexQList<const FxList*>*listref);
	bool stopThread();

private:
	void run();

signals:

public slots:

};

#endif // CONTROLLOOPTHREADINTERFACE_H

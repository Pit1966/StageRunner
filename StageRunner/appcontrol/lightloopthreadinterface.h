#ifndef CONTROLLOOPTHREADINTERFACE_H
#define CONTROLLOOPTHREADINTERFACE_H

#include <QThread>

class LightLoop;
class FxList;
class LightControl;
class FxSceneItem;

// template <class T> class MutexQList;

class LightLoopThreadInterface : public QThread
{
	Q_OBJECT
private:
	LightControl & lightControlRef;
	LightLoop * lightLoop;

public:
	LightLoopThreadInterface(LightControl & unit_light);
	~LightLoopThreadInterface();

	bool startThread();
	bool stopThread();
	inline LightLoop * getLightLoopInstance() {return lightLoop;}

private:
	void run();

signals:
	void wantedDeleteFxScene(FxSceneItem *scene);

public slots:

};

#endif // CONTROLLOOPTHREADINTERFACE_H

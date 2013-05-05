#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include "config.h"
#include "toolclasses.h"

#include <QObject>
#include <QByteArray>

class AppCentral;
class LightLoopThreadInterface;
class FxList;
class QLCIOPlugin;
class FxSceneItem;


class LightControl : public QObject
{
	Q_OBJECT
public:
	AppCentral *myApp;
	LightLoopThreadInterface *lightLoopInterface;
	MutexQList<const FxList*>lightFxLists;
	MutexQHash<int,const FxSceneItem*>activeScenes;
	QByteArray dmxOutputValues[MAX_DMX_UNIVERSE];
	bool dmxOutputChanged[MAX_DMX_UNIVERSE];

public:
	LightControl(AppCentral *app_central);
	~LightControl();

	bool setLightLoopEnabled(bool state);
	bool addFxListToControlLoop(const FxList *list);
	bool sendChangedDmxData();
	bool setSceneActive(const FxSceneItem * scene);
	bool setSceneIdle(const FxSceneItem * scene);

private:
	void init();

signals:

public slots:

};

#endif // LIGHTCONTROL_H

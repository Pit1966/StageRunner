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
	MutexQHash<int,FxSceneItem*>activeScenes;
	QByteArray dmxOutputValues[MAX_DMX_UNIVERSE];
	bool dmxOutputChanged[MAX_DMX_UNIVERSE];

public:
	LightControl(AppCentral *app_central);
	~LightControl();

	bool setLightLoopEnabled(bool state);
	bool addFxListToControlLoop(const FxList *list);
	bool sendChangedDmxData();
	bool startFxSceneSimple(FxSceneItem *scene);
	bool stopFxScene(FxSceneItem *scene);
	bool setSceneActive(FxSceneItem *scene);
	bool setSceneIdle(FxSceneItem * scene);
	qint32 black(qint32 time_ms);

private:
	void init();

signals:
	void sceneChanged(FxSceneItem *scene);
	void sceneFadeChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void sceneCueReady(FxSceneItem *scene);
	void outputUniverseChanged(int universe, const QByteArray &dmxValues);
	void audioSlotVolChanged(int slot, int value);

public slots:
	void onSceneStatusChanged(FxSceneItem *scene, quint32 status);
	void onSceneFadeProgressChanged(FxSceneItem *scene, int perMilleA, int perMilleB);
	void onSceneCueReady(FxSceneItem *scene);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);

};

#endif // LIGHTCONTROL_H

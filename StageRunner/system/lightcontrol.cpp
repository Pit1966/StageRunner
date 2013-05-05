#include "lightcontrol.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "controlloopthreadinterface.h"
#include "controlloop.h"
#include "fxitem.h"
#include "fxlist.h"
#include "qlcioplugin.h"
#include "ioplugincentral.h"


LightControl::LightControl(AppCentral *app_central)
	: QObject()
	,myApp(app_central)
{
	init();
}

LightControl::~LightControl()
{
	delete lightLoopInterface;

}

bool LightControl::setLightLoopEnabled(bool state)
{
	bool ok = true;
	if (state) {
		ok = lightLoopInterface->startThread(&lightFxLists);
	} else {
		ok = lightLoopInterface->stopThread();
	}
	return ok;
}

/**
 * @brief Add a FxList to be processed in ControlLoop
 * @param list Pointer to an existing FxList
 * @return true if added or already existing
 */
bool LightControl::addFxListToControlLoop(const FxList *list)
{
	bool ok = true;
	int id = list->regId();
	if (id < 1) {
		DEBUGERROR("%s: We cannot add a list without Id",Q_FUNC_INFO);
		return false;
	}

	lightFxLists.lock();
	bool id_exists = false;
	for (int t=0; t<lightFxLists.size(); t++) {
		if (lightFxLists.at(t)->regId() == id) id_exists = true;
	}

	if (!id_exists) {
		lightFxLists.append(list);
	}

	lightFxLists.unlock();
	return ok;
}

bool LightControl::sendChangedDmxData()
{
	bool sent = false;
	QList<QLCIOPlugin*>plugins = myApp->pluginCentral->qlcPlugins();
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		if (dmxOutputChanged[t]) {
			for (int i=0; i<plugins.size(); i++) {
				plugins.at(i)->writeUniverse(t,dmxOutputValues[t]);
				sent = true;
			}
			dmxOutputChanged[t] = false;
		}
	}

	return sent;
}

void LightControl::init()
{
	for (int t=0; t<MAX_DMX_UNIVERSE; t++) {
		dmxOutputChanged[t] = true;
		dmxOutputValues[t].resize(512);
		memset(dmxOutputValues[t].data(),0,512);
	}
	lightLoopInterface = new LightLoopThreadInterface(this);
}

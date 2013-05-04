#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include "system/commandsystem.h"

#include <QList>
#include <QObject>
#include <QByteArray>

class AudioControl;
class Project;
class UserSettings;
class FxItem;
class FxList;
class IOPluginCentral;
class ControlLoopThreadInterface;

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool edit_mode_f;
	QByteArray dmx_direct_data;

public:
	AudioControl *unitAudio;
	Project *project;
	UserSettings *userSettings;
	IOPluginCentral *pluginCentral;
	ControlLoopThreadInterface *lightLoop;

private:
	AppCentral();
	~AppCentral();
	void init();

public:
	static AppCentral * instance();
	static bool destroyInstance();

	void clearProject();
	bool setLightLoopEnabled(bool state);


	void stopAllFxAudio();
	void fadeoutAllFxAudio();
	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	inline bool isEditMode() {return edit_mode_f;}
	inline void setEditMode(bool state) {edit_mode_f = state;}

	void loadPlugins();
	void openPlugins();
	void closePlugins();

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd);
	void executeNextFx(int listID);

	void testSetDmxChannel(int val, int channel);

};

#endif // APPCENTRAL_H

#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include "system/commandsystem.h"

#include <QList>
#include <QObject>

class AudioControl;
class Project;
class UserSettings;
class FxItem;
class FxList;
class IOPluginCentral;

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool edit_mode_f;

public:
	AudioControl *unitAudio;
	Project *project;
	UserSettings *userSettings;
	IOPluginCentral *pluginCentral;

private:
	AppCentral();
	~AppCentral();
	void init();

public:
	static AppCentral * instance();
	static bool destroyInstance();

	void clearProject();


	void stopAllFxAudio();
	void fadeoutAllFxAudio();
	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	inline bool isEditMode() {return edit_mode_f;}
	inline void setEditMode(bool state) {edit_mode_f = state;}

	void loadPlugins();

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd);
	void executeNextFx(int listID);

};

#endif // APPCENTRAL_H

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

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;


public:
	AudioControl *unitAudio;
	Project *project;
	UserSettings *userSettings;

private:
	AppCentral();
	~AppCentral();
	void init();

public:
	static AppCentral * instance();
	static bool destroyInstance();

	void clearProject();


	void stopAllFxAudio();
	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd);
	void executeNextFx(int listID);

};

#endif // APPCENTRAL_H

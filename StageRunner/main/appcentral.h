#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include <QList>
#include <QObject>

class AudioControl;
class Project;
class UserSettings;
class FxItem;

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;

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

public slots:
	void executeFxCmd(FxItem *fx, int cmd);

};

#endif // APPCENTRAL_H

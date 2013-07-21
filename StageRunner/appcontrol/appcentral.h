#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include "commandsystem.h"

#include <QList>
#include <QObject>
#include <QByteArray>

class AudioControl;
class LightControl;
class Project;
class UserSettings;
class FxItem;
class FxList;
class IOPluginCentral;
class DmxMonitor;

using namespace AUDIO;
using namespace LIGHT;

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool edit_mode_f;
	bool input_assign_mode_f;
	FxItem * input_assign_target_fxitem;

	FxItem * current_selected_project_fx;

public:
	AudioControl *unitAudio;
	LightControl *unitLight;
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
	bool setLightLoopEnabled(bool state);

	void stopAllFxAudio();
	void fadeoutAllFxAudio();

	void lightBlack(qint32 time_ms = 0);

	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	inline bool isEditMode() const {return edit_mode_f;}
	void setEditMode(bool state);
	inline bool isInputAssignMode() const {return input_assign_mode_f;}
	void setInputAssignMode(bool state);
	void setInputAssignMode(FxItem *fx);

	void loadPlugins();
	void openPlugins();
	void reOpenPlugins();
	void closePlugins();
	DmxMonitor *openDmxInMonitor(int universe);
	DmxMonitor *openDmxOutMonitor(int universe);

	void assignInputToSelectedFxItem(qint32 universe, qint32 channel, int value);

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd);
	void executeNextFx(int listID);
	void moveToFollowerFx(int listID);
	void moveToForeRunnerFx(int listID);

	void testSetDmxChannel(int val, int channel);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);
	void storeSelectedFxListWidgetFx(FxItem *item);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void editModeChanged(bool state);
	void inputAssignModeChanged(bool state);
	void inputAssigned(FxItem *);

};

#endif // APPCENTRAL_H

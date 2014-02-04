#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include "commandsystem.h"

#include <QList>
#include <QObject>
#include <QByteArray>
#include <QWidget>

class AudioControl;
class LightControl;
class Project;
class UserSettings;
class FxItem;
class FxList;
class IOPluginCentral;
class DmxMonitor;
class ExecCenter;
class Executer;
class FxList;
class FxControl;
class FxListVarSet;

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
	FxItem * last_global_selected_fxitem;

public:
	QObject *mainWinObj;
	AudioControl *unitAudio;
	LightControl *unitLight;
	FxControl *unitFx;
	Project *project;
	UserSettings *userSettings;
	IOPluginCentral *pluginCentral;
	ExecCenter *execCenter;
	FxListVarSet *templateFxList;

private:
	AppCentral();
	~AppCentral();
	void init();

public:
	static AppCentral * instance();
	static bool destroyInstance();

	void clearProject();
	bool setLightLoopEnabled(bool state);
	bool setFxExecLoopEnabled(bool state);

	void stopAllFxAudio();
	void fadeoutAllFxAudio();
	void stopAllSequencesAndPlaylists();

	void lightBlack(qint32 time_ms = 0);
	void sequenceStop(FxItem *fxseq = 0);

	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	inline bool isEditMode() const {return edit_mode_f;}
	bool isExperimentalAudio() const;
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
	bool addFxAudioDialog(FxList *fxlist, QWidget *widget = 0, int row = -1);
	FxItem *globalSelectedFx() {return last_global_selected_fxitem;}
	FxItem *addDefaultSceneToFxList(FxList *fxlist);

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd, Executer * exec);
	void executeNextFx(int listID = 1);
	void clearCurrentFx(int listID);
	void moveToFollowerFx(int listID);
	void moveToForeRunnerFx(int listID);
	void setEditMode(bool state);
	void setExperimentalAudio(bool state);
	void setFFTAudioChannelMask(qint32 mask);

	void testSetDmxChannel(int val, int channel);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);
	void setGlobalSelectedFx(FxItem *item);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void editModeChanged(bool state);
	void inputAssignModeChanged(bool state);
	void inputAssigned(FxItem *);

};

#endif // APPCENTRAL_H

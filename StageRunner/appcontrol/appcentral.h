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

using namespace AUDIO;
using namespace LIGHT;

class AppCentral : public QObject
{
	Q_OBJECT

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool edit_mode_f;
	bool experimental_audio_f;
	bool input_assign_mode_f;
	FxItem * input_assign_target_fxitem;

	FxItem * current_selected_project_fx;

public:
	QObject *mainWinObj;
	AudioControl *unitAudio;
	LightControl *unitLight;
	FxControl *unitFx;
	Project *project;
	UserSettings *userSettings;
	IOPluginCentral *pluginCentral;
	ExecCenter *execCenter;

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
	inline bool isExperimentalAudio() const {return experimental_audio_f;}
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

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd, Executer * exec);
	void executeNextFx(int listID);
	void moveToFollowerFx(int listID);
	void moveToForeRunnerFx(int listID);
	void setEditMode(bool state);
	void setExperimentalAudio(bool state);

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

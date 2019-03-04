#ifndef STAGERUNNERMAINWIN_H
#define STAGERUNNERMAINWIN_H

#include "ui_stagerunnermainwin.h"

#include <QString>
#include <QStyle>


class AppCentral;
class QErrorMessage;
class FxItemPropertyWidget;
class QDockWidget;
class SceneStatusWidget;
class FxSceneItem;
class SequenceStatusWidget;
class FxListWidget;
class UserSettings;
class FxScriptItem;
class PsDockWidget;

class StageRunnerMainWin : public QMainWindow, private Ui::StageRunnerMainWin
{
	Q_OBJECT
private:
	AppCentral *appCentral;
	QErrorMessage *msg_dialog;
	PsDockWidget *fxitem_editor_dock;
	PsDockWidget *scene_status_dock;
	PsDockWidget *sequence_status_dock;
	PsDockWidget *template_dock;

public:
	QStyle *dialWidgetStyle;
	QTextEdit *logWidget;
	FxItemPropertyWidget *fxItemEditor;
	SceneStatusWidget *sceneStatusDisplay;
	SequenceStatusWidget *seqStatusDisplay;
	FxListWidget *templateWidget;

	int activeKeyModifiers;

public:
	StageRunnerMainWin(AppCentral * myapp);
	~StageRunnerMainWin();

	void initConnects();

	void clearProject();
	void initAppDefaults();
	void showShadowLog();
	void showModuleError();
	void copyGuiSettingsToProject();
	void copyProjectSettingsToGui();

	void openFxSceneItemPanel(FxSceneItem *fx);
	void openFxPlayListItemPanel(FxPlayListItem *fx);
	void openFxSeqItemPanel(FxSeqItem *fx);
	void openFxScriptPanel(FxScriptItem *fx);
	void setProjectName(const QString &path);

private:
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent *event);

public slots:
	void showInfoMsg(QString where, QString text);
	void showErrorMsg(QString where, QString text);
	void setApplicationGuiStyle(QString style);
	void updateButtonStyles(QString style = "");
	void openFxPropertyEditor(FxItem *item);
	void openFxItemPanel(FxItem *fx);
	void applyUserSettingsToGui(UserSettings *set);
	void guiSetAudioOutput(AUDIO::AudioOutputType type);

private slots:
	void on_addAudioFxButton_clicked();
	void on_actionSave_Project_as_triggered();
	void on_actionLoad_Project_triggered();
	void on_actionNew_Project_triggered();
	void on_actionEdit_Mode_toggled(bool arg1);
	void on_actionSetup_triggered();
	void on_actionSave_Project_triggered();
	void on_actionExit_StageRunner_triggered();
	void on_addFxSceneButton_clicked();
	void on_debugLevelSpin_valueChanged(int arg1);
	void on_stopMainLoopButton_clicked();
	void on_actionDMX_Input_triggered();
	void on_actionDMX_Output_triggered();
	void on_actionInput_Assign_Mode_triggered(bool checked);
	void on_cloneSelectedSceneButton_clicked();
	void on_addAudioPlayListButton_clicked();
	void on_addAudioTrackButton_clicked();
	void on_actionInitialize_plugins_DMX_triggered();
	void on_actionInfo_triggered();
	void on_addFxSeqButton_clicked();
	void on_actionExperimental_audio_mode_triggered(bool checked);
	void on_actionUse_SDL_audio_triggered(bool arg1);
	void on_actionOpen_FxItem_triggered();
	void on_saveTemplatesButton_clicked();
	void on_loadTemplatesButton_clicked();
	void on_actionShow_Templates_triggered();
	void on_actionShow_Fx_Properties_Editor_triggered();
	void on_actionShow_Scene_Status_triggered();
	void on_actionShow_Sequence_Status_triggered();
	void on_saveUniverseButton_clicked();
	void on_loadUniverseButton_clicked();

	void on_actionVirtualDmxOutput_triggered(bool checked);


	void on_actionClassic_audio_mode_triggered(bool checked);

private:
	void init();
	void setup_gui_docks();
	void restore_window();
};

#endif // STAGERUNNERMAINWIN_H

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

class StageRunnerMainWin : public QMainWindow, private Ui::StageRunnerMainWin
{
	Q_OBJECT
private:
	AppCentral *appCentral;
	QErrorMessage *msg_dialog;
	QDockWidget *fxitem_editor_dock;
	QDockWidget *scene_status_dock;

public:
	QStyle *dialWidgetStyle;
	QTextEdit *logWidget;
	FxItemPropertyWidget *fxItemEditor;
	SceneStatusWidget *sceneStatusDisplay;

	int activeKeyModifiers;

public:
	StageRunnerMainWin(AppCentral * myapp);
	~StageRunnerMainWin();

	void initConnects();

	void clearProject();
	void initAppDefaults();
	void copyGuiSettingsToProject();
	void copyProjectSettingsToGui();

private:
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent *event);

public slots:
	void showInfoMsg(QString where, QString text);
	void showErrorMsg(QString where, QString text);
	void setApplicationGuiStyle(QString style);
	void updateButtonStyles(QString style = "");

private slots:
	void slot_addFxFile(QString path, int pos = -1);
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

	void on_executeAudioPlayListButton_clicked();

	void on_actionInitialize_plugins_DMX_triggered();

	void on_actionInfo_triggered();

private:
	void init();
	void setup_gui_docks();
	void restore_window();
};

#endif // STAGERUNNERMAINWIN_H

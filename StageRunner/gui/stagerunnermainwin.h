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
	AppCentral *mainapp;
	QErrorMessage *msg_dialog;
	QDockWidget *fxitem_editor_dock;
	QDockWidget *scene_status_dock;

public:
	QStyle *dialWidgetStyle;
	QTextEdit *logWidget;
	FxItemPropertyWidget *fxItemEditor;
	SceneStatusWidget *sceneStatusDisplay;

	bool shiftPressedFlag;

public:
	StageRunnerMainWin(AppCentral * myapp);
	~StageRunnerMainWin();

	void initConnects();
	void updateButtonStyles();

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

private:
	void init();
	void setup_gui_docks();
	void restore_window();
};

#endif // STAGERUNNERMAINWIN_H

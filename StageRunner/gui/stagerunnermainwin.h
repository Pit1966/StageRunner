#ifndef STAGERUNNERMAINWIN_H
#define STAGERUNNERMAINWIN_H

#include "ui_stagerunnermainwin.h"

#include <QString>
#include <QStyle>

class AppCentral;

class StageRunnerMainWin : public QMainWindow, private Ui::StageRunnerMainWin
{
	Q_OBJECT
private:
	AppCentral *mainapp;
	QStyle *dialWidgetStyle;

public:
	QTextEdit *logWidget;

public:
	StageRunnerMainWin(AppCentral * myapp);
	~StageRunnerMainWin();

	void initConnects();
	void updateButtonStyles();

	void clearProject();
	void initAppDefaults();


private slots:
	void on_addAudioFxButton_clicked();
	void on_actionSave_Project_as_triggered();

	void on_actionLoad_Project_triggered();

	void on_actionNew_Project_triggered();

private:
	void init();
};

#endif // STAGERUNNERMAINWIN_H

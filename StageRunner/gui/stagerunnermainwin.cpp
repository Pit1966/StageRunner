#include "stagerunnermainwin.h"
#include "system/commandsystem.h"
#include "main/appcentral.h"
#include "system/audiocontrol.h"
#include "fx/fxlist.h"
#include "main/project.h"
#include "main/usersettings.h"
#include "thirdparty/widget/qsynthdialpeppinostyle.h"
#include "thirdparty/widget/qsynthdialclassicstyle.h"

#include <QFileDialog>


StageRunnerMainWin::StageRunnerMainWin(AppCentral *myapp) :
	QMainWindow(0)
{
	mainapp = myapp;
	dialWidgetStyle = 0;

	init();

	setupUi(this);

	updateButtonStyles();

	// For external access
	logWidget = logEdit;
}

StageRunnerMainWin::~StageRunnerMainWin()
{
	delete dialWidgetStyle;
}

void StageRunnerMainWin::initConnects()
{
	// FxListWidget (Liste der Effekte im Mainwin)
	connect(fxListWidget,SIGNAL(fxCmdActivated(FxItem*,CtrlCmd)),mainapp,SLOT(executeFxCmd(FxItem*,CtrlCmd)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),seqCtrlGroup,SLOT(setNextFx(FxItem*)));

	// Audio Control Panel <-> Audio Control
	connect(mainapp->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),mainapp->unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(mainapp->unitAudio,SIGNAL(vuLevelChanged(int,int,int)),audioCtrlGroup,SLOT(setVuMeterLevel(int,int,int)));



}

void StageRunnerMainWin::updateButtonStyles()
{
	// Update Style of Dial knobs
	if (dialWidgetStyle) {
		delete dialWidgetStyle;
	}

	//dialWidgetStyle = new qsynthDialPeppinoStyle();
	dialWidgetStyle = new qsynthDialClassicStyle();

	QList<qsynthKnob *> all_dials = findChildren<qsynthKnob *>();
	foreach(qsynthKnob* dial, all_dials) {
		dial->setStyle(dialWidgetStyle);
		dial->setDialMode(qsynthKnob::LinearMode);
	}

}

/**
 * @brief Clear Project
 *
 * Generates a new empty project in main application
 */
void StageRunnerMainWin::clearProject()
{
	// Clear Project and project member
	mainapp->clearProject();

	// Clear Fx list in GUI
	fxListWidget->setFxList(mainapp->project->fxList);
}

void StageRunnerMainWin::init()
{
}


/**
 * @brief Initialize Application and restore Usersettings
 *
 * This includes:
 * - Loading predefined project (or last loaded project)
 *
 */
void StageRunnerMainWin::initAppDefaults()
{
	if (mainapp->userSettings->pLastProjectLoadPath.size()) {
		mainapp->project->loadFromFile(mainapp->userSettings->pLastProjectLoadPath);
		fxListWidget->setFxList(mainapp->project->fxList);

	}
}


void StageRunnerMainWin::on_addAudioFxButton_clicked()
{
	FxList *fxlist = mainapp->project->fxList;
	QString path = QFileDialog::getOpenFileName(this,tr("Choose Audio File")
												,mainapp->userSettings->pLastAudioFxImportPath);

	if (path.size()) {
		mainapp->userSettings->pLastAudioFxImportPath = path;
		fxlist->addFxAudioSimple(path);
	}
	fxListWidget->setFxList(fxlist);
}

void StageRunnerMainWin::on_actionSave_Project_as_triggered()
{
	QString path = QFileDialog::getSaveFileName(this,tr("Choose Project save path")
												,mainapp->userSettings->pLastProjectSavePath);
	if (path.size()) {
		if (path.right(4).toLower() != ".srp") {
			path += ".srp";
		}
		mainapp->userSettings->pLastProjectSavePath = path;
		mainapp->project->saveToFile(path);

		/// @todo Error handling
	}

}

void StageRunnerMainWin::on_actionLoad_Project_triggered()
{

	QString path = QFileDialog::getOpenFileName(this,tr("Choose Project")
												,mainapp->userSettings->pLastProjectLoadPath
												,tr("StageRunner projects (*.srp);;All files (*)"));
	if (path.size()) {
		clearProject();

		mainapp->userSettings->pLastProjectLoadPath = path;
		debug = 3;
		mainapp->project->loadFromFile(path);
		debug = 0;

		fxListWidget->setFxList(mainapp->project->fxList);

		/// @todo Error handling
	}
}

void StageRunnerMainWin::on_actionNew_Project_triggered()
{
	clearProject();

}

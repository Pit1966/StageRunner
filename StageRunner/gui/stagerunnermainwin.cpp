#include "stagerunnermainwin.h"
#include "system/commandsystem.h"
#include "appcontrol/appcentral.h"
#include "system/audiocontrol.h"
#include "fx/fxlist.h"
#include "appcontrol/project.h"
#include "appcontrol/usersettings.h"
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
	connect(fxListWidget,SIGNAL(dropEventReceived(QString,int)),this,SLOT(slot_addFxFile(QString,int)));
	connect(mainapp->project->fxList,SIGNAL(fxNextChanged(FxItem*)),fxListWidget,SLOT(selectFx(FxItem*)));


	// Audio Control Panel <-> Audio Control
	connect(mainapp->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),mainapp->unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(mainapp->unitAudio,SIGNAL(vuLevelChanged(int,int,int)),audioCtrlGroup,SLOT(setVuMeterLevel(int,int,int)));

	qApp->installEventFilter(this);

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
	shiftPressedFlag = false;
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
		copyProjectSettingsToGui();
	}
}

void StageRunnerMainWin::copyGuiSettingsToProject()
{
	mainapp->project->pAutoProceedSequence = fxListWidget->fxList()->autoProceedSequence();
}

void StageRunnerMainWin::copyProjectSettingsToGui()
{
	fxListWidget->setAutoProceedSequence( mainapp->project->pAutoProceedSequence );
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
		copyGuiSettingsToProject();
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
		copyProjectSettingsToGui();
		/// @todo Error handling
	}
}

void StageRunnerMainWin::on_actionNew_Project_triggered()
{
	clearProject();

}

bool StageRunnerMainWin::eventFilter(QObject *obj, QEvent *event)
{
	if (actionEdit_Mode->isChecked()) return qApp->eventFilter(obj, event);

	if (event->type() == 6) {
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		int key = ev->key();
		qDebug() << "Key pressed" << key << " " << "string:" << ev->text()<< obj->objectName();
		switch (key) {
		case Qt::Key_Shift:
			shiftPressedFlag = true;
			break;
		case Qt::Key_Space:
			mainapp->fadeoutAllFxAudio();
			break;
		case Qt::Key_Escape:
			mainapp->stopAllFxAudio();
			break;
		case Qt::Key_Delete:
			mainapp->project->fxList->deleteFx(mainapp->project->fxList->nextFx());
			break;

		default:
			{
				QString key = ev->text().toUpper();
				if (key.size()) {
					FxItem *fx = mainapp->project->fxList->getFxByKeyCode(key.at(0).unicode());
					if (fx) {
						mainapp->executeFxCmd(fx, CMD_AUDIO_START);
						// fxListWidget->selectFx(fx);
					}
				}
			}
			break;
		}

		return true;
	}
	if (event->type() == 7) {
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		if (ev->key() == Qt::Key_Shift) shiftPressedFlag = false;
		// qDebug() << "keyup" << ev->key() << " " << obj->objectName();
	}
	return qApp->eventFilter(obj, event);
}

void StageRunnerMainWin::slot_addFxFile(QString path, int pos)
{
	if (!path.startsWith("file://")) {
		DEBUGERROR("Add Drag'n'Drop File: %s not valid",path.toLatin1().data());
		return;
	}
	path = path.mid(7);

	if (path.size()) {
		qDebug() << path;
		FxList *fxlist = mainapp->project->fxList;
		fxlist->addFxAudioSimple(path,pos);
		fxListWidget->setFxList(fxlist);
	}
}


void StageRunnerMainWin::on_actionEdit_Mode_toggled(bool arg1)
{
	AppCentral::instance()->setEditMode(arg1);
}

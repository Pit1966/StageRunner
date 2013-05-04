#include "config.h"
#include "stagerunnermainwin.h"
#include "system/commandsystem.h"
#include "appcontrol/appcentral.h"
#include "system/audiocontrol.h"
#include "fx/fxlist.h"
#include "appcontrol/project.h"
#include "appcontrol/usersettings.h"
#include "thirdparty/widget/qsynthdialpeppinostyle.h"
#include "thirdparty/widget/qsynthdialclassicstyle.h"
#include "gui/setupwidget.h"
#include "gui/fxitempropertywidget.h"

#include <QFileDialog>
#include <QErrorMessage>


StageRunnerMainWin::StageRunnerMainWin(AppCentral *myapp) :
	QMainWindow(0)
{
	mainapp = myapp;

	init();



	setupUi(this);
	setup_gui_docks();

	updateButtonStyles();

	debugLevelSpin->setValue(debug);

	// For external access
	logWidget = logEdit;
}

StageRunnerMainWin::~StageRunnerMainWin()
{
	delete dialWidgetStyle;
	delete msg_dialog;
	delete fxitem_editor_dock;
}

void StageRunnerMainWin::initConnects()
{
	// FxListWidget (Liste der Effekte im Mainwin)
	connect(fxListWidget,SIGNAL(fxCmdActivated(FxItem*,CtrlCmd)),mainapp,SLOT(executeFxCmd(FxItem*,CtrlCmd)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),seqCtrlGroup,SLOT(setNextFx(FxItem*)));
	connect(fxListWidget,SIGNAL(dropEventReceived(QString,int)),this,SLOT(slot_addFxFile(QString,int)));
	connect(mainapp->project->fxList,SIGNAL(fxNextChanged(FxItem*)),fxListWidget,SLOT(selectFx(FxItem*)));

	// FxListWidget <-> Fx Editor (Dock Widget)
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),fxItemEditor,SLOT(setFxItem(FxItem*)));
	connect(fxItemEditor,SIGNAL(modified()),fxListWidget,SLOT(refreshList()));

	// Audio Control Panel <-> Audio Control
	connect(mainapp->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),mainapp->unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(mainapp->unitAudio,SIGNAL(vuLevelChanged(int,int,int)),audioCtrlGroup,SLOT(setVuMeterLevel(int,int,int)));

	// DMX Direct Control
	connect(dmxDirectWidget,SIGNAL(mixerMoved(int,int)),mainapp,SLOT(testSetDmxChannel(int,int)));

	// Global Info & Error Messaging
	connect(logThread,SIGNAL(infoMsgReceived(QString,QString)),this,SLOT(showInfoMsg(QString,QString)));
	connect(logThread,SIGNAL(errorMsgReceived(QString,QString)),this,SLOT(showErrorMsg(QString,QString)));

	qApp->installEventFilter(this);

}

void StageRunnerMainWin::setup_gui_docks()
{
	fxitem_editor_dock = new QDockWidget(this);
	fxItemEditor = new FxItemPropertyWidget(this);

	fxitem_editor_dock->setObjectName("Fx Editor");
	fxitem_editor_dock->setWindowTitle("Fx Editor");
	fxitem_editor_dock->setWidget(fxItemEditor);
	fxitem_editor_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea,fxitem_editor_dock);

}

void StageRunnerMainWin::restore_window()
{
	QSettings set;
	set.beginGroup("GuiSettings");
	if (set.contains("MainWinGeometry")) {
		restoreGeometry(set.value("MainWinGeometry").toByteArray());
		restoreState(set.value("MainWinDocks").toByteArray());
	}
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
	dialWidgetStyle = 0;

	msg_dialog = new QErrorMessage(this);
	fxitem_editor_dock = 0;
	fxItemEditor = 0;
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
	restore_window();

	if (mainapp->userSettings->pLastProjectLoadPath.size()) {
		mainapp->project->loadFromFile(mainapp->userSettings->pLastProjectLoadPath);
		fxListWidget->setFxList(mainapp->project->fxList);
		copyProjectSettingsToGui();
	}

	dmxDirectWidget->setMixerCount(24);
	dmxDirectWidget->setRange(0,MAX_DMX_FADER_RANGE);
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

void StageRunnerMainWin::on_actionSave_Project_triggered()
{
	Project * pro = mainapp->project;

	if (pro->curProjectFilePath.size()) {
		if (pro->saveToFile(pro->curProjectFilePath)) {
			POPUPINFOMSG(Q_FUNC_INFO,tr("Project successfully saved!"));
		}
	} else {
		on_actionSave_Project_as_triggered();
	}

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
		if (mainapp->project->saveToFile(path)) {
			POPUPINFOMSG(Q_FUNC_INFO,tr("Project successfully saved!"));
		} else {
			POPUPERRORMSG(Q_FUNC_INFO,tr("Could not save project!"));
		}
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

void StageRunnerMainWin::closeEvent(QCloseEvent *event)
{
	if (mainapp->project->isModified()) {
		int ret = QMessageBox::question(this,tr("Attention")
										,tr("Project is modified!\n\nDo you want to save it now?"));
		if (ret == QMessageBox::Yes) {
			on_actionSave_Project_triggered();
		}
	}
	QSettings set;
	set.beginGroup("GuiSettings");
	set.setValue("MainWinGeometry",saveGeometry());
	set.setValue("MainWinDocks",saveState());
	QMainWindow::closeEvent(event);
}




void StageRunnerMainWin::showInfoMsg(QString where, QString text)
{
	Q_UNUSED(where);
	QString msg = QString("<font color=#222222>%1</font><br><br>Reported from function:%2")
			.arg(text,where);
	msg_dialog->setStyleSheet("");
	msg_dialog->showMessage(text,where);
}

void StageRunnerMainWin::showErrorMsg(QString where, QString text)
{
	QString msg = QString("<font color=red>%1</font><br><br>Reported from function:%2")
			.arg(text,where);
	msg_dialog->setStyleSheet("color:red;");
	msg_dialog->showMessage(text,where);
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

void StageRunnerMainWin::on_actionSetup_triggered()
{
	SetupWidget setup(mainapp);
	setup.show();
	setup.exec();

}


void StageRunnerMainWin::on_actionExit_StageRunner_triggered()
{
	close();
}

void StageRunnerMainWin::on_addFxSceneButton_clicked()
{
	mainapp->project->fxList->addFxScene(12);
	fxListWidget->refreshList();
}

void StageRunnerMainWin::on_debugLevelSpin_valueChanged(int arg1)
{
	debug = arg1;
}

void StageRunnerMainWin::on_stopMainLoopButton_clicked()
{
	qDebug() << "Mainloop stopped";
	QTime wait;
	wait.start();
	while (wait.elapsed() < 3000) ;;
	qDebug() << "Mainloop is back";
}

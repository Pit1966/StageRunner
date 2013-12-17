#include "config.h"
#include "stagerunnermainwin.h"
#include "commandsystem.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "fxcontrol.h"
#include "execcenter.h"
#include "fxlist.h"
#include "project.h"
#include "usersettings.h"
#include "thirdparty/widget/qsynthdialpeppinostyle.h"
#include "thirdparty/widget/qsynthdialclassicstyle.h"
#include "setupwidget.h"
#include "fxitempropertywidget.h"
#include "scenestatuswidget.h"
#include "sequencestatuswidget.h"
#include "qtstatictools.h"
#include "style/lightdeskstyle.h"
#include "fxitem.h"
#include "fxplaylistitem.h"
#include "executer.h"

#include <QFileDialog>
#include <QErrorMessage>


StageRunnerMainWin::StageRunnerMainWin(AppCentral *myapp) :
	QMainWindow(0)
{
	appCentral = myapp;

	init();

	setupUi(this);
	setObjectName("StageRunnerMainwin");
	setup_gui_docks();

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
	// AppCentral <-> Mainwin
	connect(appCentral,SIGNAL(editModeChanged(bool)),actionEdit_Mode,SLOT(setChecked(bool)));
	connect(appCentral,SIGNAL(inputAssignModeChanged(bool)),actionInput_Assign_Mode,SLOT(setChecked(bool)));

	// Project FxListWidget (Liste der Effekte im Mainwin)
	connect(fxListWidget,SIGNAL(fxCmdActivated(FxItem*,CtrlCmd,Executer*)),appCentral,SLOT(executeFxCmd(FxItem*,CtrlCmd,Executer*)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),appCentral,SLOT(storeSelectedFxListWidgetFx(FxItem*)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),seqCtrlGroup,SLOT(setNextFx(FxItem*)));
	connect(appCentral->project->fxList,SIGNAL(fxNextChanged(FxItem*)),fxListWidget,SLOT(selectFx(FxItem*)));
	connect(appCentral,SIGNAL(editModeChanged(bool)),fxListWidget,SLOT(setEditable(bool)));

	// Project FxListWidget <-> Fx Editor (Dock Widget)
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),fxItemEditor,SLOT(setFxItem(FxItem*)));
	connect(fxListWidget,SIGNAL(fxItemSelectedForEdit(FxItem*)),this,SLOT(openFxPropertyEditor(FxItem*)));
	connect(fxItemEditor,SIGNAL(modified()),fxListWidget,SLOT(refreshList()));
	connect(fxListWidget,SIGNAL(editableChanged(bool)),appCentral,SLOT(setEditMode(bool)));

	// Audio Control Panel <-> Audio Control
	connect(appCentral->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),appCentral->unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(appCentral->unitAudio,SIGNAL(vuLevelChanged(int,int,int)),audioCtrlGroup,SLOT(setVuMeterLevel(int,int,int)));

	// Light Control -> SceneStatusWidget
	connect(appCentral->unitLight,SIGNAL(sceneChanged(FxSceneItem*)),sceneStatusDisplay,SLOT(propagateScene(FxSceneItem*)));

	// Light Control -> Project FxListWidget
	connect(appCentral->unitLight,SIGNAL(sceneChanged(FxSceneItem*)),fxListWidget,SLOT(propagateSceneStatus(FxSceneItem*)));
	connect(appCentral->unitLight,SIGNAL(sceneFadeChanged(FxSceneItem*,int,int)),fxListWidget,SLOT(propagateSceneFadeProgress(FxSceneItem*,int,int)));

	// Light Control -> Audio Control
	connect(appCentral->unitLight,SIGNAL(audioSlotVolChanged(int,int)),appCentral->unitAudio,SLOT(setVolumeFromDmxLevel(int,int)));

	// Audio Control -> Project FxListWidget
	connect(appCentral->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),fxListWidget,SLOT(propagateAudioStatus(AudioCtrlMsg)));

	// Fx Control -> SequenceStatusWidget
	connect(appCentral->unitFx,SIGNAL(executerChanged(Executer*)),seqStatusDisplay,SLOT(propagateExecuter(Executer*)));

	// ExecCenter -> SequenceStatusWidget
	connect(appCentral->execCenter,SIGNAL(executerCreated(Executer*)),seqStatusDisplay,SLOT(insertExecuter(Executer*)));
	connect(appCentral->execCenter,SIGNAL(executerDeleted(Executer*)),seqStatusDisplay,SLOT(scratchExecuter(Executer*)));
	connect(appCentral->execCenter,SIGNAL(executerChanged(Executer*)),seqStatusDisplay,SLOT(propagateExecuter(Executer*)));

	// Global Info & Error Messaging
	connect(logThread,SIGNAL(infoMsgReceived(QString,QString)),this,SLOT(showInfoMsg(QString,QString)));
	connect(logThread,SIGNAL(errorMsgReceived(QString,QString)),this,SLOT(showErrorMsg(QString,QString)));

	qApp->installEventFilter(this);

}

void StageRunnerMainWin::setup_gui_docks()
{
	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
	setDockNestingEnabled(true);

	fxitem_editor_dock = new QDockWidget(this);
	fxItemEditor = new FxItemPropertyWidget();
	connect(appCentral,SIGNAL(editModeChanged(bool)),fxItemEditor,SLOT(setEditable(bool)));
	fxItemEditor->setEditable(false);

	fxitem_editor_dock->setObjectName("Fx Editor");
	fxitem_editor_dock->setWindowTitle("Fx Editor");
	fxitem_editor_dock->setWidget(fxItemEditor);
	fxitem_editor_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::TopDockWidgetArea,fxitem_editor_dock);

	scene_status_dock = new QDockWidget(this);
	sceneStatusDisplay = new SceneStatusWidget();
	scene_status_dock->setObjectName("Scene Status Display");
	scene_status_dock->setWindowTitle("Scene Status Display");
	scene_status_dock->setWidget(sceneStatusDisplay);
	scene_status_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,scene_status_dock);

	sequence_status_dock = new QDockWidget(this);
	seqStatusDisplay = new SequenceStatusWidget();
	sequence_status_dock->setObjectName("Sequence Status Display");
	sequence_status_dock->setWindowTitle("Sequence Status Display");
	sequence_status_dock->setWidget(seqStatusDisplay);
	sequence_status_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,sequence_status_dock);

}

void StageRunnerMainWin::restore_window()
{
	QSettings set;
	set.beginGroup("GuiSettings");
	if (set.contains("MainWinGeometry")) {
		restoreGeometry(set.value("MainWinGeometry").toByteArray());
		restoreState(set.value("MainWinDocks").toByteArray());
		resize(set.value("MainWinSize").toSize());
	}
}

void StageRunnerMainWin::updateButtonStyles(QString style)
{
	LOGTEXT(tr("Set dial knob style to '%1'").arg(style));

	// Update Style of Dial knobs
	if (dialWidgetStyle) {
		delete dialWidgetStyle;
		dialWidgetStyle = 0;
	}

	if (style == "" || style == "QSynth Dial Classic") {
		dialWidgetStyle = new qsynthDialClassicStyle();
	}
	else if (style == "QSynth Dial Peppino") {
		dialWidgetStyle = new qsynthDialPeppinoStyle();
	}

	// if (!dialWidgetStyle) return;

	QList<qsynthKnob *> all_dials = findChildren<qsynthKnob *>();
	foreach(qsynthKnob* dial, all_dials) {
		dial->setStyle(dialWidgetStyle);
		dial->setDialMode(qsynthKnob::LinearMode);
	}
}

void StageRunnerMainWin::openFxPropertyEditor(FxItem *item)
{
	fxitem_editor_dock->show();
	fxItemEditor->setFxItem(item);
	fxItemEditor->setEditable(true,true);
}

/**
 * @brief Clear Project
 *
 * Generates a new empty project in main application
 */
void StageRunnerMainWin::clearProject()
{
	// Clear Project and project member
	appCentral->clearProject();

	// Clear Fx list in GUI
	fxListWidget->setFxList(appCentral->project->fxList);
}

void StageRunnerMainWin::init()
{
	activeKeyModifiers = 0;
	dialWidgetStyle = 0;

	msg_dialog = new QErrorMessage(this);
	fxitem_editor_dock = 0;
	fxItemEditor = 0;
	scene_status_dock = 0;
	sceneStatusDisplay = 0;
	sequence_status_dock = 0;
	seqStatusDisplay = 0;
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

	if (appCentral->userSettings->pLastProjectLoadPath.size()) {
		appCentral->project->loadFromFile(appCentral->userSettings->pLastProjectLoadPath);
		appCentral->project->postLoadProcessFxList();
		fxListWidget->setFxList(appCentral->project->fxList);
		copyProjectSettingsToGui();
	}
}

void StageRunnerMainWin::copyGuiSettingsToProject()
{
	appCentral->project->pAutoProceedSequence = fxListWidget->fxList()->autoProceedSequence();
}

void StageRunnerMainWin::copyProjectSettingsToGui()
{
	fxListWidget->setAutoProceedSequence( appCentral->project->pAutoProceedSequence );
}

void StageRunnerMainWin::setApplicationGuiStyle(QString style)
{
	LOGTEXT(tr("Set Application GUI style to '%1'").arg(style));
	if (style == "" || style == "LightDesk") {
		QApplication::setStyle(new LightDeskStyle);
	}
	else if (style == "Default") {
		QApplication::setStyle(0);
	}
	else {
		QApplication::setStyle(QStyleFactory::create(style));
	}

}

void StageRunnerMainWin::on_addAudioFxButton_clicked()
{
	FxList *fxlist = appCentral->project->fxList;
	appCentral->addFxAudioDialog(fxlist,this);
	fxListWidget->setFxList(fxlist);
}

void StageRunnerMainWin::on_actionSave_Project_triggered()
{
	Project * pro = appCentral->project;

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
												,appCentral->userSettings->pLastProjectSavePath);
	if (path.size()) {
		if (path.right(4).toLower() != ".srp") {
			path += ".srp";
		}
		appCentral->userSettings->pLastProjectSavePath = path;
		copyGuiSettingsToProject();
		if (appCentral->project->saveToFile(path)) {
			POPUPINFOMSG(Q_FUNC_INFO,tr("Project successfully saved!"));
		} else {
			POPUPERRORMSG(Q_FUNC_INFO,tr("Could not save project!"));
		}
	}

}

void StageRunnerMainWin::on_actionLoad_Project_triggered()
{

	QString path = QFileDialog::getOpenFileName(this,tr("Choose Project")
												,appCentral->userSettings->pLastProjectLoadPath
												,tr("StageRunner projects (*.srp);;All files (*)"));
	if (path.size()) {
		clearProject();

		appCentral->userSettings->pLastProjectLoadPath = path;
		appCentral->project->loadFromFile(path);
		appCentral->project->postLoadProcessFxList();

		fxListWidget->setFxList(appCentral->project->fxList);
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

	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease ) {

		if (actionEdit_Mode->isChecked()) return qApp->eventFilter(obj, event);

		QString widname;
		if (QApplication::activeWindow()) {
			widname = QApplication::activeWindow()->objectName();
		}
		QString focusname;
		QString parentfocusname;
		if (QApplication::focusWidget()) {
			focusname = QApplication::focusWidget()->objectName();
			QWidget *window = QApplication::focusWidget()->window();
			if (window) {
				parentfocusname = window->objectName();
			}
		}
		// qDebug() << "focuswidget" << widname << "parentwid" << parentfocusname << "focusname" << focusname;

		if (widname != "StageRunnerMainwin"
				|| focusname == "commentEdit"
				|| fxItemEditor->isOnceEdit())
		{
			return qApp->eventFilter(obj, event);
		}
	}


	if (event->type() == 6) {
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);
		int key = ev->key();
		switch (key) {
		case Qt::Key_Shift:
			activeKeyModifiers |= Qt::SHIFT;
			break;
		case Qt::Key_Control:
			activeKeyModifiers |= Qt::CTRL;
			break;
		case Qt::Key_Space:
			appCentral->fadeoutAllFxAudio();
			break;
		case Qt::Key_Escape:
			appCentral->stopAllFxAudio();
			break;
		case Qt::Key_Delete:
			appCentral->project->fxList->deleteFx(appCentral->project->fxList->nextFx());
			break;
		case Qt::Key_Backspace:
			appCentral->lightBlack(0);
			break;

		default:
			if (key) {
				QList<FxItem *>fxlist = appCentral->project->fxList->getFxListByKeyCode(key + activeKeyModifiers);
				if (fxlist.size()) {
					for (int t=0; t<fxlist.size(); t++) {
						appCentral->executeFxCmd(fxlist.at(t), CMD_FX_START, 0);
					}
					// fxListWidget->selectFx(fx);
				}
			}
			break;
		}

		DEBUGTEXT("Key pressed: #%d -> '%s'"
				  ,key, QtStaticTools::keyToString(key,activeKeyModifiers).toLatin1().data());

		return true;
	}
	if (event->type() == 7) {
		QKeyEvent *ev = static_cast<QKeyEvent *>(event);

		switch (ev->key()) {
		case Qt::Key_Shift:
			activeKeyModifiers &= ~Qt::SHIFT;
			break;
		case Qt::Key_Control:
			activeKeyModifiers &= ~Qt::CTRL;
			break;
		default:
			break;
		}
	}

	return qApp->eventFilter(obj, event);
}

void StageRunnerMainWin::closeEvent(QCloseEvent *event)
{
	if (appCentral->project->isModified()) {
		int ret = QMessageBox::question(this,tr("Attention")
										,tr("Project is modified!\n\nDo you want to save it now?"));
		if (ret == QMessageBox::Yes) {
			on_actionSave_Project_triggered();
		}
	}

	if (appCentral->unitAudio->fadeoutAllFxAudio(1000)) {
		hide();
		QTime wait;
		wait.start();
		while (wait.elapsed() < 1500) QApplication::processEvents();
	}


	QSettings set;
	set.beginGroup("GuiSettings");
	set.setValue("MainWinGeometry",saveGeometry());
	set.setValue("MainWinSize",size());
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


void StageRunnerMainWin::on_actionEdit_Mode_toggled(bool arg1)
{
	appCentral->setEditMode(arg1);
}

void StageRunnerMainWin::on_actionSetup_triggered()
{
	SetupWidget setup(appCentral);
	connect(&setup,SIGNAL(applicationStyleChanged(QString)),this,SLOT(setApplicationGuiStyle(QString)));
	connect(&setup,SIGNAL(dialKnobStyleChanged(QString)),this,SLOT(updateButtonStyles(QString)));


	setup.show();
	setup.exec();

}


void StageRunnerMainWin::on_actionExit_StageRunner_triggered()
{
	close();
}

void StageRunnerMainWin::on_addFxSceneButton_clicked()
{
	appCentral->project->fxList->addFxScene(12);
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

void StageRunnerMainWin::on_actionDMX_Input_triggered()
{
	QWidget * mon = reinterpret_cast<QWidget *>(appCentral->openDmxInMonitor(0));
	if (mon) {
		mon->show();
		mon->raise();
	} else {
		QMessageBox::warning(this,tr("System message")
							 ,tr("No input universe detected"));
	}
}

void StageRunnerMainWin::on_actionDMX_Output_triggered()
{
	QWidget * mon = reinterpret_cast<QWidget *>(appCentral->openDmxOutMonitor(0));
	if (mon) {
		mon->show();
		mon->raise();
	} else {
		QMessageBox::warning(this,tr("System message")
							 ,tr("No output universe detected"));
	}
}

void StageRunnerMainWin::on_actionInput_Assign_Mode_triggered(bool checked)
{
	appCentral->setInputAssignMode(checked);
}

void StageRunnerMainWin::on_cloneSelectedSceneButton_clicked()
{
	appCentral->project->fxList->cloneSelectedSceneItem();
	fxListWidget->refreshList();

}

void StageRunnerMainWin::on_addAudioPlayListButton_clicked()
{
	appCentral->project->fxList->addFxAudioPlayList();
	fxListWidget->refreshList();
}

void StageRunnerMainWin::on_addFxSeqButton_clicked()
{
	appCentral->project->fxList->addFxSequence();
	fxListWidget->refreshList();
}

void StageRunnerMainWin::on_addAudioTrackButton_clicked()
{
	FxItem *fx = appCentral->project->fxList->nextFx();
	if (!fx || fx->fxType() != FX_AUDIO_PLAYLIST) return;

	QString path = QFileDialog::getOpenFileName(this,tr("Choose Audio Track")
												,appCentral->userSettings->pLastAudioTrackImportPath);
	if (path.size()) {
		appCentral->userSettings->pLastAudioTrackImportPath = path;
		static_cast<FxPlayListItem*>(fx)->addAudioTrack(path);
	}

}

void StageRunnerMainWin::on_actionInitialize_plugins_DMX_triggered()
{
	appCentral->reOpenPlugins();

}

void StageRunnerMainWin::on_actionInfo_triggered()
{
	QString msg = QString("%1 %2\n\n(C) 2013 Stonechip Entertainment").arg(APP_NAME).arg(APP_VERSION);
	QMessageBox::information(this,tr("About Info"),msg);

}


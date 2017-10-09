#include "config.h"
#include "version.h"
#include "log.h"
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
#include "fxsceneitem.h"
#include "fxseqitem.h"
#include "fxscriptitem.h"
#include "fxplaylistitem.h"
#include "executer.h"
#include "fxlistwidget.h"
#include "scenedeskwidget.h"
#include "fxitemobj.h"
#include "fxlistvarset.h"
#include "customwidget/psvideowidget.h"
#include "dmxuniverseproperty.h"
#include "fxscriptwidget.h"
// #include "configrev.h"

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

	fxListWidget->setFxList(appCentral->project->mainFxList());

	debugLevelSpin->setValue(debug);

	actionExperimental_audio_mode->setChecked(appCentral->userSettings->pAltAudioEngine);
	actionEnable_audio_FFT->setChecked(appCentral->userSettings->pFFTAudioMask > 0);

	// For external access
	logWidget = logEdit;
	if (myapp->mainWinObj) {
		DEBUGERROR("There seems to be more than one StageRunnerMainWin instance!");
	}
	myapp->mainWinObj = this;
}

StageRunnerMainWin::~ StageRunnerMainWin()
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
	connect(fxListWidget,SIGNAL(fxItemSelectedInChildFxListWidget(FxItem*)),appCentral,SLOT(setGlobalSelectedFx(FxItem*)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),appCentral,SLOT(setGlobalSelectedFx(FxItem*)));
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),seqCtrlGroup,SLOT(setNextFx(FxItem*)));
	connect(appCentral->project->mainFxList(),SIGNAL(fxNextChanged(FxItem*)),fxListWidget,SLOT(selectFx(FxItem*)));
	connect(appCentral->project->mainFxList(),SIGNAL(fxCurrentChanged(FxItem*,FxItem*)),fxListWidget,SLOT(setCurrentFx(FxItem*,FxItem*)));

	connect(appCentral,SIGNAL(editModeChanged(bool)),fxListWidget,SLOT(setEditable(bool)));

	// Project FxListWidget <-> Fx Editor (Dock Widget)
	connect(fxListWidget,SIGNAL(fxItemSelected(FxItem*)),fxItemEditor,SLOT(setFxItem(FxItem*)));
	connect(fxListWidget,SIGNAL(fxItemSelectedForEdit(FxItem*)),this,SLOT(openFxPropertyEditor(FxItem*)));
	connect(fxListWidget,SIGNAL(fxTypeColumnDoubleClicked(FxItem*)),this,SLOT(openFxItemPanel(FxItem*)));
	connect(fxItemEditor,SIGNAL(modified(FxItem*)),fxListWidget,SLOT(refreshFxItem(FxItem*)));
	connect(fxListWidget,SIGNAL(editableChanged(bool)),appCentral,SLOT(setEditMode(bool)));

	// Audio Control Panel <-> Audio Control
	connect(appCentral->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AudioCtrlMsg)),appCentral->unitAudio,SLOT(audioCtrlReceiver(AudioCtrlMsg)));
	connect(appCentral->unitAudio,SIGNAL(vuLevelChanged(int,qreal,qreal)),audioCtrlGroup,SLOT(setVuMeterLevel(int,qreal,qreal)));
	connect(appCentral->unitAudio,SIGNAL(fftSpectrumChanged(int, FrqSpectrum *)),audioCtrlGroup,SLOT(setFFTSpectrum(int, FrqSpectrum *)));

	// Light Control -> SceneStatusWidget
	connect(appCentral->unitLight,SIGNAL(sceneChanged(FxSceneItem*)),sceneStatusDisplay,SLOT(propagateScene(FxSceneItem*)));
	connect(appCentral->unitLight,SIGNAL(sceneFadeChanged(FxSceneItem*,int,int)),sceneStatusDisplay,SLOT(propagateSceneFade(FxSceneItem*,int,int)));

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
	fxItemEditor->closeButton->hide();

	fxitem_editor_dock->setObjectName("Fx Editor");
	fxitem_editor_dock->setWindowTitle("Fx Editor");
	fxitem_editor_dock->setWidget(fxItemEditor);
	fxitem_editor_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,fxitem_editor_dock);

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

	template_dock = new QDockWidget(this);
	templateWidget =new FxListWidget();
	template_dock->setObjectName("Fx Templates");
	template_dock->setWindowTitle("Fx Templates");
	template_dock->setWidget(templateWidget);
	template_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,template_dock);
	templateWidget->setFxList(appCentral->templateFxList->fxList());
	templateWidget->setAutoProceedCheckVisible(false);
	connect(templateWidget,SIGNAL(fxTypeColumnDoubleClicked(FxItem*)),this,SLOT(openFxItemPanel(FxItem*)));

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
	if (appCentral->project->isModified()) {
		int ret = QMessageBox::question(this,tr("Attention")
										,tr("The current project is modified!\n\nDo you want to save it now?")
										,QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (ret == QMessageBox::Yes) {
			on_actionSave_Project_triggered();
		}
		else if (ret == QMessageBox::Cancel) {
			return;
		}
	}

	// Clear Project and project member
	appCentral->clearProject();
	setProjectName("");

	// Clear Fx list in GUI
	fxListWidget->setFxList(appCentral->project->mainFxList());
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
		Project *pro = appCentral->project;
		QString propath = appCentral->userSettings->pLastProjectLoadPath;
		if (pro->loadFromFile(propath)) {
			pro->postLoadProcessFxList();
			pro->postLoadResetFxScenes();

			fxListWidget->setFxList(pro->mainFxList());
			copyProjectSettingsToGui();
			setProjectName(propath);
		} else {
			QMessageBox::critical(this,tr("Load error")
								  ,tr("An error occured while loading file:\n\n%1\nLine %2:'%3'")
								  .arg(appCentral->userSettings->pLastProjectLoadPath)
								  .arg(pro->loadErrorLineNumber).arg(pro->loadErrorLineString));
			setProjectName("");
		}
	}

	// Test Audio Features
	appCentral->unitAudio->getAudioDevices();

	// Set Audio Defaults
	if (appCentral->unitAudio->isValid()) {
		appCentral->unitAudio->setFFTAudioChannelFromMask(appCentral->userSettings->pFFTAudioMask);
		actionEnable_audio_FFT->setChecked(appCentral->userSettings->pFFTAudioMask == 15);
	}

	// Copy User Settings to GUI
	applyUserSettingsToGui(appCentral->userSettings);

	// Load Default Template FxList
	if (QFile::exists(appCentral->userSettings->pFxTemplatePath))
		on_loadTemplatesButton_clicked();

}

void StageRunnerMainWin::copyGuiSettingsToProject()
{
	appCentral->project->pAutoProceedSequence = fxListWidget->fxList()->autoProceedSequence();
}

void StageRunnerMainWin::copyProjectSettingsToGui()
{
	fxListWidget->setAutoProceedSequence( appCentral->project->pAutoProceedSequence );
}

void StageRunnerMainWin::openFxItemPanel(FxItem *fx)
{
	switch (fx->fxType()) {
	case FX_SCENE:
		openFxSceneItemPanel(static_cast<FxSceneItem*>(fx));
		break;
	case FX_AUDIO:
		break;
	case FX_AUDIO_PLAYLIST:
		openFxPlayListItemPanel(static_cast<FxPlayListItem*>(fx));
		break;
	case FX_SEQUENCE:
		openFxSeqItemPanel(static_cast<FxSeqItem*>(fx));
		break;
	case FX_SCRIPT:
		openFxScriptPanel(static_cast<FxScriptItem*>(fx));
		break;
	}
}

void StageRunnerMainWin::applyUserSettingsToGui(UserSettings *set)
{
	audioCtrlGroup->setFFTGraphVisibleFromMask(set->pFFTAudioMask);
	audioCtrlGroup->setVolumeDialVisibleFromMask(set->pVolumeDialMask);
}

void StageRunnerMainWin::openFxSceneItemPanel(FxSceneItem *fx)
{
	SceneDeskWidget *desk = SceneDeskWidget::openSceneDesk(fx);

	if (desk) {
		FxListWidget *parentWid = FxListWidget::findParentFxListWidget(fx);
		if (parentWid) {
			connect(desk,SIGNAL(modified()),parentWid,SLOT(refreshList()));
		}
		desk->show();
	}
}

void StageRunnerMainWin::openFxPlayListItemPanel(FxPlayListItem *fx)
{
	bool new_created;

	FxListWidget *playlistwid = FxListWidget::getCreateFxListWidget(fx->fxPlayList, fx, &new_created);

	// Let us look if an executer is running on this FxPlayListItem
	if (new_created) {
		playlistwid->generateDropAllowedFxTypeList(fx);
		FxListExecuter *exe = AppCentral::instance()->execCenter->findFxListExecuter(fx);
		if (exe) {
			playlistwid->markFx(exe->currentFx());
			playlistwid->selectFx(exe->nextFx());
			connect(exe,SIGNAL(currentFxChanged(FxItem*)),playlistwid,SLOT(markFx(FxItem*)));
			connect(exe,SIGNAL(nextFxChanged(FxItem*)),playlistwid,SLOT(selectFx(FxItem*)));
			connect(AppCentral::instance()->unitAudio,SIGNAL(audioCtrlMsgEmitted(AudioCtrlMsg)),playlistwid,SLOT(propagateAudioStatus(AudioCtrlMsg)));
			connect(playlistwid,SIGNAL(fxItemSelected(FxItem*)),exe,SLOT(selectNextFx(FxItem*)));
			connect(playlistwid,SIGNAL(fxTypeColumnDoubleClicked(FxItem*)),this,SLOT(openFxItemPanel(FxItem*)));

		}
		// This connect is for starting / forwarding the playback by double click on a FxAudio in the PlayList
		connect(playlistwid,SIGNAL(fxCmdActivated(FxItem*,CtrlCmd,Executer*))
				,fx->connector(),SLOT(playFxPlayList(FxItem*,CtrlCmd,Executer*)),Qt::QueuedConnection);

		// connect signals from new FxListWidget to parent FxListWidget
		FxListWidget *parentFxListWidget = FxListWidget::findParentFxListWidget(fx);
		if (parentFxListWidget) {
			connect(playlistwid,SIGNAL(fxItemSelected(FxItem*)),parentFxListWidget,SLOT(onFxItemSelectedInChildWidget(FxItem*)));
		}

	} else {
		playlistwid->refreshList();
	}
	if (playlistwid->isHidden()) {
		playlistwid->show();
		playlistwid->setOriginFx(fx);
	} else {
		playlistwid->raise();
	}
}

void StageRunnerMainWin::openFxSeqItemPanel(FxSeqItem *fx)
{
	bool new_created;

	FxListWidget *sequencewid = FxListWidget::getCreateFxListWidget(fx->seqList, fx, &new_created);

	// Let us look if an executer is running on this FxSequenceItem
	if (new_created) {
		sequencewid->generateDropAllowedFxTypeList(fx);

		connect(sequencewid,SIGNAL(fxTypeColumnDoubleClicked(FxItem*)),this,SLOT(openFxItemPanel(FxItem*)));
		FxListExecuter *exe = AppCentral::instance()->execCenter->findFxListExecuter(fx);
		if (exe) {
			sequencewid->markFx(exe->currentFx());
			sequencewid->selectFx(exe->nextFx());
		}

		// connect signals from new FxListWidget to parent FxListWidget
		FxListWidget *parentFxListWidget = FxListWidget::findParentFxListWidget(fx);
		if (parentFxListWidget) {
			connect(sequencewid,SIGNAL(fxItemSelected(FxItem*)),parentFxListWidget,SLOT(onFxItemSelectedInChildWidget(FxItem*)));
		}

		// This connect is for starting / forwarding the sequence by double click on an item in the sequence list
//		connect(sequencewid,SIGNAL(fxCmdActivated(FxItem*,CtrlCmd,Executer*))
//				,fx->connector(),SLOT(playFxPlayList(FxItem*,CtrlCmd,Executer*)),Qt::QueuedConnection);
	} else {
		sequencewid->refreshList();
	}

	if (sequencewid->isHidden()) {
		sequencewid->show();
		sequencewid->setOriginFx(fx);
	} else {
		sequencewid->raise();
	}
}

void StageRunnerMainWin::openFxScriptPanel(FxScriptItem *fx)
{
	FxScriptWidget *editor = FxScriptWidget::openFxScriptPanel(fx);

	if (editor) {
		FxListWidget *parentWid = FxListWidget::findParentFxListWidget(fx);
		if (parentWid) {
			connect(editor,SIGNAL(modified()),parentWid,SLOT(refreshList()));
		}
		editor->show();
	}
}

void StageRunnerMainWin::setProjectName(const QString &path)
{
	QString title = QString("%1 %2 ").arg(APP_NAME,APP_VERSION);
	if (path.isEmpty()) {
		title += APP_PRODUCER;
		appCentral->project->curProjectFilePath.clear();
	} else {
		title += "- ";
		title += QFileInfo(path).baseName();
		appCentral->project->curProjectFilePath = path;
	}
	QWidget::setWindowTitle(title);
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
	FxList *fxlist = appCentral->project->mainFxList();
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
		appCentral->userSettings->pLastProjectLoadPath = path;
		copyGuiSettingsToProject();
		if (appCentral->project->saveToFile(path)) {
			setProjectName(path);
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
		Project *pro = appCentral->project;
		if ( !pro->loadFromFile(path) ) {
			QMessageBox::critical(this,tr("Load error")
								  ,tr("An error occured while loading file:\n\n%1\n\nLine %2:'%3'")
								  .arg(appCentral->userSettings->pLastProjectLoadPath)
								  .arg(pro->loadErrorLineNumber).arg(pro->loadErrorLineString));
			setProjectName("");
		} else {
			pro->postLoadProcessFxList();
			pro->postLoadResetFxScenes();

			fxListWidget->setFxList(pro->mainFxList());
			copyProjectSettingsToGui();
			setProjectName(path);
		}
	}
}

void StageRunnerMainWin::on_actionNew_Project_triggered()
{
	clearProject();

}

bool StageRunnerMainWin::eventFilter(QObject *obj, QEvent *event)
{

	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease ) {
		// qDebug("KeyEvent %s",obj->objectName().toLocal8Bit().data());

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

		bool isNormal = true;
		if (widname == "FxListWidget") {
			FxListWidget *wid = qobject_cast<FxListWidget*>(obj);
			if (wid && !wid->isEditable()) {
				isNormal = false;
			}
		}
		else if (widname == "StageRunnerMainwin") {
			if (focusname != "commentEdit"
					&& !fxItemEditor->isOnceEdit())
			{
				isNormal = false;
			}
		}


		if (isNormal) {
			return qApp->eventFilter(obj, event);
		}
	}


	if (event->type() == QEvent::KeyPress) {
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
			appCentral->unitAudio->storeCurrentSeekPositions();
			appCentral->fadeoutAllFxAudio();
			break;
		case Qt::Key_Escape:
			appCentral->unitAudio->storeCurrentSeekPositions();
			appCentral->stopAllFxAudio();
			break;
		case Qt::Key_Delete:
			appCentral->project->mainFxList()->deleteFx(appCentral->project->mainFxList()->nextFx());
			break;
		case Qt::Key_Backspace:
			appCentral->unitFx->stopAllFxSequences();
			appCentral->lightBlack(0);
			appCentral->videoBlack(0);
			break;

		default:
			if (key) {
				// Do not start FX if CTRL key is pressed.
				if ((activeKeyModifiers & Qt::CTRL)) {
					return qApp->eventFilter(obj, event);
				}

				QList<FxItem *>fxlist = appCentral->project->mainFxList()->getFxListByKeyCode(key + activeKeyModifiers);
				if (fxlist.size()) {
					for (int t=0; t<fxlist.size(); t++) {
						appCentral->executeFxCmd(fxlist.at(t), CMD_FX_START, 0);
					}
					// fxListWidget->selectFx(fx);
				}
			}
			break;
		}

		if (debug) DEBUGTEXT("Key pressed: #%d -> '%s'"
				  ,key, QtStaticTools::keyToString(key,activeKeyModifiers).toLatin1().data());

		return true;
	}

	if (event->type() == QEvent::KeyRelease) {
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

	if (appCentral->templateFxList->isModified()) {
		int ret = QMessageBox::question(this,tr("Attention")
										,tr("Your FX template list is modified!\n\nDo you want to save it now?"));
		if (ret == QMessageBox::Yes) {
			on_saveTemplatesButton_clicked();
		}
	}

	if (appCentral->project->curProjectFilePath.isEmpty()) {
//		appCentral->userSettings->pLastProjectLoadPath = "";
	}

	// This stops and removes all running sequences
	appCentral->stopAllSequencesAndPlaylists();

	if (appCentral->unitAudio->isValid()) {
		if (appCentral->unitAudio->fadeoutAllFxAudio(1000)) {
			hide();
			QTime wait;
			wait.start();
			while (wait.elapsed() < 1500) QApplication::processEvents();
		}
	}

	if (appCentral->unitAudio->videoWidget())
		appCentral->unitAudio->videoWidget()->close();


	QSettings set;
	set.beginGroup("GuiSettings");
	set.setValue("MainWinGeometry",saveGeometry());
	set.setValue("MainWinSize",size());
	set.setValue("MainWinDocks",saveState());
	set.endGroup();

	QMainWindow::closeEvent(event);

	FxListWidget::destroyAllFxListWidgets();
	SceneDeskWidget::destroyAllSceneDesks();

}

void StageRunnerMainWin::showInfoMsg(QString where, QString text)
{
	Q_UNUSED(where);
	QString msg = QString("<font color=#222222>%1</font><br><br>Reported from function:%2")
			.arg(text,where);
	msg_dialog->setStyleSheet("");
	msg_dialog->showMessage(text,where);
	msg_dialog->resize(600,200);
	msg_dialog->setWindowTitle(tr("Information"));
}

void StageRunnerMainWin::showErrorMsg(QString where, QString text)
{
	QString msg = QString("<font color=red>%1</font><br><br>Reported from function:%2")
			.arg(text,where);
	msg_dialog->setStyleSheet("color:red;");
	msg_dialog->showMessage(text,where);
	msg_dialog->resize(600,200);
	msg_dialog->setWindowTitle(tr("StageRunner error message"));
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
	connect(&setup,SIGNAL(setupChanged(UserSettings *)),this,SLOT(applyUserSettingsToGui(UserSettings*)));


	setup.show();
	setup.exec();

}


void StageRunnerMainWin::on_actionExit_StageRunner_triggered()
{
	close();
}

void StageRunnerMainWin::on_addFxSceneButton_clicked()
{
	appCentral->project->mainFxList()->addFxScene(12);
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
	appCentral->project->mainFxList()->cloneSelectedSceneItem();
	fxListWidget->refreshList();

}

void StageRunnerMainWin::on_addAudioPlayListButton_clicked()
{
	appCentral->project->mainFxList()->addFxAudioPlayList();
	fxListWidget->refreshList();
}

void StageRunnerMainWin::on_addFxSeqButton_clicked()
{
	appCentral->project->mainFxList()->addFxSequence();
	fxListWidget->refreshList();
}

void StageRunnerMainWin::on_addAudioTrackButton_clicked()
{
	FxItem *fx = appCentral->project->mainFxList()->nextFx();
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
	QString msg = QString("%1 %2\n%3\n\n%4\n\n").arg(APP_NAME).arg(APP_VERSION).arg(APP_MILESTONE).arg(APP_PRODUCER);
	msg += QString("Qt version (binary): %1\nQt version (runtime): %2\n\n").arg(QT_VERSION_STR).arg(qVersion());

#ifdef unix
	msg += QString("Git timestamp: %1\n").arg(QDateTime::fromTime_t(GIT_APP_TIME).toString());
	msg += QString("Build date: %1\n").arg(BUILD_DATE);
	msg += QString("Build path: %1\n").arg(BUILD_PATH);
	QMessageBox::information(this,tr("About Info"),msg);
#endif

}


void StageRunnerMainWin::on_actionExperimental_audio_mode_triggered(bool checked)
{
	appCentral->setExperimentalAudio(checked);
	appCentral->unitAudio->reCreateMediaPlayerInstances();
}

void StageRunnerMainWin::on_actionOpen_FxItem_triggered()
{
	if (appCentral->globalSelectedFx())
		openFxItemPanel(appCentral->globalSelectedFx());
}

void StageRunnerMainWin::on_saveTemplatesButton_clicked()
{
	appCentral->templateFxList->fileSave(appCentral->userSettings->pFxTemplatePath,false,true);
}

void StageRunnerMainWin::on_loadTemplatesButton_clicked()
{
	appCentral->templateFxList->fileLoad(appCentral->userSettings->pFxTemplatePath);
	templateWidget->refreshList();
}

void StageRunnerMainWin::on_actionShow_Templates_triggered()
{
	template_dock->show();
	templateWidget->show();

}

void StageRunnerMainWin::on_actionShow_Fx_Properties_Editor_triggered()
{
	fxitem_editor_dock->show();
	fxItemEditor->show();
}

void StageRunnerMainWin::on_actionShow_Scene_Status_triggered()
{
	scene_status_dock->show();
	sceneStatusDisplay->show();
}

void StageRunnerMainWin::on_actionShow_Sequence_Status_triggered()
{
	sequence_status_dock->show();
	seqStatusDisplay->show();
}

void StageRunnerMainWin::on_saveUniverseButton_clicked()
{
	appCentral->universeLayout->initDmxChannels(0, 64);
	appCentral->universeLayout->fileSave("/tmp/universelayout.txt");
}

void StageRunnerMainWin::on_loadUniverseButton_clicked()
{
	appCentral->universeLayout->fileLoad("/tmp/universelayout.txt");
	appCentral->universeLayout->fileSave("/tmp/universelayout_copy.txt");
}

//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "config.h"
#include "configrev.h"
#include "version.h"
#include "log.h"
#include "stagerunnermainwin.h"
#include "commandsystem.h"
#include "appcentral.h"
#include "audiocontrol.h"
#include "lightcontrol.h"
#include "videocontrol.h"
#include "system/fxcontrol.h"
#include "execcenter.h"
#include "fxlist.h"
#include "project.h"
#include "usersettings.h"
#include "thirdparty/widget/qsynthdialpeppinostyle.h"
#include "thirdparty/widget/qsynthdialclassicstyle.h"
#include "thirdparty/widget/qsynthdialvokistyle.h"
#include "thirdparty/widget/qsynthdialskulpturestyle.h"
#include "setupwidget.h"
#include "fxitempropertywidget.h"
#include "scenestatuswidget.h"
#include "sequencestatuswidget.h"
#include "qtstatictools.h"
#include "style/lightdeskstyle.h"
#include "style/extmenustyle.h"
#include "fx/fxitem.h"
#include "fx/fxsceneitem.h"
#include "fx/fxseqitem.h"
#include "fx/fxscriptitem.h"
#include "fx/fxtimelineitem.h"
#include "fxplaylistitem.h"
#include "executer.h"
#include "fxlistwidget.h"
#include "scenedeskwidget.h"
#include "fx/fxitemobj.h"
#include "appcontrol/fxlistvarset.h"
#ifdef IS_QT6
#	include "gui/customwidget/psvideowidget6.h"
#else
#	include "gui/customwidget/psvideowidget.h"
#endif
#include "system/dmxuniverseproperty.h"
#include "fxscriptwidget.h"
#include "gui/customwidget/psinfodialog.h"
#include "gui/customwidget/psdockwidget.h"
#include "gui/consolidatedialog.h"
#include "gui/universeeditorwidget.h"
#include "mods/timeline/timelinebox.h"
#include "widgets/fxtimelineeditwidget.h"

#include "../plugins/yadi/src/dmxmonitor.h"

#include <QFileDialog>
#include <QErrorMessage>
#include <QTextEdit>
#include <QFont>
#include <QSplitterHandle>

using namespace PS_TL;

StageRunnerMainWin::StageRunnerMainWin(AppCentral *myapp) :
	QMainWindow(nullptr)
{
	appCentral = myapp;

	init();


	// DocWidgets defauls
	setTabPosition(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea, QTabWidget::North);
	setTabPosition(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea, QTabWidget::West);

	setupUi(this);
	setObjectName("StageRunnerMainwin");
	setup_gui_docks();

	auto acts = menuBar()->actions();
	for (QAction *a : acts) {
		if (a->menu()) {
			QMenu *m = a->menu();
			m->setStyle(new ExtMenuStyle());
			// m->setStyleSheet("QMenu {icon-size: 30px;} QMenu::item {background: transparent;} QMenu::item:selected {background: #333;}");
		}
		// qDebug() << a->menu()->objectName();
	}

	AudioControl *ac = myapp->unitAudio;

	if (!ac->isAudioOutAvailable(OUT_SDL2)) {
		delete actionUse_SDL_audio;
		actionUse_SDL_audio = nullptr;
	}
	if (!ac->isAudioOutAvailable(OUT_DEVICE)) {
		delete actionClassic_audio_mode;
		actionClassic_audio_mode = nullptr;
	}
	if (!ac->isAudioOutAvailable(OUT_MEDIAPLAYER)) {
		delete actionExperimental_audio_mode;
		actionExperimental_audio_mode = nullptr;
	}

	fxListWidget->setFxList(appCentral->project->mainFxList());

	debugLevelSpin->setValue(debug);

#ifdef IS_MAC
//	delete actionExperimental_audio_mode;
//	actionExperimental_audio_mode = nullptr;
#endif
	guiSetAudioOutput(myapp->usedAudioOutputType());

	actionEnable_audio_FFT->setChecked(appCentral->userSettings->pFFTAudioMask > 0);

	actionVirtualDmxOutput->setChecked(false);
	virtDmxWidget->setVisible(false);
	virtDmxWidget->setAutoBarsEnabled(true);
	virtDmxWidget->setBarsBordersEnabled(false);
	virtDmxWidget->setSecondBarGroupEnabled(true);

	// Time display and status
	m_timeLabel = new QLabel("12:00", this);
	m_timeLabel->move(width() - 100, height() - 30);
	m_timeLabel->show();
	QFont myfont(font());
	myfont.setPixelSize(16);
	myfont.setBold(true);
	m_timeLabel->setFont(myfont);

	connect(&m_statusTimer, SIGNAL(timeout()), this, SLOT(onStatusTimer()));
	m_statusTimer.start(1000);

	// Font in log view
	QFont font("DejaVu Sans Mono",9);
	font.setStyleHint(QFont::Monospace);
	logEdit->setFont(font);

	// For external access
	logWidget = logEdit;
	if (myapp->mainWinObj) {
		DEBUGERROR("There seems to be more than one StageRunnerMainWin instance!");
	}
	myapp->mainWinObj = this;
	myapp->mainwinWidget = this;
}

StageRunnerMainWin::~ StageRunnerMainWin()
{
	delete m_universeEditor;
	delete dialWidgetStyle;
	delete msg_dialog;
	delete fxitem_editor_dock;
}

void StageRunnerMainWin::initModules()
{
	TimeLineWidget *tlwid = new TimeLineWidget();
	if (!tlwid->isInitialized())
		tlwid->init();
	timelineLayout->addWidget(tlwid);

	// populize with some timeline items
	tlwid->addTimeLineBox(0, 10000, "item 1", 1);

	TimeLineBox *item = tlwid->addTimeLineBox(50000, 10000, "item 2", 2);
	item->setBackgroundColor(0x552222);

	// FxTimeLineEditWidget *fxTimeLineWid = new FxTimeLineEditWidget();
	// timelineLayout->addWidget(fxTimeLineWid);

	// // populize with some timeline items
	// fxTimeLineWid->timeLineWidget()->addTimeLineItem(0, 10000, "item 1", 1);

	// TimeLineItem *item = fxTimeLineWid->timeLineWidget()->addTimeLineItem(50000, 10000, "item 2", 2);
	// item->setBackgroundColor(0x552222);
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
	connect(appCentral->unitAudio,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),audioCtrlGroup,SLOT(audioCtrlReceiver(AUDIO::AudioCtrlMsg)));
	connect(audioCtrlGroup,SIGNAL(audioCtrlCmdEmitted(AUDIO::AudioCtrlMsg)),appCentral->unitAudio,SLOT(audioCtrlReceiver(AUDIO::AudioCtrlMsg)));
	connect(appCentral->unitAudio,SIGNAL(vuLevelChanged(int,qreal,qreal)),audioCtrlGroup,SLOT(setVuMeterLevel(int,qreal,qreal)));
	connect(appCentral->unitAudio,SIGNAL(fftSpectrumChanged(int,FrqSpectrum*)),audioCtrlGroup,SLOT(setFFTSpectrum(int,FrqSpectrum*)));
	connect(appCentral->unitAudio,SIGNAL(masterVolumeChanged(int)),seqCtrlGroup,SLOT(setMasterVolume(int)));
	connect(appCentral->unitAudio,SIGNAL(mediaPlayerInstancesCreated(QString)),this,SLOT(onMediaPlayerInstancesCreated(QString)));
	connect(appCentral->unitAudio,SIGNAL(audioIdleEventCountChanged(int)),this,SLOT(onAudioIdleEventsChanged(int)));

	// Light Control -> SceneStatusWidget
	connect(appCentral->unitLight,SIGNAL(sceneChanged(FxSceneItem*)),sceneStatusDisplay,SLOT(propagateScene(FxSceneItem*)));
	connect(appCentral->unitLight,SIGNAL(sceneFadeChanged(FxSceneItem*,int,int)),sceneStatusDisplay,SLOT(propagateSceneFade(FxSceneItem*,int,int)));
	// AppCentral -> SceneStatusWidget
	connect(appCentral,SIGNAL(fxSceneDeleted(FxSceneItem*)),sceneStatusDisplay,SLOT(removeScene(FxSceneItem*)));

	// Light Control -> Project FxListWidget
	connect(appCentral->unitLight,SIGNAL(sceneChanged(FxSceneItem*)),fxListWidget,SLOT(propagateSceneStatus(FxSceneItem*)));
	connect(appCentral->unitLight,SIGNAL(sceneFadeChanged(FxSceneItem*,int,int)),fxListWidget,SLOT(propagateSceneFadeProgress(FxSceneItem*,int,int)));

	// Light Control -> Audio Control
	connect(appCentral->unitLight,SIGNAL(audioSlotVolChanged(int,int)),appCentral->unitAudio,SLOT(setVolumeFromDmxLevel(int,int)));

	// Light Control -> virt DMX Monitor
	connect(appCentral->unitLight,SIGNAL(outputUniverseChanged(int,QByteArray)),virtDmxWidget,SLOT(setDmxValues(int,QByteArray)));
	connect(appCentral->unitLight,SIGNAL(inputUniverseChanged(int,QByteArray)),virtDmxWidget,SLOT(setDmxValuesSec(int,QByteArray)));

	// Audio Control -> Project FxListWidget
	connect(appCentral->unitAudio,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),fxListWidget,SLOT(propagateAudioStatus(AUDIO::AudioCtrlMsg)));

	// Fx Control -> SequenceStatusWidget
	connect(appCentral->unitFx,SIGNAL(executerChanged(Executer*)),seqStatusDisplay,SLOT(propagateExecuter(Executer*)));

	// ExecCenter -> SequenceStatusWidget
	connect(appCentral->execCenter,SIGNAL(executerCreated(Executer*)),seqStatusDisplay,SLOT(insertExecuter(Executer*)));
	connect(appCentral->execCenter,SIGNAL(executerDeleted(Executer*)),seqStatusDisplay,SLOT(scratchExecuter(Executer*)));
	connect(appCentral->execCenter,SIGNAL(executerChanged(Executer*)),seqStatusDisplay,SLOT(propagateExecuter(Executer*)));

	// Global Info & Error Messaging
	connect(logThread,SIGNAL(infoMsgReceived(QString,QString)),this,SLOT(showInfoMsg(QString,QString)));
	connect(logThread,SIGNAL(errorMsgReceived(QString,QString)),this,SLOT(showErrorMsg(QString,QString)));
	connect(logThread,SIGNAL(warnMsgSent(QString,int)),this,SLOT(setStatusMessage(QString,int)));

	// Project <-> Mainwindow
	connect(appCentral->project, SIGNAL(projectLoadedOrSaved(QString,bool)), this, SLOT(addRecentProject(QString)));

	// Video Control
	connect(appCentral->unitVideo, SIGNAL(mainVideoViewStateChanged(int,int)), lightCtrlGroup, SLOT(setVideoStatus(int,int)));

	qApp->installEventFilter(this);
}

void StageRunnerMainWin::setup_gui_docks()
{
	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
	setDockNestingEnabled(true);

	fxitem_editor_dock = new PsDockWidget(this);

	fxItemEditor = new FxItemPropertyWidget();
	connect(appCentral,SIGNAL(editModeChanged(bool)),fxItemEditor,SLOT(setEditable(bool)));
	fxItemEditor->setEditable(false);
	fxItemEditor->closeButton->hide();
	fxitem_editor_dock->setObjectName("Fx Editor");
	fxitem_editor_dock->setWindowTitle("Fx Editor");
	fxitem_editor_dock->setWidget(fxItemEditor);
	fxitem_editor_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,fxitem_editor_dock);

	template_dock = new PsDockWidget(this);
	templateWidget =new FxListWidget();
	template_dock->setObjectName("Fx Templates");
	template_dock->setWindowTitle("Fx Templates");
	template_dock->setWidget(templateWidget);
	template_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,template_dock);
	templateWidget->setFxList(appCentral->templateFxList->fxList());
	templateWidget->setAutoProceedCheckVisible(false);
	connect(templateWidget,SIGNAL(fxTypeColumnDoubleClicked(FxItem*)),this,SLOT(openFxItemPanel(FxItem*)));
	this->tabifyDockWidget(fxitem_editor_dock, template_dock);
	fxitem_editor_dock->raise();

	scene_status_dock = new PsDockWidget(this);
	sceneStatusDisplay = new SceneStatusWidget();
	scene_status_dock->setObjectName("Scene Status Display");
	scene_status_dock->setWindowTitle("Scene Status Display");
	scene_status_dock->setWidget(sceneStatusDisplay);
	scene_status_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,scene_status_dock);

	sequence_status_dock = new PsDockWidget(this);
	seqStatusDisplay = new SequenceStatusWidget();
	sequence_status_dock->setObjectName("Sequence Status Display");
	sequence_status_dock->setWindowTitle("Sequence Status Display");
	sequence_status_dock->setWidget(seqStatusDisplay);
	sequence_status_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::RightDockWidgetArea,sequence_status_dock);
	this->tabifyDockWidget(sequence_status_dock, scene_status_dock);


}

void StageRunnerMainWin::restore_window()
{
	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("GuiSettings");
	if (set.contains("MainWinGeometry")) {
		restoreGeometry(set.value("MainWinGeometry").toByteArray());
		restoreState(set.value("MainWinDocks").toByteArray());
		resize(set.value("MainWinSize").toSize());
		if (set.contains("SplitterPos")) {
			const QStringList s = set.value("SplitterPos").toString().split(';', QT_SKIP_EMPTY_PARTS);
			QList<int>sizes;
			for (const QString &e : s) {
				sizes.append(e.toInt());
			}
			splitter->setSizes(sizes);
		}
	}
	set.endGroup();

	// Get recent projects
//	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("PROJECT");
	set.beginReadArray("RecentProjects");
	for (int t=0; t<20; t++) {
		set.setArrayIndex(t);
		QString path = set.value("Path").toString();
		if (path.size() && !m_recentProjectPaths.contains(path))
			m_recentProjectPaths.append(path);
	}
	set.endArray();
	set.endGroup();
	// update Menu -> Recent Projects
	addRecentProject("");

	actionFullscreen->setChecked(isFullScreen());
	if (m_timeLabel)
		m_timeLabel->setVisible(isFullScreen());
}

void StageRunnerMainWin::updateDialWidgetStyle(const QString &style)
{
	LOGTEXT(tr("Set dial knob style to '%1'").arg(style));

	// Update Style of Dial knobs
	if (dialWidgetStyle) {
		delete dialWidgetStyle;
		dialWidgetStyle = nullptr;
	}

	if (style == "" || style == "QSynth Dial Classic") {
		dialWidgetStyle = new qsynthDialClassicStyle();
	}
	else if (style == "QSynth Dial Peppino") {
		dialWidgetStyle = new qsynthDialPeppinoStyle();
	}
	else if (style == "QSynth Dial Voki") {
		dialWidgetStyle = new qsynthDialVokiStyle();
	}
	else if (style == "QSynth Dial Skulpture") {
		dialWidgetStyle = new qsynthDialSkulptureStyle();
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
		QWidget *parentWid = this;
		QWidget *videoWid = nullptr;
		if (appCentral->isVideoWidgetVisible(&videoWid))
			parentWid = videoWid;

		int ret = QMessageBox::question(parentWid,tr("Attention")
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
	dialWidgetStyle = nullptr;

	msg_dialog = new QErrorMessage(this);
	fxitem_editor_dock = nullptr;
	fxItemEditor = nullptr;
	scene_status_dock = nullptr;
	sceneStatusDisplay = nullptr;
	sequence_status_dock = nullptr;
	m_universeEditor = nullptr;
	seqStatusDisplay = nullptr;
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
	if (appCentral->userSettings->pShowVirtualDMXMonitor) {
		virtDmxWidget->show();
		virtDmxWidget->setSmallHeightEnabled(true);
		actionVirtualDmxOutput->setChecked(true);
	}

	// restore window layout
	restore_window();

	// Load Default Template FxList
	if (QFile::exists(appCentral->userSettings->pFxTemplatePath))
		on_loadTemplatesButton_clicked();

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

	// Test Audio Features -> removed: already done in initAudioControl()
	// appCentral->unitAudio->getAudioDevices();

	// Set Audio Defaults
	if (appCentral->unitAudio->isValid()) {
		appCentral->unitAudio->setFFTAudioChannelFromMask(appCentral->userSettings->pFFTAudioMask);
		actionEnable_audio_FFT->setChecked(appCentral->userSettings->pFFTAudioMask == 15);
	}

	// Copy User Settings to GUI
	applyUserSettingsToGui(appCentral->userSettings);


}

/**
 * @brief Open Dialog with content of shadow log messages if there are any
 */
void StageRunnerMainWin::showShadowLog()
{
	if (logThread->shadowErrorCount() == 0)
		return;

	PsInfoDialog *dialog = new PsInfoDialog(this);
	if (logThread->shadowErrorCount() > 0)
		dialog->setText(logThread->shadowLogStrings(MSG_ALL));
	dialog->show();
}

/**
 * @brief Open Dialog with module error status, if there is any module error
 */
void StageRunnerMainWin::showModuleError()
{
#ifdef USE_SDL
	if (appCentral->moduleErrors() == AppCentral::E_NO_AUDIO_DECODER && appCentral->hasSDL()) {
		appCentral->setModuleError(AppCentral::E_NO_AUDIO_DECODER, false);
		actionUse_SDL_audio->setChecked(true);
		appCentral->setAudioOutputType(OUT_SDL2);
		LOGTEXT("<font color=info>Switched to SDL audio, since Qt audio is not available");
	}
#endif

	if (!appCentral->hasModuleError())
		return;

	PsInfoDialog *dialog = new PsInfoDialog(this);
	dialog->setText(appCentral->moduleErrorText(appCentral->moduleErrors()));
	dialog->show();
}

void StageRunnerMainWin::copyGuiSettingsToProject()
{
	appCentral->project->pAutoProceedSequence = fxListWidget->fxList()->autoProceedSequence();
	appCentral->project->pSlotGuiConfig = audioCtrlGroup->completeGuiSettings();
}

void StageRunnerMainWin::copyProjectSettingsToGui()
{
	fxListWidget->setAutoProceedSequence( appCentral->project->pAutoProceedSequence );
	audioCtrlGroup->setCompleteGuiSettings(appCentral->project->pSlotGuiConfig);
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
	case FX_TIMELINE:
		openFxTimeLinePanel(static_cast<FxTimeLineItem*>(fx));
		break;
	}
}

void StageRunnerMainWin::applyUserSettingsToGui(UserSettings *set)
{
	appCentral->unitAudio->checkAudioLimits();
	audioCtrlGroup->setFFTGraphVisibleFromMask(set->pFFTAudioMask);
	audioCtrlGroup->setVolumeDialVisibleFromMask(set->pVolumeDialMask);
}

AUDIO::AudioOutputType StageRunnerMainWin::guiSetAudioOutput(AudioOutputType type)
{
	if (actionExperimental_audio_mode)
		actionExperimental_audio_mode->setChecked(false);
	if (actionUse_SDL_audio)
		actionUse_SDL_audio->setChecked(false);
	if (actionClassic_audio_mode)
		actionClassic_audio_mode->setChecked(false);
	actionConfigDefaultAudio->setChecked(false);

	if (!AppCentral::ref().unitAudio->isAudioOutAvailable(type)) {
		type = OUT_NONE;		// this is default type
		actionConfigDefaultAudio->setChecked(true);
	}
	else if (type == OUT_NONE) {
		actionConfigDefaultAudio->setChecked(true);
	}
	else if (type == OUT_DEVICE && actionClassic_audio_mode) {
		actionClassic_audio_mode->setChecked(true);
	}
	else if (type == OUT_MEDIAPLAYER && actionExperimental_audio_mode) {
		actionExperimental_audio_mode->setChecked(true);
	}
	else if (type == OUT_SDL2 && actionUse_SDL_audio) {
		actionUse_SDL_audio->setChecked(true);
	}
	else {
		type = OUT_NONE;
	}

	return type;
}

/**
 * @brief Manage recent project list (both internal and GUI)
 * @param pathname Project path you wish to add to recent project list
 *
 * Max. 10 Projects are hold in the list.
 */
void StageRunnerMainWin::addRecentProject(const QString &pathname)
{
	bool updateGuiOnly = pathname.isEmpty();

	if (!updateGuiOnly) {
		// this brings current pathname to front without producing doubles
		m_recentProjectPaths.removeAll(pathname);
		m_recentProjectPaths.prepend(pathname);
		while (m_recentProjectPaths.size() > 20)
			m_recentProjectPaths.removeLast();

		QSettings set(QSETFORMAT,APPNAME);
		set.beginGroup("PROJECT");
		set.beginWriteArray("RecentProjects");
		for (int t=0; t<m_recentProjectPaths.size(); t++) {
			set.setArrayIndex(t);
			set.setValue("Path",m_recentProjectPaths.at(t));
		}
		for (int t=m_recentProjectPaths.size(); t<20; t++) {
			set.setArrayIndex(t);
			set.remove("Path");
		}
		set.endArray();
		set.endGroup();
	}

	QAction *ma = findChild<QAction*>("actionFileRecentProjects");
	if (ma) {
		QMenu *m = ma->menu();
		if (!m) {
			m = new QMenu();
			m->setToolTipsVisible(true);
			ma->setMenu(m);
		} else {
			m->clear();
		}

		for (int t=0; t<m_recentProjectPaths.size(); t++) {
			QString path = m_recentProjectPaths.at(t);
			QString name = QFileInfo(path).completeBaseName();
			QAction *a = new QAction(name, this);
			a->setToolTip(path);
			a->setObjectName(path);
			connect(a, SIGNAL(triggered(bool)), this, SLOT(onRecentProjectActionSelected()));
//			QIcon icon;
//			icon.addFile(QString::fromUtf8(":/button/images48/drive-harddisk.png"), QSize(20,20), QIcon::Normal, QIcon::Off);
//			a->setIcon(icon);

			m->addAction(a);
		}
	}
}

void StageRunnerMainWin::setStatusMessage(const QString &msg, int displayTimeMs)
{
	if (msg.isEmpty()) {
		statusBar()->setStyleSheet("");
		statusBar()->showMessage("", 20);
	}
	else {
		if (msg == statusBar()->currentMessage() || statusBar()->styleSheet().isEmpty()) {
			if (statusBar()->styleSheet().startsWith("color:red")) {
				statusBar()->setStyleSheet("color:white; background:red; font-weight:bold;");
			} else {
				statusBar()->setStyleSheet("color:red; background:white; font-weight:bold;");
			}
		}
		statusBar()->showMessage(msg, displayTimeMs);
	}

}

void StageRunnerMainWin::onStatusTimer()
{
	if (m_timeLabel) {
		if (isFullScreen()) {
			m_timeLabel->show();
			actionFullscreen->setChecked(true);
		}

		QDateTime dt = QDateTime::currentDateTimeUtc();
		QString time = dt.toLocalTime().toString("ddd d. MMMM hh:mm:ss");
		m_timeLabel->setText(time);

        // calculate size of text
        QFontMetrics fm(m_timeLabel->font());
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        int fw = fm.width(time) + 8;
#else
        int fw = fm.horizontalAdvance(time) + 8;
#endif
		// qDebug() << "mainwin fw" << fw << m_timeLabel->width();
		if (fw > m_timeLabel->width() || m_timeLabel->x() > height() - fw) {
			m_timeLabel->resize(fw, m_timeLabel->height());
			m_timeLabel->move(width() - fw, m_timeLabel->y());
		}
	}
}

void StageRunnerMainWin::onAudioIdleEventsChanged(int count)
{
	if (count == 0) {
		audioCtrlGroup->setStatusExt(QString());
	}
	else {
		QString stat = QString("Audio Idle count: (buffer underrun): %1").arg(count);
		audioCtrlGroup->setStatusExt(stat);
	}
}

void StageRunnerMainWin::onMediaPlayerInstancesCreated(const QString &msg)
{
	audioCtrlGroup->setStatus(msg);
	onAudioIdleEventsChanged(0);
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
			connect(AppCentral::instance()->unitAudio,SIGNAL(audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg)),playlistwid,SLOT(propagateAudioStatus(AUDIO::AudioCtrlMsg)));
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

void StageRunnerMainWin::openFxTimeLinePanel(FxTimeLineItem *fx)
{
	FxTimeLineEditWidget *timelineWid = FxTimeLineEditWidget::openTimeLinePanel(fx);
	if (timelineWid) {
		timelineWid->resize(1000, 270);
		timelineWid->show();
	}
}

void StageRunnerMainWin::setProjectName(const QString &path)
{
	QString title = QString("%1 %2 ").arg(APPNAME,APPVERSION);
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

void StageRunnerMainWin::loadProject(const QString &path)
{
	clearProject();

	appCentral->userSettings->pLastProjectLoadPath = path;

	Project *pro = appCentral->project;
	if (!QFile::exists(path)) {
		QMessageBox::critical(this,tr("Load error")
							  ,tr("Project file not found!\n'%1'")
							  .arg(path));
	}
	else if ( !pro->loadFromFile(path) ) {
		QMessageBox::critical(this,tr("Load error")
							  ,tr("An error occured while loading file:\n\n%1\n\nLine %2:'%3'")
							  .arg(appCentral->userSettings->pLastProjectLoadPath)
							  .arg(pro->loadErrorLineNumber).arg(pro->loadErrorLineString));
	}
	else {
		pro->postLoadProcessFxList();
		pro->postLoadResetFxScenes();

		fxListWidget->setFxList(pro->mainFxList());
		copyProjectSettingsToGui();
		setProjectName(path);

		return;
	}

	// loading failed, remove project from recent project list
	setProjectName("");
	m_recentProjectPaths.removeAll(path);
	addRecentProject("");
}

void StageRunnerMainWin::setApplicationGuiStyle(const QString &style)
{
	LOGTEXT(tr("Set Application GUI style to '%1'").arg(style));

	bool isLightDesk = false;
	if (style == "" || style == "LightDesk") {
		QApplication::setStyle(new LightDeskStyle);
		isLightDesk = true;
	}
	else if (style == "Default") {
		QApplication::setStyle(nullptr);
	}
	else {
		QApplication::setStyle(QStyleFactory::create(style));
	}

	// set palette also. In some environments this is necessary
	QApplication::setPalette(QApplication::style()->standardPalette());

	if (m_timeLabel) {
		if (isLightDesk) {
			QPalette pal = m_timeLabel->palette();
			pal.setBrush(QPalette::WindowText, QColor(220,180,0));
			m_timeLabel->setPalette(pal);
		} else {
			m_timeLabel->setPalette(QApplication::style()->standardPalette());
		}
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
		copyGuiSettingsToProject();
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

void StageRunnerMainWin::on_actionConsolidate_Project_triggered()
{
	Project *pro = appCentral->project;
	if (pro->curProjectFilePath.isEmpty())
		return;

	ConsolidateDialog dialog(pro, this);

	int ret = dialog.exec();

	if (ret == QDialog::Accepted) {

	}

}


void StageRunnerMainWin::on_actionLoad_Project_triggered()
{
	QString curpath = appCentral->userSettings->pLastProjectLoadPath;
	if (curpath.isEmpty())
		curpath = QDir::homePath();

	QString path = QFileDialog::getOpenFileName(this,tr("Choose Project")
												,curpath
												,tr("StageRunner projects (*.srp);;FxMaster projects (*.fxm);;All files (*)"));
	if (path.size())
		loadProject(path);
}

void StageRunnerMainWin::on_actionNew_Project_triggered()
{
	clearProject();

}

bool StageRunnerMainWin::eventFilter(QObject *obj, QEvent *event)
{

	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease ) {
		// qDebug("KeyEvent %s",obj->objectName().toLocal8Bit().data());

		if (actionEdit_Mode->isChecked())
			return qApp->eventFilter(obj, event);

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
		bool isAutoRepeat = ev->isAutoRepeat();

		LOGTEXT(tr("<font color=green>Key pressed</font>: #%1 -> '<b><font color=orange>%2</font></b>'")
				.arg(key)
				.arg(QtStaticTools::keyToString(key,activeKeyModifiers)));
		// qInfo() << "key pressed" << key;

		switch (key) {
		case Qt::Key_Shift:
			activeKeyModifiers |= Qt::SHIFT;
			this->statusBar()->setStyleSheet("color: red; font-weight: bold; background: white;");
			this->statusBar()->showMessage("SHIFT PRESSED", 1000);
			break;
		case Qt::Key_Control:
			activeKeyModifiers |= Qt::CTRL;
			break;
		case Qt::Key_Space:
			if (!isAutoRepeat) {
				appCentral->unitAudio->storeCurrentSeekPositions();
				appCentral->fadeoutAllFxAudio();
			}
			break;
		case Qt::Key_Escape:
			if (!isAutoRepeat) {
				appCentral->unitAudio->storeCurrentSeekPositions();
				appCentral->stopAllFxAudio();
			}
			break;
		case Qt::Key_Delete:
			on_actionDelete_FX_Item_triggered();
			break;
		case Qt::Key_Backspace:
			if (!isAutoRepeat) {
				appCentral->unitFx->stopAllFxSequences();
				appCentral->unitFx->stopAllFxScripts();
				appCentral->unitFx->stopAllTimeLines();
				appCentral->lightBlack(0);
				appCentral->videoBlack(0);
				appCentral->unitLight->setYadiInOutMergeMode(0);			// Set Yadi devices to HTP mode
			}
			break;
		case Qt::Key_Right:
			if (!appCentral->isEditMode()) {
				appCentral->executeNextFx(1);
			}
			break;
		case Qt::Key_Left:
			if (!appCentral->isEditMode()) {
				appCentral->executePrevFx(1);
			}
			break;

//		case Qt::Key_Q: {
//			QList<QTabWidget*> list = findChildren<QTabWidget*>();
//			qDebug() << "tabs" << list;

//			QWidget *l = scene_status_dock;
//			while (l->parentWidget()) {
//				l = l->parentWidget();
//				qDebug() << "parent" << l;
//			}
//		}
//			break;

		default:
			if (key) {
				// Do not start FX if CTRL key is pressed.
				if (uint(activeKeyModifiers) & Qt::CTRL) {
					return qApp->eventFilter(obj, event);
				}

				if (!isAutoRepeat) {
					// get all fx items, that are bound to the pressed key combination
					// and start each of this
					QList<FxItem *>fxlist = appCentral->project->mainFxList()->getFxListByKeyCode(key + activeKeyModifiers);
					if (fxlist.size()) {
						for (int t=0; t<fxlist.size(); t++) {
							appCentral->executeFxCmd(fxlist.at(t), CMD_FX_START, nullptr);
						}
						// fxListWidget->selectFx(fx);
					}
				}
			}
			break;
		}

		appCentral->setCurrentKeyModifiers(activeKeyModifiers);
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
	appCentral->closeAllDmxMonitors();


	if (appCentral->project->isModified()) {
		QWidget *parentWid = this;
		QWidget *videoWid = nullptr;
		if (appCentral->isVideoWidgetVisible(&videoWid))
			parentWid = videoWid;
		int ret = QMessageBox::question(parentWid,
										tr("Attention"),
										tr("Project is modified!\n\nDo you want to save it now?"));
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

	// This stops and removes all running sequences, timelines and scripts
	appCentral->stopAllSequencesAndPlaylists();

	if (appCentral->unitAudio->isValid()) {
		if (appCentral->unitAudio->fadeoutAllFxAudio(1000)) {
			hide();
			QElapsedTimer wait;
			wait.start();
			while (wait.elapsed() < 1500)
				QApplication::processEvents();
		}
	}

	if (appCentral->unitAudio->videoWidget())
		appCentral->unitAudio->videoWidget()->close();


	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("GuiSettings");
	set.setValue("MainWinGeometry",saveGeometry());
	set.setValue("MainWinSize",size());
	set.setValue("MainWinDocks",saveState());

	QString s;
	for (int w : splitter->sizes()) {
		s += QString::number(w);
		s += ';';
	}

	set.setValue("SplitterPos", s);
	set.endGroup();

	bool videoWinEnabled = appCentral->unitAudio->videoWidget() && appCentral->unitAudio->videoWidget()->isVisible();
	set.setValue("VideoWinEnabled",videoWinEnabled);

	QMainWindow::closeEvent(event);

	FxListWidget::destroyAllFxListWidgets();
	SceneDeskWidget::destroyAllSceneDesks();
	FxTimeLineEditWidget::destroyAllTimelinePanels();
}

void StageRunnerMainWin::resizeEvent(QResizeEvent *event)
{
	if (m_timeLabel) {
		QSize mainsize = event->size();
		int x = mainsize.width() - 100;
		int y = mainsize.height() - 30;
		m_timeLabel->move(x, y);
		if (isFullScreen())
			m_timeLabel->show();
	}
}

void StageRunnerMainWin::showInfoMsg(const QString &where, const QString &text)
{
	// QString msg = QString("<font color=#222222>%1</font><br><br>Reported from function:%2")
	// 		.arg(text,where);
	msg_dialog->setStyleSheet("");
	msg_dialog->showMessage(text, where);
	msg_dialog->resize(800,200);
	msg_dialog->setWindowTitle(tr("Information"));

}

void StageRunnerMainWin::showErrorMsg(const QString &where, const QString &text)
{
	QString msg = QString("<font color=#ff7722>%1</font><br><br>Reported from function: %2")
			.arg(text,where);
	msg.replace('\n',"<br>");


	msg_dialog->setStyleSheet("color:#ff7722;");
	msg_dialog->showMessage(msg, where);
	msg_dialog->resize(800,200);
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
	connect(&setup,SIGNAL(dialKnobStyleChanged(QString)),this,SLOT(updateDialWidgetStyle(QString)));
	connect(&setup,SIGNAL(setupChanged(UserSettings*)),this,SLOT(applyUserSettingsToGui(UserSettings*)));

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
	QElapsedTimer wait;
	wait.start();
	while (wait.elapsed() < 8000) {
		;
	}
	qDebug() << "Mainloop is back";

//	int t = 0;
//	int a = wait.elapsed() / t;
}

void StageRunnerMainWin::on_actionDMX_Input_triggered()
{
	QWidget * mon = appCentral->openDmxInMonitor(0);
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
	QString msg = QString("%1 %2\n%3\n\n%4\n\n").arg(APPNAME, APPVERSION, APP_MILESTONE, APP_PRODUCER);
	msg += QString("Qt version (binary): %1\nQt version (runtime): %2\n\n").arg(QT_VERSION_STR, qVersion());

#ifdef __unix__
	// msg += QString("Git timestamp: %1\n").arg(QDateTime::fromTime_t(GIT_APP_TIME).toString());
	msg += QString("Git timestamp: %1\n").arg(QDateTime::fromSecsSinceEpoch(GIT_APP_TIME).toString());
	msg += QString("Build date: %1\n").arg(BUILD_DATE);
	msg += QString("Build path: %1\n").arg(BUILD_PATH);
#endif

	msg += QString("\nStageRunner uses code from:\n"
				   " - QDial styles and vu meter from QSynth by Rui Nuno Capela\n"
				   " - some DMX plugins and interface from QLCplus");

	QMessageBox msgbox(QMessageBox::Information,
					   QString("StageRunner information"),
					   msg);
	msgbox.findChild<QLabel*>("qt_msgbox_label")->setFixedWidth(700);
	msgbox.exec();

	// QMessageBox::information(this,tr("About Info"),msg);

}


void StageRunnerMainWin::on_actionConfigDefaultAudio_triggered(bool checked)
{
	if (checked) {
		AudioOutputType audioType = guiSetAudioOutput(OUT_NONE);
		appCentral->setAudioOutputType(audioType);
		appCentral->unitAudio->reCreateMediaPlayerInstances();
	}
}


void StageRunnerMainWin::on_actionClassic_audio_mode_triggered(bool checked)
{
	if (checked) {
		AudioOutputType audioType = guiSetAudioOutput(OUT_DEVICE);
		appCentral->setAudioOutputType(audioType);
		appCentral->unitAudio->reCreateMediaPlayerInstances();
	}
}

void StageRunnerMainWin::on_actionExperimental_audio_mode_triggered(bool checked)
{
	if (checked) {
		AudioOutputType audioType = guiSetAudioOutput(OUT_MEDIAPLAYER);
		appCentral->setAudioOutputType(audioType);
		appCentral->unitAudio->reCreateMediaPlayerInstances();
	}
}

void StageRunnerMainWin::on_actionUse_SDL_audio_triggered(bool arg1)
{
	if (arg1) {
		AudioOutputType audioType = guiSetAudioOutput(OUT_SDL2);
		appCentral->setAudioOutputType(audioType);
		appCentral->unitAudio->reCreateMediaPlayerInstances();
	};
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
	appCentral->templateFxList->fxList()->recreateFxIDs(11000);
	appCentral->templateFxList->fxList()->refAllMembers();
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

void StageRunnerMainWin::on_actionVirtualDmxOutput_triggered(bool checked)
{
	virtDmxWidget->setVisible(checked);
	virtDmxWidget->setSmallHeightEnabled(true);
	appCentral->userSettings->pShowVirtualDMXMonitor = checked;
}

void StageRunnerMainWin::onRecentProjectActionSelected()
{
	loadProject(sender()->objectName());
}


void StageRunnerMainWin::on_actionOpen_universe_layout_editor_triggered()
{
	if (!m_universeEditor) {
		m_universeEditor = new UniverseEditorWidget();
	}

	m_universeEditor->show();
	m_universeEditor->raise();
}

void StageRunnerMainWin::on_actionNotes_triggered()
{
	QTextEdit *edit = new QTextEdit();
	edit->resize(1000,700);
	edit->setAttribute(Qt::WA_DeleteOnClose);
	edit->setReadOnly(true);
#if QT_VERSION >= 0x050a00
	edit->setTabStopDistance(40);
#endif

	QFile file(":/docs/help.txt");
	if (file.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream in(&file);
		edit->setPlainText(in.readAll());
	}
	edit->show();
}

void StageRunnerMainWin::on_actionFullscreen_triggered(bool checked)
{
	if (checked) {
		this->showFullScreen();
	} else {
		this->showNormal();
	}

	if (m_timeLabel) {
		m_timeLabel->setVisible(checked);
	}
}

void StageRunnerMainWin::on_actionDelete_FX_Item_triggered()
{
	if (appCentral->project->mainFxList()->nextFx()->isUsed()) {
		POPUPERRORMSG(tr("Cancel message"), tr("FX item is used!\nIt's not allowed to delete an active FX."), this);
		return;
	}
	if (QMessageBox::question(this,tr("Attention"),tr("Do you want to delete current selected FX from play?"))
			== QMessageBox::Yes) {
		appCentral->project->mainFxList()->deleteFx(appCentral->project->mainFxList()->nextFx());
	}
}


void StageRunnerMainWin::on_actionClose_video_window_triggered()
{
	appCentral->closeVideoWidget();
}


void StageRunnerMainWin::on_showFontsButton_clicked()
{
	QTreeWidget *fontTree = new QTreeWidget;
	fontTree->setAttribute(Qt::WA_DeleteOnClose);
	fontTree->setColumnCount(2);
	fontTree->setHeaderLabels(QStringList() << "Font" << "Smooth Sizes");

#ifdef IS_QT5
	QFontDatabase database;
	const QStringList fontFamilies = database.families();
	for (const QString &family : fontFamilies) {
		QTreeWidgetItem *familyItem = new QTreeWidgetItem(fontTree);
		familyItem->setText(0, family);

		const QStringList fontStyles = database.styles(family);
		for (const QString &style : fontStyles) {
			QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
			styleItem->setText(0, style);

			QString sizes;
			const QList<int> smoothSizes = database.smoothSizes(family, style);
			for (int points : smoothSizes)
				sizes += QString::number(points) + ' ';

			styleItem->setText(1, sizes.trimmed());
		}
	}
#else
	const QStringList fontFamilies = QFontDatabase::families();
	for (const QString &family : fontFamilies) {
		QTreeWidgetItem *familyItem = new QTreeWidgetItem(fontTree);
		familyItem->setText(0, family);

		const QStringList fontStyles = QFontDatabase::styles(family);
		for (const QString &style : fontStyles) {
			QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
			styleItem->setText(0, style);

			QString sizes;
			const QList<int> smoothSizes = QFontDatabase::smoothSizes(family, style);
			for (int points : smoothSizes)
				sizes += QString::number(points) + ' ';

			styleItem->setText(1, sizes.trimmed());
		}
	}
#endif

	fontTree->show();
}


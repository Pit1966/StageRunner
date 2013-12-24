/********************************************************************************
** Form generated from reading UI file 'stagerunnermainwin.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STAGERUNNERMAINWIN_H
#define UI_STAGERUNNERMAINWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gui/audiocontrolwidget.h"
#include "gui/fxlistwidget.h"
#include "gui/lightcontrolwidget.h"
#include "gui/sequencecontrolwidget.h"

QT_BEGIN_NAMESPACE

class Ui_StageRunnerMainWin
{
public:
    QAction *actionSave_Project;
    QAction *actionSave_Project_as;
    QAction *actionNew_Project;
    QAction *actionLoad_Project;
    QAction *actionEdit_Mode;
    QAction *actionSetup;
    QAction *actionExit_StageRunner;
    QAction *actionDMX_Input;
    QAction *actionDMX_Output;
    QAction *actionInput_Assign_Mode;
    QAction *actionInitialize_plugins_DMX;
    QAction *actionInfo;
    QAction *actionExperimental_audio_mode;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    FxListWidget *fxListWidget;
    QTabWidget *dmxMixerTab;
    QWidget *controlTab;
    QVBoxLayout *verticalLayout;
    SequenceControlWidget *seqCtrlGroup;
    AudioControlWidget *audioCtrlGroup;
    LightControlWidget *groupBox;
    QWidget *logTab;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *logEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *clearLogButton;
    QWidget *develTab;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *develGroup;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *addAudioFxButton;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *addAudioPlayListButton;
    QPushButton *addAudioTrackButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QSpinBox *debugLevelSpin;
    QPushButton *cloneSelectedSceneButton;
    QPushButton *stopMainLoopButton;
    QPushButton *addFxSceneButton;
    QPushButton *addFxSeqButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuConfiguration;
    QMenu *menuMonitor;
    QMenu *menuAbout;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *StageRunnerMainWin)
    {
        if (StageRunnerMainWin->objectName().isEmpty())
            StageRunnerMainWin->setObjectName(QStringLiteral("StageRunnerMainWin"));
        StageRunnerMainWin->resize(888, 668);
        QIcon icon;
        icon.addFile(QStringLiteral(":/gfx/icons/audip_power.png"), QSize(), QIcon::Normal, QIcon::Off);
        StageRunnerMainWin->setWindowIcon(icon);
        actionSave_Project = new QAction(StageRunnerMainWin);
        actionSave_Project->setObjectName(QStringLiteral("actionSave_Project"));
        actionSave_Project_as = new QAction(StageRunnerMainWin);
        actionSave_Project_as->setObjectName(QStringLiteral("actionSave_Project_as"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/gfx/icons/media-floppy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_Project_as->setIcon(icon1);
        actionNew_Project = new QAction(StageRunnerMainWin);
        actionNew_Project->setObjectName(QStringLiteral("actionNew_Project"));
        actionLoad_Project = new QAction(StageRunnerMainWin);
        actionLoad_Project->setObjectName(QStringLiteral("actionLoad_Project"));
        actionEdit_Mode = new QAction(StageRunnerMainWin);
        actionEdit_Mode->setObjectName(QStringLiteral("actionEdit_Mode"));
        actionEdit_Mode->setCheckable(true);
        actionSetup = new QAction(StageRunnerMainWin);
        actionSetup->setObjectName(QStringLiteral("actionSetup"));
        actionExit_StageRunner = new QAction(StageRunnerMainWin);
        actionExit_StageRunner->setObjectName(QStringLiteral("actionExit_StageRunner"));
        actionDMX_Input = new QAction(StageRunnerMainWin);
        actionDMX_Input->setObjectName(QStringLiteral("actionDMX_Input"));
        actionDMX_Output = new QAction(StageRunnerMainWin);
        actionDMX_Output->setObjectName(QStringLiteral("actionDMX_Output"));
        actionInput_Assign_Mode = new QAction(StageRunnerMainWin);
        actionInput_Assign_Mode->setObjectName(QStringLiteral("actionInput_Assign_Mode"));
        actionInput_Assign_Mode->setCheckable(true);
        actionInitialize_plugins_DMX = new QAction(StageRunnerMainWin);
        actionInitialize_plugins_DMX->setObjectName(QStringLiteral("actionInitialize_plugins_DMX"));
        actionInfo = new QAction(StageRunnerMainWin);
        actionInfo->setObjectName(QStringLiteral("actionInfo"));
        actionExperimental_audio_mode = new QAction(StageRunnerMainWin);
        actionExperimental_audio_mode->setObjectName(QStringLiteral("actionExperimental_audio_mode"));
        actionExperimental_audio_mode->setCheckable(true);
        centralwidget = new QWidget(StageRunnerMainWin);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        fxListWidget = new FxListWidget(splitter);
        fxListWidget->setObjectName(QStringLiteral("fxListWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fxListWidget->sizePolicy().hasHeightForWidth());
        fxListWidget->setSizePolicy(sizePolicy);
        splitter->addWidget(fxListWidget);
        dmxMixerTab = new QTabWidget(splitter);
        dmxMixerTab->setObjectName(QStringLiteral("dmxMixerTab"));
        dmxMixerTab->setIconSize(QSize(32, 32));
        controlTab = new QWidget();
        controlTab->setObjectName(QStringLiteral("controlTab"));
        verticalLayout = new QVBoxLayout(controlTab);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        seqCtrlGroup = new SequenceControlWidget(controlTab);
        seqCtrlGroup->setObjectName(QStringLiteral("seqCtrlGroup"));

        verticalLayout->addWidget(seqCtrlGroup);

        audioCtrlGroup = new AudioControlWidget(controlTab);
        audioCtrlGroup->setObjectName(QStringLiteral("audioCtrlGroup"));

        verticalLayout->addWidget(audioCtrlGroup);

        groupBox = new LightControlWidget(controlTab);
        groupBox->setObjectName(QStringLiteral("groupBox"));

        verticalLayout->addWidget(groupBox);

        QIcon icon2;
        icon2.addFile(QStringLiteral(":/gfx/icons/audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        dmxMixerTab->addTab(controlTab, icon2, QString());
        logTab = new QWidget();
        logTab->setObjectName(QStringLiteral("logTab"));
        verticalLayout_3 = new QVBoxLayout(logTab);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        logEdit = new QTextEdit(logTab);
        logEdit->setObjectName(QStringLiteral("logEdit"));

        verticalLayout_3->addWidget(logEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        clearLogButton = new QPushButton(logTab);
        clearLogButton->setObjectName(QStringLiteral("clearLogButton"));

        horizontalLayout->addWidget(clearLogButton);


        verticalLayout_3->addLayout(horizontalLayout);

        QIcon icon3;
        icon3.addFile(QStringLiteral(":/gfx/icons/mode_edit_list.png"), QSize(), QIcon::Normal, QIcon::Off);
        dmxMixerTab->addTab(logTab, icon3, QString());
        develTab = new QWidget();
        develTab->setObjectName(QStringLiteral("develTab"));
        verticalLayout_5 = new QVBoxLayout(develTab);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        develGroup = new QGroupBox(develTab);
        develGroup->setObjectName(QStringLiteral("develGroup"));
        gridLayout = new QGridLayout(develGroup);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(0);
        gridLayout->setVerticalSpacing(6);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 7, 0, 1, 1);

        addAudioFxButton = new QPushButton(develGroup);
        addAudioFxButton->setObjectName(QStringLiteral("addAudioFxButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addAudioFxButton->sizePolicy().hasHeightForWidth());
        addAudioFxButton->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(addAudioFxButton, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, -1);
        addAudioPlayListButton = new QPushButton(develGroup);
        addAudioPlayListButton->setObjectName(QStringLiteral("addAudioPlayListButton"));

        horizontalLayout_3->addWidget(addAudioPlayListButton);

        addAudioTrackButton = new QPushButton(develGroup);
        addAudioTrackButton->setObjectName(QStringLiteral("addAudioTrackButton"));

        horizontalLayout_3->addWidget(addAudioTrackButton);


        gridLayout->addLayout(horizontalLayout_3, 5, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        label = new QLabel(develGroup);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        debugLevelSpin = new QSpinBox(develGroup);
        debugLevelSpin->setObjectName(QStringLiteral("debugLevelSpin"));

        horizontalLayout_2->addWidget(debugLevelSpin);


        gridLayout->addLayout(horizontalLayout_2, 9, 0, 1, 1);

        cloneSelectedSceneButton = new QPushButton(develGroup);
        cloneSelectedSceneButton->setObjectName(QStringLiteral("cloneSelectedSceneButton"));

        gridLayout->addWidget(cloneSelectedSceneButton, 3, 0, 1, 1);

        stopMainLoopButton = new QPushButton(develGroup);
        stopMainLoopButton->setObjectName(QStringLiteral("stopMainLoopButton"));

        gridLayout->addWidget(stopMainLoopButton, 8, 0, 1, 1);

        addFxSceneButton = new QPushButton(develGroup);
        addFxSceneButton->setObjectName(QStringLiteral("addFxSceneButton"));

        gridLayout->addWidget(addFxSceneButton, 1, 0, 1, 1);

        addFxSeqButton = new QPushButton(develGroup);
        addFxSeqButton->setObjectName(QStringLiteral("addFxSeqButton"));

        gridLayout->addWidget(addFxSeqButton, 2, 0, 1, 1);


        verticalLayout_5->addWidget(develGroup);

        QIcon icon4;
        icon4.addFile(QStringLiteral(":/gfx/icons/mode_edit.png"), QSize(), QIcon::Normal, QIcon::Off);
        dmxMixerTab->addTab(develTab, icon4, QString());
        splitter->addWidget(dmxMixerTab);

        verticalLayout_2->addWidget(splitter);

        StageRunnerMainWin->setCentralWidget(centralwidget);
        menubar = new QMenuBar(StageRunnerMainWin);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 888, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuConfiguration = new QMenu(menubar);
        menuConfiguration->setObjectName(QStringLiteral("menuConfiguration"));
        menuMonitor = new QMenu(menubar);
        menuMonitor->setObjectName(QStringLiteral("menuMonitor"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        StageRunnerMainWin->setMenuBar(menubar);
        statusbar = new QStatusBar(StageRunnerMainWin);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        StageRunnerMainWin->setStatusBar(statusbar);
        toolBar = new QToolBar(StageRunnerMainWin);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        StageRunnerMainWin->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuConfiguration->menuAction());
        menubar->addAction(menuMonitor->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionSave_Project);
        menuFile->addAction(actionSave_Project_as);
        menuFile->addAction(actionExit_StageRunner);
        menuEdit->addAction(actionEdit_Mode);
        menuEdit->addAction(actionInput_Assign_Mode);
        menuConfiguration->addAction(actionSetup);
        menuConfiguration->addAction(actionInitialize_plugins_DMX);
        menuConfiguration->addAction(actionExperimental_audio_mode);
        menuMonitor->addAction(actionDMX_Input);
        menuMonitor->addAction(actionDMX_Output);
        menuAbout->addAction(actionInfo);
        toolBar->addSeparator();

        retranslateUi(StageRunnerMainWin);
        QObject::connect(clearLogButton, SIGNAL(clicked()), logEdit, SLOT(clear()));

        dmxMixerTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(StageRunnerMainWin);
    } // setupUi

    void retranslateUi(QMainWindow *StageRunnerMainWin)
    {
        StageRunnerMainWin->setWindowTitle(QApplication::translate("StageRunnerMainWin", "StageRunner (C) Stonechip Entertainment", 0));
        actionSave_Project->setText(QApplication::translate("StageRunnerMainWin", "Save Project", 0));
        actionSave_Project_as->setText(QApplication::translate("StageRunnerMainWin", "Save Project as", 0));
        actionNew_Project->setText(QApplication::translate("StageRunnerMainWin", "New Project", 0));
        actionLoad_Project->setText(QApplication::translate("StageRunnerMainWin", "Load Project", 0));
        actionEdit_Mode->setText(QApplication::translate("StageRunnerMainWin", "Edit Mode", 0));
        actionSetup->setText(QApplication::translate("StageRunnerMainWin", "Setup", 0));
        actionExit_StageRunner->setText(QApplication::translate("StageRunnerMainWin", "Exit StageRunner", 0));
        actionDMX_Input->setText(QApplication::translate("StageRunnerMainWin", "DMX Input", 0));
        actionDMX_Output->setText(QApplication::translate("StageRunnerMainWin", "DMX Output", 0));
        actionInput_Assign_Mode->setText(QApplication::translate("StageRunnerMainWin", "Auto Input Assign Mode", 0));
        actionInitialize_plugins_DMX->setText(QApplication::translate("StageRunnerMainWin", "Initialize plugins (DMX)", 0));
        actionInfo->setText(QApplication::translate("StageRunnerMainWin", "Info", 0));
        actionExperimental_audio_mode->setText(QApplication::translate("StageRunnerMainWin", "Experimental audio mode", 0));
        seqCtrlGroup->setTitle(QApplication::translate("StageRunnerMainWin", "Sequence Control", 0));
        audioCtrlGroup->setTitle(QApplication::translate("StageRunnerMainWin", "Audio Control", 0));
        groupBox->setTitle(QApplication::translate("StageRunnerMainWin", "Light Control", 0));
        dmxMixerTab->setTabText(dmxMixerTab->indexOf(controlTab), QApplication::translate("StageRunnerMainWin", "Control", 0));
        clearLogButton->setText(QApplication::translate("StageRunnerMainWin", "Clear Log", 0));
        dmxMixerTab->setTabText(dmxMixerTab->indexOf(logTab), QApplication::translate("StageRunnerMainWin", "Logging", 0));
        develGroup->setTitle(QApplication::translate("StageRunnerMainWin", "Development Quick Functions", 0));
        addAudioFxButton->setText(QApplication::translate("StageRunnerMainWin", "Import and Add Audio Fx", 0));
        addAudioPlayListButton->setText(QApplication::translate("StageRunnerMainWin", "Add Audio Play List FX", 0));
        addAudioTrackButton->setText(QApplication::translate("StageRunnerMainWin", "Add Audio Track To Play List Fx", 0));
        label->setText(QApplication::translate("StageRunnerMainWin", "Debug level", 0));
        cloneSelectedSceneButton->setText(QApplication::translate("StageRunnerMainWin", "Clone Selected Scene FX", 0));
        stopMainLoopButton->setText(QApplication::translate("StageRunnerMainWin", "Stop main loop for 3 seconds", 0));
        addFxSceneButton->setText(QApplication::translate("StageRunnerMainWin", "Add Scene Fx", 0));
        addFxSeqButton->setText(QApplication::translate("StageRunnerMainWin", "Add Fx Sequence", 0));
        dmxMixerTab->setTabText(dmxMixerTab->indexOf(develTab), QApplication::translate("StageRunnerMainWin", "Development", 0));
        menuFile->setTitle(QApplication::translate("StageRunnerMainWin", "File", 0));
        menuEdit->setTitle(QApplication::translate("StageRunnerMainWin", "Edit", 0));
        menuConfiguration->setTitle(QApplication::translate("StageRunnerMainWin", "Configuration", 0));
        menuMonitor->setTitle(QApplication::translate("StageRunnerMainWin", "Monitor", 0));
        menuAbout->setTitle(QApplication::translate("StageRunnerMainWin", "About", 0));
        toolBar->setWindowTitle(QApplication::translate("StageRunnerMainWin", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class StageRunnerMainWin: public Ui_StageRunnerMainWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STAGERUNNERMAINWIN_H

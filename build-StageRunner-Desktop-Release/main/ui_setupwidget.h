/********************************************************************************
** Form generated from reading UI file 'setupwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETUPWIDGET_H
#define UI_SETUPWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetupWidget
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *dmxTab;
    QVBoxLayout *verticalLayout_3;
    QLabel *qlcPluginLab;
    QSplitter *splitter_2;
    QSplitter *splitter;
    QListWidget *qlcPluginsList;
    QTableWidget *dmxMappingTable;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *qlcPluginEdit;
    QPushButton *configurePluginButton;
    QHBoxLayout *pluginLayout;
    QPushButton *saveDmxConfigButton;
    QPushButton *updateLinesButton;
    QSpacerItem *horizontalSpacer_2;
    QWidget *dmxUniverseTab;
    QVBoxLayout *verticalLayout_5;
    QPushButton *addDmxUniverseToTemplateButton;
    QSpacerItem *verticalSpacer_3;
    QWidget *audioTab;
    QGridLayout *gridLayout_2;
    QSpinBox *playlistChannelSpin;
    QCheckBox *prohibitAudioDoubleStartCheck;
    QLabel *mapAudioToChannelLabel1_3;
    QLabel *chanOutputLabel0;
    QSpinBox *mapAudioToUniv4;
    QSpinBox *mapAudioToUniv3;
    QLabel *reactivateAudioLabel;
    QSpinBox *mapAudioToDmx4;
    QLabel *label_2;
    QSpinBox *mapAudioToDmx1;
    QComboBox *slotDeviceCombo1;
    QLabel *mapAudioToChannelLabel1;
    QComboBox *slotDeviceCombo4;
    QLabel *playlistChannelLabel;
    QLabel *label_3;
    QLabel *mapAudioToChannelLabel1_2;
    QSpinBox *reactivateAudioTimeSpin;
    QLabel *mapAudioToChannelLabel1_4;
    QSpinBox *mapAudioToDmx2;
    QLabel *label_5;
    QSpinBox *mapAudioToUniv2;
    QComboBox *slotDeviceCombo2;
    QLineEdit *audioBufferSizeEdit;
    QSpacerItem *verticalSpacer_2;
    QSpinBox *mapAudioToDmx3;
    QComboBox *slotDeviceCombo3;
    QLabel *label_4;
    QSpinBox *mapAudioToUniv1;
    QPushButton *restartAudioVideoSlots;
    QLabel *defaultAudioFadeOutTimeMs;
    QSpinBox *defaultAudioFadeOutMsSpin;
    QWidget *guiTab;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *appStyleLabel;
    QComboBox *appStyleCombo;
    QLabel *dialKnobStyleLabel;
    QComboBox *dialKnobStyleCombo;
    QCheckBox *guiIsDarkCheck;
    QCheckBox *noInterfaceFeedbackCheck;
    QGroupBox *showVolumeDialGroup;
    QGroupBox *enableFFTGroup;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelButton;
    QPushButton *okButton;

    void setupUi(QDialog *SetupWidget)
    {
        if (SetupWidget->objectName().isEmpty())
            SetupWidget->setObjectName(QString::fromUtf8("SetupWidget"));
        SetupWidget->resize(1100, 704);
        verticalLayout = new QVBoxLayout(SetupWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(SetupWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        dmxTab = new QWidget();
        dmxTab->setObjectName(QString::fromUtf8("dmxTab"));
        verticalLayout_3 = new QVBoxLayout(dmxTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        qlcPluginLab = new QLabel(dmxTab);
        qlcPluginLab->setObjectName(QString::fromUtf8("qlcPluginLab"));

        verticalLayout_3->addWidget(qlcPluginLab);

        splitter_2 = new QSplitter(dmxTab);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy);
        splitter_2->setOrientation(Qt::Horizontal);
        splitter_2->setHandleWidth(8);
        splitter_2->setChildrenCollapsible(false);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setHandleWidth(8);
        qlcPluginsList = new QListWidget(splitter);
        qlcPluginsList->setObjectName(QString::fromUtf8("qlcPluginsList"));
        splitter->addWidget(qlcPluginsList);
        dmxMappingTable = new QTableWidget(splitter);
        dmxMappingTable->setObjectName(QString::fromUtf8("dmxMappingTable"));
        splitter->addWidget(dmxMappingTable);
        splitter_2->addWidget(splitter);
        layoutWidget = new QWidget(splitter_2);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        qlcPluginEdit = new QTextEdit(layoutWidget);
        qlcPluginEdit->setObjectName(QString::fromUtf8("qlcPluginEdit"));

        verticalLayout_2->addWidget(qlcPluginEdit);

        configurePluginButton = new QPushButton(layoutWidget);
        configurePluginButton->setObjectName(QString::fromUtf8("configurePluginButton"));

        verticalLayout_2->addWidget(configurePluginButton);

        splitter_2->addWidget(layoutWidget);

        verticalLayout_3->addWidget(splitter_2);

        pluginLayout = new QHBoxLayout();
        pluginLayout->setObjectName(QString::fromUtf8("pluginLayout"));
        pluginLayout->setContentsMargins(-1, 0, -1, -1);
        saveDmxConfigButton = new QPushButton(dmxTab);
        saveDmxConfigButton->setObjectName(QString::fromUtf8("saveDmxConfigButton"));

        pluginLayout->addWidget(saveDmxConfigButton);

        updateLinesButton = new QPushButton(dmxTab);
        updateLinesButton->setObjectName(QString::fromUtf8("updateLinesButton"));

        pluginLayout->addWidget(updateLinesButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        pluginLayout->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(pluginLayout);

        tabWidget->addTab(dmxTab, QString());
        dmxUniverseTab = new QWidget();
        dmxUniverseTab->setObjectName(QString::fromUtf8("dmxUniverseTab"));
        verticalLayout_5 = new QVBoxLayout(dmxUniverseTab);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        addDmxUniverseToTemplateButton = new QPushButton(dmxUniverseTab);
        addDmxUniverseToTemplateButton->setObjectName(QString::fromUtf8("addDmxUniverseToTemplateButton"));

        verticalLayout_5->addWidget(addDmxUniverseToTemplateButton);

        verticalSpacer_3 = new QSpacerItem(20, 479, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_3);

        tabWidget->addTab(dmxUniverseTab, QString());
        audioTab = new QWidget();
        audioTab->setObjectName(QString::fromUtf8("audioTab"));
        gridLayout_2 = new QGridLayout(audioTab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        playlistChannelSpin = new QSpinBox(audioTab);
        playlistChannelSpin->setObjectName(QString::fromUtf8("playlistChannelSpin"));
        playlistChannelSpin->setMinimum(1);
        playlistChannelSpin->setMaximum(4);

        gridLayout_2->addWidget(playlistChannelSpin, 6, 1, 1, 1);

        prohibitAudioDoubleStartCheck = new QCheckBox(audioTab);
        prohibitAudioDoubleStartCheck->setObjectName(QString::fromUtf8("prohibitAudioDoubleStartCheck"));

        gridLayout_2->addWidget(prohibitAudioDoubleStartCheck, 1, 0, 1, 1);

        mapAudioToChannelLabel1_3 = new QLabel(audioTab);
        mapAudioToChannelLabel1_3->setObjectName(QString::fromUtf8("mapAudioToChannelLabel1_3"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_3, 4, 0, 1, 1);

        chanOutputLabel0 = new QLabel(audioTab);
        chanOutputLabel0->setObjectName(QString::fromUtf8("chanOutputLabel0"));

        gridLayout_2->addWidget(chanOutputLabel0, 2, 3, 1, 1);

        mapAudioToUniv4 = new QSpinBox(audioTab);
        mapAudioToUniv4->setObjectName(QString::fromUtf8("mapAudioToUniv4"));
        mapAudioToUniv4->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv4, 5, 1, 1, 1);

        mapAudioToUniv3 = new QSpinBox(audioTab);
        mapAudioToUniv3->setObjectName(QString::fromUtf8("mapAudioToUniv3"));
        mapAudioToUniv3->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv3, 4, 1, 1, 1);

        reactivateAudioLabel = new QLabel(audioTab);
        reactivateAudioLabel->setObjectName(QString::fromUtf8("reactivateAudioLabel"));

        gridLayout_2->addWidget(reactivateAudioLabel, 1, 1, 1, 1);

        mapAudioToDmx4 = new QSpinBox(audioTab);
        mapAudioToDmx4->setObjectName(QString::fromUtf8("mapAudioToDmx4"));
        mapAudioToDmx4->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx4, 5, 2, 1, 1);

        label_2 = new QLabel(audioTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        mapAudioToDmx1 = new QSpinBox(audioTab);
        mapAudioToDmx1->setObjectName(QString::fromUtf8("mapAudioToDmx1"));
        mapAudioToDmx1->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx1, 2, 2, 1, 1);

        slotDeviceCombo1 = new QComboBox(audioTab);
        slotDeviceCombo1->setObjectName(QString::fromUtf8("slotDeviceCombo1"));

        gridLayout_2->addWidget(slotDeviceCombo1, 2, 4, 1, 1);

        mapAudioToChannelLabel1 = new QLabel(audioTab);
        mapAudioToChannelLabel1->setObjectName(QString::fromUtf8("mapAudioToChannelLabel1"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1, 2, 0, 1, 1);

        slotDeviceCombo4 = new QComboBox(audioTab);
        slotDeviceCombo4->setObjectName(QString::fromUtf8("slotDeviceCombo4"));

        gridLayout_2->addWidget(slotDeviceCombo4, 5, 4, 1, 1);

        playlistChannelLabel = new QLabel(audioTab);
        playlistChannelLabel->setObjectName(QString::fromUtf8("playlistChannelLabel"));

        gridLayout_2->addWidget(playlistChannelLabel, 6, 0, 1, 1);

        label_3 = new QLabel(audioTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 3, 3, 1, 1);

        mapAudioToChannelLabel1_2 = new QLabel(audioTab);
        mapAudioToChannelLabel1_2->setObjectName(QString::fromUtf8("mapAudioToChannelLabel1_2"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_2, 3, 0, 1, 1);

        reactivateAudioTimeSpin = new QSpinBox(audioTab);
        reactivateAudioTimeSpin->setObjectName(QString::fromUtf8("reactivateAudioTimeSpin"));
        reactivateAudioTimeSpin->setMaximum(10000);
        reactivateAudioTimeSpin->setSingleStep(25);
        reactivateAudioTimeSpin->setValue(800);

        gridLayout_2->addWidget(reactivateAudioTimeSpin, 1, 2, 1, 1);

        mapAudioToChannelLabel1_4 = new QLabel(audioTab);
        mapAudioToChannelLabel1_4->setObjectName(QString::fromUtf8("mapAudioToChannelLabel1_4"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_4, 5, 0, 1, 1);

        mapAudioToDmx2 = new QSpinBox(audioTab);
        mapAudioToDmx2->setObjectName(QString::fromUtf8("mapAudioToDmx2"));
        mapAudioToDmx2->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx2, 3, 2, 1, 1);

        label_5 = new QLabel(audioTab);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 5, 3, 1, 1);

        mapAudioToUniv2 = new QSpinBox(audioTab);
        mapAudioToUniv2->setObjectName(QString::fromUtf8("mapAudioToUniv2"));
        mapAudioToUniv2->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv2, 3, 1, 1, 1);

        slotDeviceCombo2 = new QComboBox(audioTab);
        slotDeviceCombo2->setObjectName(QString::fromUtf8("slotDeviceCombo2"));

        gridLayout_2->addWidget(slotDeviceCombo2, 3, 4, 1, 1);

        audioBufferSizeEdit = new QLineEdit(audioTab);
        audioBufferSizeEdit->setObjectName(QString::fromUtf8("audioBufferSizeEdit"));

        gridLayout_2->addWidget(audioBufferSizeEdit, 0, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 8, 1, 1, 1);

        mapAudioToDmx3 = new QSpinBox(audioTab);
        mapAudioToDmx3->setObjectName(QString::fromUtf8("mapAudioToDmx3"));
        mapAudioToDmx3->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx3, 4, 2, 1, 1);

        slotDeviceCombo3 = new QComboBox(audioTab);
        slotDeviceCombo3->setObjectName(QString::fromUtf8("slotDeviceCombo3"));

        gridLayout_2->addWidget(slotDeviceCombo3, 4, 4, 1, 1);

        label_4 = new QLabel(audioTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 4, 3, 1, 1);

        mapAudioToUniv1 = new QSpinBox(audioTab);
        mapAudioToUniv1->setObjectName(QString::fromUtf8("mapAudioToUniv1"));
        mapAudioToUniv1->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv1, 2, 1, 1, 1);

        restartAudioVideoSlots = new QPushButton(audioTab);
        restartAudioVideoSlots->setObjectName(QString::fromUtf8("restartAudioVideoSlots"));

        gridLayout_2->addWidget(restartAudioVideoSlots, 7, 3, 1, 2);

        defaultAudioFadeOutTimeMs = new QLabel(audioTab);
        defaultAudioFadeOutTimeMs->setObjectName(QString::fromUtf8("defaultAudioFadeOutTimeMs"));

        gridLayout_2->addWidget(defaultAudioFadeOutTimeMs, 7, 0, 1, 2);

        defaultAudioFadeOutMsSpin = new QSpinBox(audioTab);
        defaultAudioFadeOutMsSpin->setObjectName(QString::fromUtf8("defaultAudioFadeOutMsSpin"));
        defaultAudioFadeOutMsSpin->setMaximum(10000);
        defaultAudioFadeOutMsSpin->setSingleStep(100);
        defaultAudioFadeOutMsSpin->setValue(5000);

        gridLayout_2->addWidget(defaultAudioFadeOutMsSpin, 7, 2, 1, 1);

        tabWidget->addTab(audioTab, QString());
        guiTab = new QWidget();
        guiTab->setObjectName(QString::fromUtf8("guiTab"));
        verticalLayout_4 = new QVBoxLayout(guiTab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        groupBox = new QGroupBox(guiTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        appStyleLabel = new QLabel(groupBox);
        appStyleLabel->setObjectName(QString::fromUtf8("appStyleLabel"));

        gridLayout->addWidget(appStyleLabel, 0, 0, 1, 1);

        appStyleCombo = new QComboBox(groupBox);
        appStyleCombo->setObjectName(QString::fromUtf8("appStyleCombo"));

        gridLayout->addWidget(appStyleCombo, 0, 1, 1, 1);

        dialKnobStyleLabel = new QLabel(groupBox);
        dialKnobStyleLabel->setObjectName(QString::fromUtf8("dialKnobStyleLabel"));

        gridLayout->addWidget(dialKnobStyleLabel, 1, 0, 1, 1);

        dialKnobStyleCombo = new QComboBox(groupBox);
        dialKnobStyleCombo->setObjectName(QString::fromUtf8("dialKnobStyleCombo"));

        gridLayout->addWidget(dialKnobStyleCombo, 1, 1, 1, 1);

        guiIsDarkCheck = new QCheckBox(groupBox);
        guiIsDarkCheck->setObjectName(QString::fromUtf8("guiIsDarkCheck"));

        gridLayout->addWidget(guiIsDarkCheck, 2, 1, 1, 1);


        verticalLayout_4->addWidget(groupBox);

        noInterfaceFeedbackCheck = new QCheckBox(guiTab);
        noInterfaceFeedbackCheck->setObjectName(QString::fromUtf8("noInterfaceFeedbackCheck"));

        verticalLayout_4->addWidget(noInterfaceFeedbackCheck);

        showVolumeDialGroup = new QGroupBox(guiTab);
        showVolumeDialGroup->setObjectName(QString::fromUtf8("showVolumeDialGroup"));

        verticalLayout_4->addWidget(showVolumeDialGroup);

        enableFFTGroup = new QGroupBox(guiTab);
        enableFFTGroup->setObjectName(QString::fromUtf8("enableFFTGroup"));

        verticalLayout_4->addWidget(enableFFTGroup);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(guiTab, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancelButton = new QPushButton(SetupWidget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        okButton = new QPushButton(SetupWidget);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        horizontalLayout->addWidget(okButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SetupWidget);
        QObject::connect(prohibitAudioDoubleStartCheck, SIGNAL(toggled(bool)), reactivateAudioLabel, SLOT(setEnabled(bool)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SetupWidget);
    } // setupUi

    void retranslateUi(QDialog *SetupWidget)
    {
        SetupWidget->setWindowTitle(QCoreApplication::translate("SetupWidget", "StageRunner Setup & Configuration", nullptr));
        qlcPluginLab->setText(QCoreApplication::translate("SetupWidget", "QLC plugins", nullptr));
        configurePluginButton->setText(QCoreApplication::translate("SetupWidget", "Configure plugin", nullptr));
        saveDmxConfigButton->setText(QCoreApplication::translate("SetupWidget", "Save Line Config", nullptr));
        updateLinesButton->setText(QCoreApplication::translate("SetupWidget", "Update Lines", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(dmxTab), QCoreApplication::translate("SetupWidget", "DMX", nullptr));
        addDmxUniverseToTemplateButton->setText(QCoreApplication::translate("SetupWidget", "Add DMX Universe Template to template list", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(dmxUniverseTab), QCoreApplication::translate("SetupWidget", "DMX Universe", nullptr));
        prohibitAudioDoubleStartCheck->setText(QCoreApplication::translate("SetupWidget", "Prohibit double start", nullptr));
        mapAudioToChannelLabel1_3->setText(QCoreApplication::translate("SetupWidget", "Map Audio Slot 3 to DMX", nullptr));
        chanOutputLabel0->setText(QCoreApplication::translate("SetupWidget", "Slot 1 Output device", nullptr));
#if QT_CONFIG(tooltip)
        mapAudioToUniv4->setToolTip(QCoreApplication::translate("SetupWidget", "Universe for sound slot 4", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        mapAudioToUniv3->setToolTip(QCoreApplication::translate("SetupWidget", "Universe for sound slot 3", nullptr));
#endif // QT_CONFIG(tooltip)
        reactivateAudioLabel->setText(QCoreApplication::translate("SetupWidget", "reactivate after (ms)", nullptr));
        label_2->setText(QCoreApplication::translate("SetupWidget", "Audio buffer size (bytes)", nullptr));
#if QT_CONFIG(tooltip)
        mapAudioToDmx1->setToolTip(QCoreApplication::translate("SetupWidget", "DMX Channel", nullptr));
#endif // QT_CONFIG(tooltip)
        mapAudioToChannelLabel1->setText(QCoreApplication::translate("SetupWidget", "Map Audio Slot 1 to DMX", nullptr));
        playlistChannelLabel->setText(QCoreApplication::translate("SetupWidget", "FX Audio playlist default slot", nullptr));
        label_3->setText(QCoreApplication::translate("SetupWidget", "Slot 2 Output device", nullptr));
        mapAudioToChannelLabel1_2->setText(QCoreApplication::translate("SetupWidget", "Map Audio Slot 2 to DMX", nullptr));
        mapAudioToChannelLabel1_4->setText(QCoreApplication::translate("SetupWidget", "Map Audio Slot 4 to DMX", nullptr));
#if QT_CONFIG(tooltip)
        mapAudioToDmx2->setToolTip(QCoreApplication::translate("SetupWidget", "DMX Channel", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("SetupWidget", "Slot 4 Output device", nullptr));
#if QT_CONFIG(tooltip)
        mapAudioToUniv2->setToolTip(QCoreApplication::translate("SetupWidget", "Universe for sound slot 2", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("SetupWidget", "Slot 3 Output device", nullptr));
#if QT_CONFIG(tooltip)
        mapAudioToUniv1->setToolTip(QCoreApplication::translate("SetupWidget", "Universe for sound slot 1", nullptr));
#endif // QT_CONFIG(tooltip)
        restartAudioVideoSlots->setText(QCoreApplication::translate("SetupWidget", "Restart Audio and Video Slots", nullptr));
        defaultAudioFadeOutTimeMs->setText(QCoreApplication::translate("SetupWidget", "Default audio fadeout time [ms]", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(audioTab), QCoreApplication::translate("SetupWidget", "Audio", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SetupWidget", "Interface Style", nullptr));
        appStyleLabel->setText(QCoreApplication::translate("SetupWidget", "Application style", nullptr));
        dialKnobStyleLabel->setText(QCoreApplication::translate("SetupWidget", "Dial knob style", nullptr));
        guiIsDarkCheck->setText(QCoreApplication::translate("SetupWidget", "GUI has dark appearance", nullptr));
        noInterfaceFeedbackCheck->setText(QCoreApplication::translate("SetupWidget", "Enable DMX channel feedback for non existing interfaces", nullptr));
        showVolumeDialGroup->setTitle(QCoreApplication::translate("SetupWidget", "Show Volume Dial", nullptr));
        enableFFTGroup->setTitle(QCoreApplication::translate("SetupWidget", "Enable FFT", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(guiTab), QCoreApplication::translate("SetupWidget", "GUI", nullptr));
        cancelButton->setText(QCoreApplication::translate("SetupWidget", "Cancel", nullptr));
        okButton->setText(QCoreApplication::translate("SetupWidget", "Ok", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SetupWidget: public Ui_SetupWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETUPWIDGET_H

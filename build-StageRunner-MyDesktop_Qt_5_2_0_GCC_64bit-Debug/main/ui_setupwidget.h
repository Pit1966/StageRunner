/********************************************************************************
** Form generated from reading UI file 'setupwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETUPWIDGET_H
#define UI_SETUPWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
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
    QLabel *label;
    QSplitter *splitter_2;
    QSplitter *splitter;
    QListWidget *qlcPluginsList;
    QTableWidget *dmxMappingTable;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *qlcPluginEdit;
    QPushButton *configurePluginButton;
    QWidget *audioTab;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLineEdit *audioBufferSizeEdit;
    QCheckBox *prohibitAudioDoubleStartCheck;
    QLabel *reactivateAudioLabel;
    QSpinBox *reactivateAudioTimeSpin;
    QLabel *mapAudioToChannelLabel1;
    QSpinBox *mapAudioToUniv1;
    QSpinBox *mapAudioToDmx1;
    QLabel *mapAudioToChannelLabel1_2;
    QSpinBox *mapAudioToUniv2;
    QSpinBox *mapAudioToDmx2;
    QLabel *mapAudioToChannelLabel1_3;
    QSpinBox *mapAudioToUniv3;
    QSpinBox *mapAudioToDmx3;
    QLabel *mapAudioToChannelLabel1_4;
    QSpinBox *mapAudioToUniv4;
    QSpinBox *mapAudioToDmx4;
    QSpacerItem *verticalSpacer_2;
    QWidget *guiTab;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *appStyleLabel;
    QComboBox *appStyleCombo;
    QLabel *dialKnobStyleLabel;
    QComboBox *dialKnobStyleCombo;
    QCheckBox *noInterfaceFeedbackCheck;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *saveDmxConfigButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelButton;
    QPushButton *okButton;

    void setupUi(QDialog *SetupWidget)
    {
        if (SetupWidget->objectName().isEmpty())
            SetupWidget->setObjectName(QStringLiteral("SetupWidget"));
        SetupWidget->resize(933, 596);
        verticalLayout = new QVBoxLayout(SetupWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(SetupWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        dmxTab = new QWidget();
        dmxTab->setObjectName(QStringLiteral("dmxTab"));
        verticalLayout_3 = new QVBoxLayout(dmxTab);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(dmxTab);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_3->addWidget(label);

        splitter_2 = new QSplitter(dmxTab);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy);
        splitter_2->setOrientation(Qt::Horizontal);
        splitter_2->setHandleWidth(8);
        splitter_2->setChildrenCollapsible(false);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setHandleWidth(8);
        qlcPluginsList = new QListWidget(splitter);
        qlcPluginsList->setObjectName(QStringLiteral("qlcPluginsList"));
        splitter->addWidget(qlcPluginsList);
        dmxMappingTable = new QTableWidget(splitter);
        dmxMappingTable->setObjectName(QStringLiteral("dmxMappingTable"));
        splitter->addWidget(dmxMappingTable);
        splitter_2->addWidget(splitter);
        layoutWidget = new QWidget(splitter_2);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        qlcPluginEdit = new QTextEdit(layoutWidget);
        qlcPluginEdit->setObjectName(QStringLiteral("qlcPluginEdit"));

        verticalLayout_2->addWidget(qlcPluginEdit);

        configurePluginButton = new QPushButton(layoutWidget);
        configurePluginButton->setObjectName(QStringLiteral("configurePluginButton"));

        verticalLayout_2->addWidget(configurePluginButton);

        splitter_2->addWidget(layoutWidget);

        verticalLayout_3->addWidget(splitter_2);

        tabWidget->addTab(dmxTab, QString());
        audioTab = new QWidget();
        audioTab->setObjectName(QStringLiteral("audioTab"));
        gridLayout_2 = new QGridLayout(audioTab);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_2 = new QLabel(audioTab);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        audioBufferSizeEdit = new QLineEdit(audioTab);
        audioBufferSizeEdit->setObjectName(QStringLiteral("audioBufferSizeEdit"));

        gridLayout_2->addWidget(audioBufferSizeEdit, 0, 2, 1, 1);

        prohibitAudioDoubleStartCheck = new QCheckBox(audioTab);
        prohibitAudioDoubleStartCheck->setObjectName(QStringLiteral("prohibitAudioDoubleStartCheck"));

        gridLayout_2->addWidget(prohibitAudioDoubleStartCheck, 1, 0, 1, 1);

        reactivateAudioLabel = new QLabel(audioTab);
        reactivateAudioLabel->setObjectName(QStringLiteral("reactivateAudioLabel"));

        gridLayout_2->addWidget(reactivateAudioLabel, 1, 1, 1, 1);

        reactivateAudioTimeSpin = new QSpinBox(audioTab);
        reactivateAudioTimeSpin->setObjectName(QStringLiteral("reactivateAudioTimeSpin"));
        reactivateAudioTimeSpin->setMaximum(10000);
        reactivateAudioTimeSpin->setSingleStep(25);
        reactivateAudioTimeSpin->setValue(800);

        gridLayout_2->addWidget(reactivateAudioTimeSpin, 1, 2, 1, 1);

        mapAudioToChannelLabel1 = new QLabel(audioTab);
        mapAudioToChannelLabel1->setObjectName(QStringLiteral("mapAudioToChannelLabel1"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1, 2, 0, 1, 1);

        mapAudioToUniv1 = new QSpinBox(audioTab);
        mapAudioToUniv1->setObjectName(QStringLiteral("mapAudioToUniv1"));
        mapAudioToUniv1->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv1, 2, 1, 1, 1);

        mapAudioToDmx1 = new QSpinBox(audioTab);
        mapAudioToDmx1->setObjectName(QStringLiteral("mapAudioToDmx1"));
        mapAudioToDmx1->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx1, 2, 2, 1, 1);

        mapAudioToChannelLabel1_2 = new QLabel(audioTab);
        mapAudioToChannelLabel1_2->setObjectName(QStringLiteral("mapAudioToChannelLabel1_2"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_2, 3, 0, 1, 1);

        mapAudioToUniv2 = new QSpinBox(audioTab);
        mapAudioToUniv2->setObjectName(QStringLiteral("mapAudioToUniv2"));
        mapAudioToUniv2->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv2, 3, 1, 1, 1);

        mapAudioToDmx2 = new QSpinBox(audioTab);
        mapAudioToDmx2->setObjectName(QStringLiteral("mapAudioToDmx2"));
        mapAudioToDmx2->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx2, 3, 2, 1, 1);

        mapAudioToChannelLabel1_3 = new QLabel(audioTab);
        mapAudioToChannelLabel1_3->setObjectName(QStringLiteral("mapAudioToChannelLabel1_3"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_3, 4, 0, 1, 1);

        mapAudioToUniv3 = new QSpinBox(audioTab);
        mapAudioToUniv3->setObjectName(QStringLiteral("mapAudioToUniv3"));
        mapAudioToUniv3->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv3, 4, 1, 1, 1);

        mapAudioToDmx3 = new QSpinBox(audioTab);
        mapAudioToDmx3->setObjectName(QStringLiteral("mapAudioToDmx3"));
        mapAudioToDmx3->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx3, 4, 2, 1, 1);

        mapAudioToChannelLabel1_4 = new QLabel(audioTab);
        mapAudioToChannelLabel1_4->setObjectName(QStringLiteral("mapAudioToChannelLabel1_4"));

        gridLayout_2->addWidget(mapAudioToChannelLabel1_4, 5, 0, 1, 1);

        mapAudioToUniv4 = new QSpinBox(audioTab);
        mapAudioToUniv4->setObjectName(QStringLiteral("mapAudioToUniv4"));
        mapAudioToUniv4->setMaximum(4);

        gridLayout_2->addWidget(mapAudioToUniv4, 5, 1, 1, 1);

        mapAudioToDmx4 = new QSpinBox(audioTab);
        mapAudioToDmx4->setObjectName(QStringLiteral("mapAudioToDmx4"));
        mapAudioToDmx4->setMaximum(512);

        gridLayout_2->addWidget(mapAudioToDmx4, 5, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 6, 1, 1, 1);

        tabWidget->addTab(audioTab, QString());
        guiTab = new QWidget();
        guiTab->setObjectName(QStringLiteral("guiTab"));
        verticalLayout_4 = new QVBoxLayout(guiTab);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        groupBox = new QGroupBox(guiTab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        appStyleLabel = new QLabel(groupBox);
        appStyleLabel->setObjectName(QStringLiteral("appStyleLabel"));

        gridLayout->addWidget(appStyleLabel, 0, 0, 1, 1);

        appStyleCombo = new QComboBox(groupBox);
        appStyleCombo->setObjectName(QStringLiteral("appStyleCombo"));

        gridLayout->addWidget(appStyleCombo, 0, 1, 1, 1);

        dialKnobStyleLabel = new QLabel(groupBox);
        dialKnobStyleLabel->setObjectName(QStringLiteral("dialKnobStyleLabel"));

        gridLayout->addWidget(dialKnobStyleLabel, 1, 0, 1, 1);

        dialKnobStyleCombo = new QComboBox(groupBox);
        dialKnobStyleCombo->setObjectName(QStringLiteral("dialKnobStyleCombo"));

        gridLayout->addWidget(dialKnobStyleCombo, 1, 1, 1, 1);


        verticalLayout_4->addWidget(groupBox);

        noInterfaceFeedbackCheck = new QCheckBox(guiTab);
        noInterfaceFeedbackCheck->setObjectName(QStringLiteral("noInterfaceFeedbackCheck"));

        verticalLayout_4->addWidget(noInterfaceFeedbackCheck);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(guiTab, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        saveDmxConfigButton = new QPushButton(SetupWidget);
        saveDmxConfigButton->setObjectName(QStringLiteral("saveDmxConfigButton"));

        horizontalLayout->addWidget(saveDmxConfigButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancelButton = new QPushButton(SetupWidget);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);

        okButton = new QPushButton(SetupWidget);
        okButton->setObjectName(QStringLiteral("okButton"));

        horizontalLayout->addWidget(okButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SetupWidget);
        QObject::connect(prohibitAudioDoubleStartCheck, SIGNAL(toggled(bool)), reactivateAudioLabel, SLOT(setEnabled(bool)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SetupWidget);
    } // setupUi

    void retranslateUi(QDialog *SetupWidget)
    {
        SetupWidget->setWindowTitle(QApplication::translate("SetupWidget", "StageRunner Setup & Configuration", 0));
        label->setText(QApplication::translate("SetupWidget", "QLC plugins", 0));
        configurePluginButton->setText(QApplication::translate("SetupWidget", "Configure plugin", 0));
        tabWidget->setTabText(tabWidget->indexOf(dmxTab), QApplication::translate("SetupWidget", "DMX", 0));
        label_2->setText(QApplication::translate("SetupWidget", "Audio buffer size (bytes)", 0));
        prohibitAudioDoubleStartCheck->setText(QApplication::translate("SetupWidget", "Prohibit double start", 0));
        reactivateAudioLabel->setText(QApplication::translate("SetupWidget", "reactivate after (ms)", 0));
        mapAudioToChannelLabel1->setText(QApplication::translate("SetupWidget", "Map Channel 1 to DMX", 0));
        mapAudioToChannelLabel1_2->setText(QApplication::translate("SetupWidget", "Map Channel 2 to DMX", 0));
        mapAudioToChannelLabel1_3->setText(QApplication::translate("SetupWidget", "Map Channel 3 to DMX", 0));
        mapAudioToChannelLabel1_4->setText(QApplication::translate("SetupWidget", "Map Channel 4 to DMX", 0));
        tabWidget->setTabText(tabWidget->indexOf(audioTab), QApplication::translate("SetupWidget", "Audio", 0));
        groupBox->setTitle(QApplication::translate("SetupWidget", "Interface Style", 0));
        appStyleLabel->setText(QApplication::translate("SetupWidget", "Application style", 0));
        dialKnobStyleLabel->setText(QApplication::translate("SetupWidget", "Dial knob style", 0));
        noInterfaceFeedbackCheck->setText(QApplication::translate("SetupWidget", "Enable DMX channel feedback for non existing interfaces", 0));
        tabWidget->setTabText(tabWidget->indexOf(guiTab), QApplication::translate("SetupWidget", "GUI", 0));
        saveDmxConfigButton->setText(QApplication::translate("SetupWidget", "Save DMX Config", 0));
        cancelButton->setText(QApplication::translate("SetupWidget", "Cancel", 0));
        okButton->setText(QApplication::translate("SetupWidget", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class SetupWidget: public Ui_SetupWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETUPWIDGET_H

/********************************************************************************
** Form generated from reading UI file 'scenedeskwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENEDESKWIDGET_H
#define UI_SCENEDESKWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "customwidget/mixergroup.h"
#include "customwidget/pslineedit.h"

QT_BEGIN_NAMESPACE

class Ui_SceneDeskWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QGroupBox *faderGroup;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *scrollAreaLayout;
    MixerGroup *faderAreaWidget;
    QWidget *layoutWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QCheckBox *liveCheck;
    QCheckBox *editCheck;
    QGroupBox *sceneGroup;
    QVBoxLayout *verticalLayout_3;
    PsLineEdit *sceneNameEdit;
    QGroupBox *groupBox;
    QGridLayout *hookedToLayout;
    QLabel *hookedUniverseLabel;
    QSpinBox *hookedUniverseSpin;
    QLabel *hookedChannelLabel;
    QSpinBox *hookedChannelSpin;
    QPushButton *autoHookButton;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *fadeInTimeLabel;
    PsLineEdit *fadeInTimeEdit;
    QLabel *fadeOutTimeLabel;
    PsLineEdit *fadeOutTimeEdit;
    QGroupBox *tubeGroup;
    QGridLayout *tubeGroupLayout;
    QLabel *tubeCommentLabel;
    PsLineEdit *tubeCommentEdit;
    QSpacerItem *verticalSpacer;
    QPushButton *closeButton;

    void setupUi(QWidget *SceneDeskWidget)
    {
        if (SceneDeskWidget->objectName().isEmpty())
            SceneDeskWidget->setObjectName(QStringLiteral("SceneDeskWidget"));
        SceneDeskWidget->resize(1057, 537);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SceneDeskWidget->sizePolicy().hasHeightForWidth());
        SceneDeskWidget->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/gfx/icons/scene_mixer.png"), QSize(), QIcon::Normal, QIcon::Off);
        SceneDeskWidget->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(SceneDeskWidget);
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        splitter = new QSplitter(SceneDeskWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setHandleWidth(6);
        faderGroup = new QGroupBox(splitter);
        faderGroup->setObjectName(QStringLiteral("faderGroup"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(7);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(faderGroup->sizePolicy().hasHeightForWidth());
        faderGroup->setSizePolicy(sizePolicy1);
        faderGroup->setTitle(QStringLiteral("Fader Group"));
        faderGroup->setFlat(false);
        verticalLayout = new QVBoxLayout(faderGroup);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 4, 2, 2);
        scrollArea = new QScrollArea(faderGroup);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Sunken);
        scrollArea->setLineWidth(1);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 834, 501));
        scrollAreaLayout = new QVBoxLayout(scrollAreaWidgetContents);
        scrollAreaLayout->setSpacing(0);
        scrollAreaLayout->setObjectName(QStringLiteral("scrollAreaLayout"));
        scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
        faderAreaWidget = new MixerGroup(scrollAreaWidgetContents);
        faderAreaWidget->setObjectName(QStringLiteral("faderAreaWidget"));
        sizePolicy.setHeightForWidth(faderAreaWidget->sizePolicy().hasHeightForWidth());
        faderAreaWidget->setSizePolicy(sizePolicy);

        scrollAreaLayout->addWidget(faderAreaWidget);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        splitter->addWidget(faderGroup);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        mainLayout = new QVBoxLayout(layoutWidget);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        liveCheck = new QCheckBox(layoutWidget);
        liveCheck->setObjectName(QStringLiteral("liveCheck"));
        liveCheck->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout->addWidget(liveCheck);

        editCheck = new QCheckBox(layoutWidget);
        editCheck->setObjectName(QStringLiteral("editCheck"));

        horizontalLayout->addWidget(editCheck);


        mainLayout->addLayout(horizontalLayout);

        sceneGroup = new QGroupBox(layoutWidget);
        sceneGroup->setObjectName(QStringLiteral("sceneGroup"));
        sizePolicy.setHeightForWidth(sceneGroup->sizePolicy().hasHeightForWidth());
        sceneGroup->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(sceneGroup);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(2, 4, 2, 2);
        sceneNameEdit = new PsLineEdit(sceneGroup);
        sceneNameEdit->setObjectName(QStringLiteral("sceneNameEdit"));

        verticalLayout_3->addWidget(sceneNameEdit);

        groupBox = new QGroupBox(sceneGroup);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        hookedToLayout = new QGridLayout(groupBox);
        hookedToLayout->setObjectName(QStringLiteral("hookedToLayout"));
        hookedToLayout->setHorizontalSpacing(2);
        hookedToLayout->setVerticalSpacing(1);
        hookedToLayout->setContentsMargins(2, 2, 2, 2);
        hookedUniverseLabel = new QLabel(groupBox);
        hookedUniverseLabel->setObjectName(QStringLiteral("hookedUniverseLabel"));

        hookedToLayout->addWidget(hookedUniverseLabel, 0, 0, 1, 1);

        hookedUniverseSpin = new QSpinBox(groupBox);
        hookedUniverseSpin->setObjectName(QStringLiteral("hookedUniverseSpin"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(2);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(hookedUniverseSpin->sizePolicy().hasHeightForWidth());
        hookedUniverseSpin->setSizePolicy(sizePolicy2);
        hookedUniverseSpin->setFrame(false);
        hookedUniverseSpin->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        hookedUniverseSpin->setMinimum(1);
        hookedUniverseSpin->setMaximum(4);
        hookedUniverseSpin->setValue(1);

        hookedToLayout->addWidget(hookedUniverseSpin, 0, 1, 1, 1);

        hookedChannelLabel = new QLabel(groupBox);
        hookedChannelLabel->setObjectName(QStringLiteral("hookedChannelLabel"));

        hookedToLayout->addWidget(hookedChannelLabel, 1, 0, 1, 1);

        hookedChannelSpin = new QSpinBox(groupBox);
        hookedChannelSpin->setObjectName(QStringLiteral("hookedChannelSpin"));
        hookedChannelSpin->setFrame(false);
        hookedChannelSpin->setMaximum(512);

        hookedToLayout->addWidget(hookedChannelSpin, 1, 1, 1, 1);

        autoHookButton = new QPushButton(groupBox);
        autoHookButton->setObjectName(QStringLiteral("autoHookButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(autoHookButton->sizePolicy().hasHeightForWidth());
        autoHookButton->setSizePolicy(sizePolicy3);

        hookedToLayout->addWidget(autoHookButton, 0, 2, 2, 1);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(sceneGroup);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(2);
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        fadeInTimeLabel = new QLabel(groupBox_2);
        fadeInTimeLabel->setObjectName(QStringLiteral("fadeInTimeLabel"));

        gridLayout->addWidget(fadeInTimeLabel, 0, 0, 1, 1);

        fadeInTimeEdit = new PsLineEdit(groupBox_2);
        fadeInTimeEdit->setObjectName(QStringLiteral("fadeInTimeEdit"));
        fadeInTimeEdit->setFrame(false);

        gridLayout->addWidget(fadeInTimeEdit, 0, 1, 1, 1);

        fadeOutTimeLabel = new QLabel(groupBox_2);
        fadeOutTimeLabel->setObjectName(QStringLiteral("fadeOutTimeLabel"));

        gridLayout->addWidget(fadeOutTimeLabel, 1, 0, 1, 1);

        fadeOutTimeEdit = new PsLineEdit(groupBox_2);
        fadeOutTimeEdit->setObjectName(QStringLiteral("fadeOutTimeEdit"));
        fadeOutTimeEdit->setFrame(false);

        gridLayout->addWidget(fadeOutTimeEdit, 1, 1, 1, 1);


        verticalLayout_3->addWidget(groupBox_2);


        mainLayout->addWidget(sceneGroup);

        tubeGroup = new QGroupBox(layoutWidget);
        tubeGroup->setObjectName(QStringLiteral("tubeGroup"));
        sizePolicy.setHeightForWidth(tubeGroup->sizePolicy().hasHeightForWidth());
        tubeGroup->setSizePolicy(sizePolicy);
        tubeGroupLayout = new QGridLayout(tubeGroup);
        tubeGroupLayout->setObjectName(QStringLiteral("tubeGroupLayout"));
        tubeGroupLayout->setVerticalSpacing(1);
        tubeGroupLayout->setContentsMargins(2, 2, 2, 2);
        tubeCommentLabel = new QLabel(tubeGroup);
        tubeCommentLabel->setObjectName(QStringLiteral("tubeCommentLabel"));

        tubeGroupLayout->addWidget(tubeCommentLabel, 0, 0, 1, 1);

        tubeCommentEdit = new PsLineEdit(tubeGroup);
        tubeCommentEdit->setObjectName(QStringLiteral("tubeCommentEdit"));
        tubeCommentEdit->setFrame(false);

        tubeGroupLayout->addWidget(tubeCommentEdit, 0, 1, 1, 1);


        mainLayout->addWidget(tubeGroup);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mainLayout->addItem(verticalSpacer);

        closeButton = new QPushButton(layoutWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        mainLayout->addWidget(closeButton);

        splitter->addWidget(layoutWidget);

        verticalLayout_2->addWidget(splitter);


        retranslateUi(SceneDeskWidget);
        QObject::connect(closeButton, SIGNAL(clicked()), SceneDeskWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(SceneDeskWidget);
    } // setupUi

    void retranslateUi(QWidget *SceneDeskWidget)
    {
        SceneDeskWidget->setWindowTitle(QApplication::translate("SceneDeskWidget", "Scene", 0));
        liveCheck->setText(QApplication::translate("SceneDeskWidget", "LIVE", 0));
        editCheck->setText(QApplication::translate("SceneDeskWidget", "EDIT", 0));
        sceneGroup->setTitle(QApplication::translate("SceneDeskWidget", "Scene", 0));
        groupBox->setTitle(QApplication::translate("SceneDeskWidget", "Hooked to", 0));
        hookedUniverseLabel->setText(QApplication::translate("SceneDeskWidget", "Universe", 0));
        hookedChannelLabel->setText(QApplication::translate("SceneDeskWidget", "Channel", 0));
        autoHookButton->setText(QApplication::translate("SceneDeskWidget", "Auto", 0));
        groupBox_2->setTitle(QApplication::translate("SceneDeskWidget", "Fade times", 0));
        fadeInTimeLabel->setText(QApplication::translate("SceneDeskWidget", "IN", 0));
        fadeOutTimeLabel->setText(QApplication::translate("SceneDeskWidget", "OUT", 0));
        tubeGroup->setTitle(QApplication::translate("SceneDeskWidget", "Tube / Channel", 0));
        tubeCommentLabel->setText(QApplication::translate("SceneDeskWidget", "Comment ", 0));
        closeButton->setText(QApplication::translate("SceneDeskWidget", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class SceneDeskWidget: public Ui_SceneDeskWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENEDESKWIDGET_H

/********************************************************************************
** Form generated from reading UI file 'scenedeskwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENEDESKWIDGET_H
#define UI_SCENEDESKWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
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
#include "customwidget/psspinbox.h"

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
    PsLineEdit *tubeCommentEdit;
    QLabel *tubeCommentLabel;
    QLabel *channelsLabel;
    PsSpinBox *channelCountSpin;
    QPushButton *cloneCurrentInputButton;
    QPushButton *cloneCurrentOutputButton;
    QSpacerItem *verticalSpacer;
    QPushButton *closeButton;

    void setupUi(QWidget *SceneDeskWidget)
    {
        if (SceneDeskWidget->objectName().isEmpty())
            SceneDeskWidget->setObjectName(QString::fromUtf8("SceneDeskWidget"));
        SceneDeskWidget->resize(1057, 537);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SceneDeskWidget->sizePolicy().hasHeightForWidth());
        SceneDeskWidget->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/scene_mixer.png"), QSize(), QIcon::Normal, QIcon::Off);
        SceneDeskWidget->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(SceneDeskWidget);
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        splitter = new QSplitter(SceneDeskWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setHandleWidth(6);
        faderGroup = new QGroupBox(splitter);
        faderGroup->setObjectName(QString::fromUtf8("faderGroup"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(7);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(faderGroup->sizePolicy().hasHeightForWidth());
        faderGroup->setSizePolicy(sizePolicy1);
        faderGroup->setTitle(QString::fromUtf8("Fader Group"));
        faderGroup->setFlat(false);
        verticalLayout = new QVBoxLayout(faderGroup);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(2, 4, 2, 2);
        scrollArea = new QScrollArea(faderGroup);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setFrameShadow(QFrame::Sunken);
        scrollArea->setLineWidth(1);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 827, 498));
        scrollAreaLayout = new QVBoxLayout(scrollAreaWidgetContents);
        scrollAreaLayout->setSpacing(0);
        scrollAreaLayout->setObjectName(QString::fromUtf8("scrollAreaLayout"));
        scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
        faderAreaWidget = new MixerGroup(scrollAreaWidgetContents);
        faderAreaWidget->setObjectName(QString::fromUtf8("faderAreaWidget"));
        sizePolicy.setHeightForWidth(faderAreaWidget->sizePolicy().hasHeightForWidth());
        faderAreaWidget->setSizePolicy(sizePolicy);

        scrollAreaLayout->addWidget(faderAreaWidget);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        splitter->addWidget(faderGroup);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        mainLayout = new QVBoxLayout(layoutWidget);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        liveCheck = new QCheckBox(layoutWidget);
        liveCheck->setObjectName(QString::fromUtf8("liveCheck"));
        liveCheck->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout->addWidget(liveCheck);

        editCheck = new QCheckBox(layoutWidget);
        editCheck->setObjectName(QString::fromUtf8("editCheck"));

        horizontalLayout->addWidget(editCheck);


        mainLayout->addLayout(horizontalLayout);

        sceneGroup = new QGroupBox(layoutWidget);
        sceneGroup->setObjectName(QString::fromUtf8("sceneGroup"));
        sizePolicy.setHeightForWidth(sceneGroup->sizePolicy().hasHeightForWidth());
        sceneGroup->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(sceneGroup);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(2, 4, 2, 2);
        sceneNameEdit = new PsLineEdit(sceneGroup);
        sceneNameEdit->setObjectName(QString::fromUtf8("sceneNameEdit"));

        verticalLayout_3->addWidget(sceneNameEdit);

        groupBox = new QGroupBox(sceneGroup);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        hookedToLayout = new QGridLayout(groupBox);
        hookedToLayout->setObjectName(QString::fromUtf8("hookedToLayout"));
        hookedToLayout->setHorizontalSpacing(2);
        hookedToLayout->setVerticalSpacing(1);
        hookedToLayout->setContentsMargins(2, 2, 2, 2);
        hookedUniverseLabel = new QLabel(groupBox);
        hookedUniverseLabel->setObjectName(QString::fromUtf8("hookedUniverseLabel"));

        hookedToLayout->addWidget(hookedUniverseLabel, 0, 0, 1, 1);

        hookedUniverseSpin = new QSpinBox(groupBox);
        hookedUniverseSpin->setObjectName(QString::fromUtf8("hookedUniverseSpin"));
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
        hookedChannelLabel->setObjectName(QString::fromUtf8("hookedChannelLabel"));

        hookedToLayout->addWidget(hookedChannelLabel, 1, 0, 1, 1);

        hookedChannelSpin = new QSpinBox(groupBox);
        hookedChannelSpin->setObjectName(QString::fromUtf8("hookedChannelSpin"));
        hookedChannelSpin->setFrame(false);
        hookedChannelSpin->setMaximum(512);

        hookedToLayout->addWidget(hookedChannelSpin, 1, 1, 1, 1);

        autoHookButton = new QPushButton(groupBox);
        autoHookButton->setObjectName(QString::fromUtf8("autoHookButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(autoHookButton->sizePolicy().hasHeightForWidth());
        autoHookButton->setSizePolicy(sizePolicy3);

        hookedToLayout->addWidget(autoHookButton, 0, 2, 2, 1);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(sceneGroup);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(2);
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        fadeInTimeLabel = new QLabel(groupBox_2);
        fadeInTimeLabel->setObjectName(QString::fromUtf8("fadeInTimeLabel"));

        gridLayout->addWidget(fadeInTimeLabel, 0, 0, 1, 1);

        fadeInTimeEdit = new PsLineEdit(groupBox_2);
        fadeInTimeEdit->setObjectName(QString::fromUtf8("fadeInTimeEdit"));
        fadeInTimeEdit->setFrame(false);

        gridLayout->addWidget(fadeInTimeEdit, 0, 1, 1, 1);

        fadeOutTimeLabel = new QLabel(groupBox_2);
        fadeOutTimeLabel->setObjectName(QString::fromUtf8("fadeOutTimeLabel"));

        gridLayout->addWidget(fadeOutTimeLabel, 1, 0, 1, 1);

        fadeOutTimeEdit = new PsLineEdit(groupBox_2);
        fadeOutTimeEdit->setObjectName(QString::fromUtf8("fadeOutTimeEdit"));
        fadeOutTimeEdit->setFrame(false);

        gridLayout->addWidget(fadeOutTimeEdit, 1, 1, 1, 1);


        verticalLayout_3->addWidget(groupBox_2);


        mainLayout->addWidget(sceneGroup);

        tubeGroup = new QGroupBox(layoutWidget);
        tubeGroup->setObjectName(QString::fromUtf8("tubeGroup"));
        sizePolicy.setHeightForWidth(tubeGroup->sizePolicy().hasHeightForWidth());
        tubeGroup->setSizePolicy(sizePolicy);
        tubeGroupLayout = new QGridLayout(tubeGroup);
        tubeGroupLayout->setObjectName(QString::fromUtf8("tubeGroupLayout"));
        tubeGroupLayout->setVerticalSpacing(1);
        tubeGroupLayout->setContentsMargins(2, 2, 2, 2);
        tubeCommentEdit = new PsLineEdit(tubeGroup);
        tubeCommentEdit->setObjectName(QString::fromUtf8("tubeCommentEdit"));
        tubeCommentEdit->setFrame(false);

        tubeGroupLayout->addWidget(tubeCommentEdit, 1, 1, 1, 1);

        tubeCommentLabel = new QLabel(tubeGroup);
        tubeCommentLabel->setObjectName(QString::fromUtf8("tubeCommentLabel"));

        tubeGroupLayout->addWidget(tubeCommentLabel, 1, 0, 1, 1);

        channelsLabel = new QLabel(tubeGroup);
        channelsLabel->setObjectName(QString::fromUtf8("channelsLabel"));

        tubeGroupLayout->addWidget(channelsLabel, 0, 0, 1, 1);

        channelCountSpin = new PsSpinBox(tubeGroup);
        channelCountSpin->setObjectName(QString::fromUtf8("channelCountSpin"));
        channelCountSpin->setMinimum(2);
        channelCountSpin->setMaximum(512);
        channelCountSpin->setValue(12);

        tubeGroupLayout->addWidget(channelCountSpin, 0, 1, 1, 1);


        mainLayout->addWidget(tubeGroup);

        cloneCurrentInputButton = new QPushButton(layoutWidget);
        cloneCurrentInputButton->setObjectName(QString::fromUtf8("cloneCurrentInputButton"));

        mainLayout->addWidget(cloneCurrentInputButton);

        cloneCurrentOutputButton = new QPushButton(layoutWidget);
        cloneCurrentOutputButton->setObjectName(QString::fromUtf8("cloneCurrentOutputButton"));

        mainLayout->addWidget(cloneCurrentOutputButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mainLayout->addItem(verticalSpacer);

        closeButton = new QPushButton(layoutWidget);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        mainLayout->addWidget(closeButton);

        splitter->addWidget(layoutWidget);

        verticalLayout_2->addWidget(splitter);


        retranslateUi(SceneDeskWidget);
        QObject::connect(closeButton, SIGNAL(clicked()), SceneDeskWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(SceneDeskWidget);
    } // setupUi

    void retranslateUi(QWidget *SceneDeskWidget)
    {
        SceneDeskWidget->setWindowTitle(QCoreApplication::translate("SceneDeskWidget", "Scene", nullptr));
        liveCheck->setText(QCoreApplication::translate("SceneDeskWidget", "LIVE", nullptr));
        editCheck->setText(QCoreApplication::translate("SceneDeskWidget", "EDIT", nullptr));
        sceneGroup->setTitle(QCoreApplication::translate("SceneDeskWidget", "Scene", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SceneDeskWidget", "Hooked to", nullptr));
        hookedUniverseLabel->setText(QCoreApplication::translate("SceneDeskWidget", "Universe", nullptr));
        hookedChannelLabel->setText(QCoreApplication::translate("SceneDeskWidget", "Channel", nullptr));
        autoHookButton->setText(QCoreApplication::translate("SceneDeskWidget", "Auto", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("SceneDeskWidget", "Fade times", nullptr));
        fadeInTimeLabel->setText(QCoreApplication::translate("SceneDeskWidget", "IN", nullptr));
        fadeOutTimeLabel->setText(QCoreApplication::translate("SceneDeskWidget", "OUT", nullptr));
        tubeGroup->setTitle(QCoreApplication::translate("SceneDeskWidget", "Tube / Channel", nullptr));
        tubeCommentLabel->setText(QCoreApplication::translate("SceneDeskWidget", "Comment ", nullptr));
        channelsLabel->setText(QCoreApplication::translate("SceneDeskWidget", "Channels", nullptr));
#if QT_CONFIG(tooltip)
        cloneCurrentInputButton->setToolTip(QCoreApplication::translate("SceneDeskWidget", "Fill scene with current universe input values", nullptr));
#endif // QT_CONFIG(tooltip)
        cloneCurrentInputButton->setText(QCoreApplication::translate("SceneDeskWidget", "Clone from input", nullptr));
#if QT_CONFIG(tooltip)
        cloneCurrentOutputButton->setToolTip(QCoreApplication::translate("SceneDeskWidget", "Fill scene from current universe output", nullptr));
#endif // QT_CONFIG(tooltip)
        cloneCurrentOutputButton->setText(QCoreApplication::translate("SceneDeskWidget", "Clone from output", nullptr));
        closeButton->setText(QCoreApplication::translate("SceneDeskWidget", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneDeskWidget: public Ui_SceneDeskWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENEDESKWIDGET_H

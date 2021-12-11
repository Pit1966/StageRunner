/********************************************************************************
** Form generated from reading UI file 'fxitempropertywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FXITEMPROPERTYWIDGET_H
#define UI_FXITEMPROPERTYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "customwidget/pslineedit.h"
#include "customwidget/pspushbutton.h"
#include "thirdparty/widget/qsynthknob.h"

QT_BEGIN_NAMESPACE

class Ui_FxItemPropertyWidget
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *fxGroup;
    QGridLayout *gridLayout;
    QLabel *nameLabel;
    QGridLayout *fadeTimeLayout;
    QLabel *postDelayLabel;
    PsLineEdit *fadeOutTimeEdit;
    PsLineEdit *preDelayEdit;
    QLabel *preDelayLabel;
    QLabel *fadeInTimeLabel;
    PsLineEdit *fadeInTimeEdit;
    QLabel *fadeOutTimeLabel;
    QLabel *holdTimeLabel;
    PsLineEdit *holdTimeEdit;
    PsLineEdit *postDelayEdit;
    QPushButton *keyClearButton;
    PsLineEdit *idEdit;
    QLabel *idLabel;
    PsLineEdit *keyEdit;
    QLabel *keyLabel;
    PsLineEdit *nameEdit;
    PsPushButton *editOnceButton;
    QGroupBox *hookedToGroup;
    QGridLayout *gridLayout_4;
    QLabel *hookedToUniverseLabel;
    QLabel *hookedToChannelLabel;
    QSpinBox *hookedToUniverseSpin;
    QSpinBox *hookedToChannelSpin;
    QGroupBox *videoGroup;
    QGridLayout *videoGroupLayout;
    QLabel *videoLoopsLabel;
    QSpinBox *videoLoopsSpin;
    QLabel *videoFilePathLabel;
    PsLineEdit *videoFilePathEdit;
    QLabel *videoInitialVolumeLabel;
    qsynthKnob *videoInitialVolDial;
    QCheckBox *videoBlackAtEndCheck;
    QGroupBox *audioGroup;
    QGridLayout *gridLayout_2;
    QSpinBox *audioLoopsSpin;
    QLabel *audioSlotLabel;
    QHBoxLayout *onStopLayout;
    QComboBox *audioOnStopCombo;
    PsLineEdit *audioOnStopEdit;
    QLabel *audioStartAtLabel;
    QLabel *audioOnStartLabel;
    QLabel *audioOnStopLabel;
    PsLineEdit *audioStartAtEdit;
    PsLineEdit *audioStopAtEdit;
    QHBoxLayout *onStartLayout;
    QComboBox *audioOnStartCombo;
    PsLineEdit *audioOnStartEdit;
    QLabel *audioInitVolLabel;
    QLabel *audioFilePathLabel;
    QHBoxLayout *initVolLayout;
    qsynthKnob *initialVolDial;
    QPushButton *setToCurrentVolButton;
    QLabel *audioStopAtLabel;
    QLabel *audioLoopsLabel;
    QSpinBox *audioSlotSpin;
    QHBoxLayout *horizontalLayout;
    PsLineEdit *audioFilePathEdit;
    QPushButton *findAudioFileButton;
    QGroupBox *sceneGroup;
    QGridLayout *gridLayout_3;
    PsLineEdit *faderCountEdit;
    QLabel *faderCountLabel;
    QPushButton *closeButton;
    QGroupBox *sequenceGroup;
    QGridLayout *gridLayout_5;
    QCheckBox *seqStopOtherCheck;
    QCheckBox *seqBlackOtherCheck;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *FxItemPropertyWidget)
    {
        if (FxItemPropertyWidget->objectName().isEmpty())
            FxItemPropertyWidget->setObjectName(QString::fromUtf8("FxItemPropertyWidget"));
        FxItemPropertyWidget->resize(335, 1011);
        QFont font;
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        FxItemPropertyWidget->setFont(font);
        FxItemPropertyWidget->setAutoFillBackground(false);
        verticalLayout = new QVBoxLayout(FxItemPropertyWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        fxGroup = new QGroupBox(FxItemPropertyWidget);
        fxGroup->setObjectName(QString::fromUtf8("fxGroup"));
        gridLayout = new QGridLayout(fxGroup);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 2, 2, 2);
        nameLabel = new QLabel(fxGroup);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        gridLayout->addWidget(nameLabel, 2, 0, 1, 1);

        fadeTimeLayout = new QGridLayout();
        fadeTimeLayout->setObjectName(QString::fromUtf8("fadeTimeLayout"));
        fadeTimeLayout->setVerticalSpacing(0);
        fadeTimeLayout->setContentsMargins(-1, 0, -1, -1);
        postDelayLabel = new QLabel(fxGroup);
        postDelayLabel->setObjectName(QString::fromUtf8("postDelayLabel"));

        fadeTimeLayout->addWidget(postDelayLabel, 4, 0, 1, 1);

        fadeOutTimeEdit = new PsLineEdit(fxGroup);
        fadeOutTimeEdit->setObjectName(QString::fromUtf8("fadeOutTimeEdit"));
        fadeOutTimeEdit->setFrame(false);

        fadeTimeLayout->addWidget(fadeOutTimeEdit, 3, 1, 1, 1);

        preDelayEdit = new PsLineEdit(fxGroup);
        preDelayEdit->setObjectName(QString::fromUtf8("preDelayEdit"));
        preDelayEdit->setFrame(false);

        fadeTimeLayout->addWidget(preDelayEdit, 0, 1, 1, 1);

        preDelayLabel = new QLabel(fxGroup);
        preDelayLabel->setObjectName(QString::fromUtf8("preDelayLabel"));

        fadeTimeLayout->addWidget(preDelayLabel, 0, 0, 1, 1);

        fadeInTimeLabel = new QLabel(fxGroup);
        fadeInTimeLabel->setObjectName(QString::fromUtf8("fadeInTimeLabel"));

        fadeTimeLayout->addWidget(fadeInTimeLabel, 1, 0, 1, 1);

        fadeInTimeEdit = new PsLineEdit(fxGroup);
        fadeInTimeEdit->setObjectName(QString::fromUtf8("fadeInTimeEdit"));
        fadeInTimeEdit->setFrame(false);

        fadeTimeLayout->addWidget(fadeInTimeEdit, 1, 1, 1, 1);

        fadeOutTimeLabel = new QLabel(fxGroup);
        fadeOutTimeLabel->setObjectName(QString::fromUtf8("fadeOutTimeLabel"));

        fadeTimeLayout->addWidget(fadeOutTimeLabel, 3, 0, 1, 1);

        holdTimeLabel = new QLabel(fxGroup);
        holdTimeLabel->setObjectName(QString::fromUtf8("holdTimeLabel"));

        fadeTimeLayout->addWidget(holdTimeLabel, 2, 0, 1, 1);

        holdTimeEdit = new PsLineEdit(fxGroup);
        holdTimeEdit->setObjectName(QString::fromUtf8("holdTimeEdit"));
        holdTimeEdit->setFrame(false);

        fadeTimeLayout->addWidget(holdTimeEdit, 2, 1, 1, 1);

        postDelayEdit = new PsLineEdit(fxGroup);
        postDelayEdit->setObjectName(QString::fromUtf8("postDelayEdit"));
        postDelayEdit->setFrame(false);

        fadeTimeLayout->addWidget(postDelayEdit, 4, 1, 1, 1);


        gridLayout->addLayout(fadeTimeLayout, 5, 0, 1, 3);

        keyClearButton = new QPushButton(fxGroup);
        keyClearButton->setObjectName(QString::fromUtf8("keyClearButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(keyClearButton->sizePolicy().hasHeightForWidth());
        keyClearButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(keyClearButton, 3, 2, 1, 1);

        idEdit = new PsLineEdit(fxGroup);
        idEdit->setObjectName(QString::fromUtf8("idEdit"));
        idEdit->setMinimumSize(QSize(60, 0));
        idEdit->setReadOnly(true);

        gridLayout->addWidget(idEdit, 4, 1, 1, 1);

        idLabel = new QLabel(fxGroup);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));

        gridLayout->addWidget(idLabel, 4, 0, 1, 1);

        keyEdit = new PsLineEdit(fxGroup);
        keyEdit->setObjectName(QString::fromUtf8("keyEdit"));
        keyEdit->setMinimumSize(QSize(60, 0));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        keyEdit->setFont(font1);
        keyEdit->setMaxLength(10);
        keyEdit->setFrame(true);

        gridLayout->addWidget(keyEdit, 3, 1, 1, 1);

        keyLabel = new QLabel(fxGroup);
        keyLabel->setObjectName(QString::fromUtf8("keyLabel"));

        gridLayout->addWidget(keyLabel, 3, 0, 1, 1);

        nameEdit = new PsLineEdit(fxGroup);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        gridLayout->addWidget(nameEdit, 2, 1, 1, 2);

        editOnceButton = new PsPushButton(fxGroup);
        editOnceButton->setObjectName(QString::fromUtf8("editOnceButton"));

        gridLayout->addWidget(editOnceButton, 1, 0, 1, 3);


        verticalLayout->addWidget(fxGroup);

        hookedToGroup = new QGroupBox(FxItemPropertyWidget);
        hookedToGroup->setObjectName(QString::fromUtf8("hookedToGroup"));
        gridLayout_4 = new QGridLayout(hookedToGroup);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setVerticalSpacing(0);
        gridLayout_4->setContentsMargins(2, 2, 2, 2);
        hookedToUniverseLabel = new QLabel(hookedToGroup);
        hookedToUniverseLabel->setObjectName(QString::fromUtf8("hookedToUniverseLabel"));

        gridLayout_4->addWidget(hookedToUniverseLabel, 0, 0, 1, 1);

        hookedToChannelLabel = new QLabel(hookedToGroup);
        hookedToChannelLabel->setObjectName(QString::fromUtf8("hookedToChannelLabel"));

        gridLayout_4->addWidget(hookedToChannelLabel, 1, 0, 1, 1);

        hookedToUniverseSpin = new QSpinBox(hookedToGroup);
        hookedToUniverseSpin->setObjectName(QString::fromUtf8("hookedToUniverseSpin"));
        hookedToUniverseSpin->setMaximum(4);

        gridLayout_4->addWidget(hookedToUniverseSpin, 0, 1, 1, 1);

        hookedToChannelSpin = new QSpinBox(hookedToGroup);
        hookedToChannelSpin->setObjectName(QString::fromUtf8("hookedToChannelSpin"));
        hookedToChannelSpin->setMaximum(512);

        gridLayout_4->addWidget(hookedToChannelSpin, 1, 1, 1, 1);


        verticalLayout->addWidget(hookedToGroup);

        videoGroup = new QGroupBox(FxItemPropertyWidget);
        videoGroup->setObjectName(QString::fromUtf8("videoGroup"));
        videoGroupLayout = new QGridLayout(videoGroup);
        videoGroupLayout->setObjectName(QString::fromUtf8("videoGroupLayout"));
        videoGroupLayout->setContentsMargins(2, 2, 2, 2);
        videoLoopsLabel = new QLabel(videoGroup);
        videoLoopsLabel->setObjectName(QString::fromUtf8("videoLoopsLabel"));

        videoGroupLayout->addWidget(videoLoopsLabel, 1, 0, 1, 1);

        videoLoopsSpin = new QSpinBox(videoGroup);
        videoLoopsSpin->setObjectName(QString::fromUtf8("videoLoopsSpin"));
        videoLoopsSpin->setMinimum(-1);
        videoLoopsSpin->setMaximum(999);

        videoGroupLayout->addWidget(videoLoopsSpin, 1, 1, 1, 2);

        videoFilePathLabel = new QLabel(videoGroup);
        videoFilePathLabel->setObjectName(QString::fromUtf8("videoFilePathLabel"));

        videoGroupLayout->addWidget(videoFilePathLabel, 0, 0, 1, 1);

        videoFilePathEdit = new PsLineEdit(videoGroup);
        videoFilePathEdit->setObjectName(QString::fromUtf8("videoFilePathEdit"));

        videoGroupLayout->addWidget(videoFilePathEdit, 0, 1, 1, 2);

        videoInitialVolumeLabel = new QLabel(videoGroup);
        videoInitialVolumeLabel->setObjectName(QString::fromUtf8("videoInitialVolumeLabel"));

        videoGroupLayout->addWidget(videoInitialVolumeLabel, 3, 0, 1, 1);

        videoInitialVolDial = new qsynthKnob(videoGroup);
        videoInitialVolDial->setObjectName(QString::fromUtf8("videoInitialVolDial"));

        videoGroupLayout->addWidget(videoInitialVolDial, 3, 1, 1, 1);

        videoBlackAtEndCheck = new QCheckBox(videoGroup);
        videoBlackAtEndCheck->setObjectName(QString::fromUtf8("videoBlackAtEndCheck"));

        videoGroupLayout->addWidget(videoBlackAtEndCheck, 3, 2, 1, 1);


        verticalLayout->addWidget(videoGroup);

        audioGroup = new QGroupBox(FxItemPropertyWidget);
        audioGroup->setObjectName(QString::fromUtf8("audioGroup"));
        gridLayout_2 = new QGridLayout(audioGroup);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(0);
        gridLayout_2->setContentsMargins(2, 2, 2, 2);
        audioLoopsSpin = new QSpinBox(audioGroup);
        audioLoopsSpin->setObjectName(QString::fromUtf8("audioLoopsSpin"));
        audioLoopsSpin->setMinimum(-1);
        audioLoopsSpin->setMaximum(999);

        gridLayout_2->addWidget(audioLoopsSpin, 4, 1, 1, 1);

        audioSlotLabel = new QLabel(audioGroup);
        audioSlotLabel->setObjectName(QString::fromUtf8("audioSlotLabel"));

        gridLayout_2->addWidget(audioSlotLabel, 3, 0, 1, 1);

        onStopLayout = new QHBoxLayout();
        onStopLayout->setObjectName(QString::fromUtf8("onStopLayout"));
        audioOnStopCombo = new QComboBox(audioGroup);
        audioOnStopCombo->setObjectName(QString::fromUtf8("audioOnStopCombo"));

        onStopLayout->addWidget(audioOnStopCombo);

        audioOnStopEdit = new PsLineEdit(audioGroup);
        audioOnStopEdit->setObjectName(QString::fromUtf8("audioOnStopEdit"));
        audioOnStopEdit->setMinimumSize(QSize(60, 0));

        onStopLayout->addWidget(audioOnStopEdit);


        gridLayout_2->addLayout(onStopLayout, 10, 1, 1, 1);

        audioStartAtLabel = new QLabel(audioGroup);
        audioStartAtLabel->setObjectName(QString::fromUtf8("audioStartAtLabel"));

        gridLayout_2->addWidget(audioStartAtLabel, 7, 0, 1, 1);

        audioOnStartLabel = new QLabel(audioGroup);
        audioOnStartLabel->setObjectName(QString::fromUtf8("audioOnStartLabel"));

        gridLayout_2->addWidget(audioOnStartLabel, 9, 0, 1, 1);

        audioOnStopLabel = new QLabel(audioGroup);
        audioOnStopLabel->setObjectName(QString::fromUtf8("audioOnStopLabel"));

        gridLayout_2->addWidget(audioOnStopLabel, 10, 0, 1, 1);

        audioStartAtEdit = new PsLineEdit(audioGroup);
        audioStartAtEdit->setObjectName(QString::fromUtf8("audioStartAtEdit"));
        audioStartAtEdit->setFrame(false);

        gridLayout_2->addWidget(audioStartAtEdit, 7, 1, 1, 1);

        audioStopAtEdit = new PsLineEdit(audioGroup);
        audioStopAtEdit->setObjectName(QString::fromUtf8("audioStopAtEdit"));
        audioStopAtEdit->setFrame(false);

        gridLayout_2->addWidget(audioStopAtEdit, 8, 1, 1, 1);

        onStartLayout = new QHBoxLayout();
        onStartLayout->setObjectName(QString::fromUtf8("onStartLayout"));
        audioOnStartCombo = new QComboBox(audioGroup);
        audioOnStartCombo->setObjectName(QString::fromUtf8("audioOnStartCombo"));

        onStartLayout->addWidget(audioOnStartCombo);

        audioOnStartEdit = new PsLineEdit(audioGroup);
        audioOnStartEdit->setObjectName(QString::fromUtf8("audioOnStartEdit"));
        audioOnStartEdit->setMinimumSize(QSize(60, 0));

        onStartLayout->addWidget(audioOnStartEdit);


        gridLayout_2->addLayout(onStartLayout, 9, 1, 1, 1);

        audioInitVolLabel = new QLabel(audioGroup);
        audioInitVolLabel->setObjectName(QString::fromUtf8("audioInitVolLabel"));
        audioInitVolLabel->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(audioInitVolLabel, 2, 0, 1, 1);

        audioFilePathLabel = new QLabel(audioGroup);
        audioFilePathLabel->setObjectName(QString::fromUtf8("audioFilePathLabel"));

        gridLayout_2->addWidget(audioFilePathLabel, 0, 0, 1, 1);

        initVolLayout = new QHBoxLayout();
        initVolLayout->setObjectName(QString::fromUtf8("initVolLayout"));
        initVolLayout->setContentsMargins(-1, 0, -1, -1);
        initialVolDial = new qsynthKnob(audioGroup);
        initialVolDial->setObjectName(QString::fromUtf8("initialVolDial"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(initialVolDial->sizePolicy().hasHeightForWidth());
        initialVolDial->setSizePolicy(sizePolicy1);
        initialVolDial->setMinimumSize(QSize(60, 60));
        initialVolDial->setMaximumSize(QSize(80, 80));
        initialVolDial->setMaximum(100);
        initialVolDial->setNotchTarget(10.000000000000000);
        initialVolDial->setNotchesVisible(true);

        initVolLayout->addWidget(initialVolDial);

        setToCurrentVolButton = new QPushButton(audioGroup);
        setToCurrentVolButton->setObjectName(QString::fromUtf8("setToCurrentVolButton"));

        initVolLayout->addWidget(setToCurrentVolButton);


        gridLayout_2->addLayout(initVolLayout, 2, 1, 1, 1);

        audioStopAtLabel = new QLabel(audioGroup);
        audioStopAtLabel->setObjectName(QString::fromUtf8("audioStopAtLabel"));

        gridLayout_2->addWidget(audioStopAtLabel, 8, 0, 1, 1);

        audioLoopsLabel = new QLabel(audioGroup);
        audioLoopsLabel->setObjectName(QString::fromUtf8("audioLoopsLabel"));

        gridLayout_2->addWidget(audioLoopsLabel, 4, 0, 1, 1);

        audioSlotSpin = new QSpinBox(audioGroup);
        audioSlotSpin->setObjectName(QString::fromUtf8("audioSlotSpin"));
        audioSlotSpin->setMaximum(4);

        gridLayout_2->addWidget(audioSlotSpin, 3, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, -1, -1);
        audioFilePathEdit = new PsLineEdit(audioGroup);
        audioFilePathEdit->setObjectName(QString::fromUtf8("audioFilePathEdit"));

        horizontalLayout->addWidget(audioFilePathEdit);

        findAudioFileButton = new QPushButton(audioGroup);
        findAudioFileButton->setObjectName(QString::fromUtf8("findAudioFileButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(findAudioFileButton->sizePolicy().hasHeightForWidth());
        findAudioFileButton->setSizePolicy(sizePolicy2);
        findAudioFileButton->setMaximumSize(QSize(28, 16777215));

        horizontalLayout->addWidget(findAudioFileButton);


        gridLayout_2->addLayout(horizontalLayout, 0, 1, 1, 1);


        verticalLayout->addWidget(audioGroup);

        sceneGroup = new QGroupBox(FxItemPropertyWidget);
        sceneGroup->setObjectName(QString::fromUtf8("sceneGroup"));
        gridLayout_3 = new QGridLayout(sceneGroup);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setVerticalSpacing(0);
        gridLayout_3->setContentsMargins(2, 2, 2, 2);
        faderCountEdit = new PsLineEdit(sceneGroup);
        faderCountEdit->setObjectName(QString::fromUtf8("faderCountEdit"));
        faderCountEdit->setFrame(true);

        gridLayout_3->addWidget(faderCountEdit, 0, 1, 1, 1);

        faderCountLabel = new QLabel(sceneGroup);
        faderCountLabel->setObjectName(QString::fromUtf8("faderCountLabel"));
        faderCountLabel->setWordWrap(true);

        gridLayout_3->addWidget(faderCountLabel, 0, 0, 1, 1);


        verticalLayout->addWidget(sceneGroup);

        closeButton = new QPushButton(FxItemPropertyWidget);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        verticalLayout->addWidget(closeButton);

        sequenceGroup = new QGroupBox(FxItemPropertyWidget);
        sequenceGroup->setObjectName(QString::fromUtf8("sequenceGroup"));
        gridLayout_5 = new QGridLayout(sequenceGroup);
        gridLayout_5->setSpacing(4);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(2, 2, 2, 2);
        seqStopOtherCheck = new QCheckBox(sequenceGroup);
        seqStopOtherCheck->setObjectName(QString::fromUtf8("seqStopOtherCheck"));

        gridLayout_5->addWidget(seqStopOtherCheck, 0, 0, 1, 1);

        seqBlackOtherCheck = new QCheckBox(sequenceGroup);
        seqBlackOtherCheck->setObjectName(QString::fromUtf8("seqBlackOtherCheck"));

        gridLayout_5->addWidget(seqBlackOtherCheck, 0, 1, 1, 1);


        verticalLayout->addWidget(sequenceGroup);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(FxItemPropertyWidget);
        QObject::connect(closeButton, SIGNAL(clicked()), FxItemPropertyWidget, SLOT(close()));

        QMetaObject::connectSlotsByName(FxItemPropertyWidget);
    } // setupUi

    void retranslateUi(QWidget *FxItemPropertyWidget)
    {
        FxItemPropertyWidget->setWindowTitle(QCoreApplication::translate("FxItemPropertyWidget", "Fx Editor", nullptr));
        fxGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Fx", nullptr));
        nameLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Name:", nullptr));
        postDelayLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Post Delay", nullptr));
        preDelayLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Pre Delay", nullptr));
        fadeInTimeLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Fade In Time", nullptr));
        fadeOutTimeLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Fade Out Time", nullptr));
        holdTimeLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Hold Time", nullptr));
        keyClearButton->setText(QCoreApplication::translate("FxItemPropertyWidget", "Clear", nullptr));
        idLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Id:", nullptr));
        keyLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Key:", nullptr));
        editOnceButton->setText(QCoreApplication::translate("FxItemPropertyWidget", "Edit once", nullptr));
        hookedToGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Hooked to", nullptr));
        hookedToUniverseLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Universe", nullptr));
        hookedToChannelLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Channel", nullptr));
        videoGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Video", nullptr));
        videoLoopsLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Loops", nullptr));
        videoFilePathLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "File path:", nullptr));
        videoInitialVolumeLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Initial\n"
"Volume", nullptr));
        videoBlackAtEndCheck->setText(QCoreApplication::translate("FxItemPropertyWidget", "Black at End", nullptr));
        audioGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Audio", nullptr));
        audioSlotLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Slot", nullptr));
        audioStartAtLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Start at", nullptr));
        audioOnStartLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "on Start", nullptr));
        audioOnStopLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "on Stop", nullptr));
        audioInitVolLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Initial\n"
"Volume", nullptr));
        audioFilePathLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "File path:", nullptr));
        setToCurrentVolButton->setText(QCoreApplication::translate("FxItemPropertyWidget", "Set to\n"
"current", nullptr));
        audioStopAtLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Stop at", nullptr));
        audioLoopsLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Loops", nullptr));
        findAudioFileButton->setText(QCoreApplication::translate("FxItemPropertyWidget", "...", nullptr));
        sceneGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Scene", nullptr));
        faderCountLabel->setText(QCoreApplication::translate("FxItemPropertyWidget", "Fader\n"
"count", nullptr));
        closeButton->setText(QCoreApplication::translate("FxItemPropertyWidget", "Close", nullptr));
        sequenceGroup->setTitle(QCoreApplication::translate("FxItemPropertyWidget", "Sequence", nullptr));
        seqStopOtherCheck->setText(QCoreApplication::translate("FxItemPropertyWidget", "STOP other\n"
"on start", nullptr));
        seqBlackOtherCheck->setText(QCoreApplication::translate("FxItemPropertyWidget", "BLACK\n"
"on start", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FxItemPropertyWidget: public Ui_FxItemPropertyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FXITEMPROPERTYWIDGET_H

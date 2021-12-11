/********************************************************************************
** Form generated from reading UI file 'audioslotwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOSLOTWIDGET_H
#define UI_AUDIOSLOTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "gui/customwidget/psspectrometer.h"
#include "gui/customwidget/psvumeter.h"
#include "thirdparty/widget/qsynthknob.h"

QT_BEGIN_NAMESPACE

class Ui_AudioSlotWidget
{
public:
    QVBoxLayout *mainLayout;
    QGridLayout *gridLayout;
    QPushButton *slotStopButton;
    QPushButton *slotPlayButton;
    QPushButton *slotAbsButton;
    QPushButton *slotPauseButton;
    PsVuMeter *meterWidget;
    QFrame *fftFrame;
    QVBoxLayout *verticalLayout;
    PsSpectrometer *fftWidget;
    qsynthKnob *slotVolumeDial;

    void setupUi(QGroupBox *AudioSlotWidget)
    {
        if (AudioSlotWidget->objectName().isEmpty())
            AudioSlotWidget->setObjectName(QString::fromUtf8("AudioSlotWidget"));
        AudioSlotWidget->resize(100, 605);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AudioSlotWidget->sizePolicy().hasHeightForWidth());
        AudioSlotWidget->setSizePolicy(sizePolicy);
        AudioSlotWidget->setMinimumSize(QSize(60, 0));
        AudioSlotWidget->setMaximumSize(QSize(100, 16777215));
        AudioSlotWidget->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        mainLayout = new QVBoxLayout(AudioSlotWidget);
        mainLayout->setSpacing(2);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(4, 0, 4, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(-1, 0, -1, -1);
        slotStopButton = new QPushButton(AudioSlotWidget);
        slotStopButton->setObjectName(QString::fromUtf8("slotStopButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/audio_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotStopButton->setIcon(icon);
        slotStopButton->setIconSize(QSize(36, 36));
        slotStopButton->setFlat(true);

        gridLayout->addWidget(slotStopButton, 1, 1, 1, 1);

        slotPlayButton = new QPushButton(AudioSlotWidget);
        slotPlayButton->setObjectName(QString::fromUtf8("slotPlayButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/gfx/icons/audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotPlayButton->setIcon(icon1);
        slotPlayButton->setIconSize(QSize(36, 36));
        slotPlayButton->setFlat(true);

        gridLayout->addWidget(slotPlayButton, 1, 0, 1, 1);

        slotAbsButton = new QPushButton(AudioSlotWidget);
        slotAbsButton->setObjectName(QString::fromUtf8("slotAbsButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/gfx/icons/config-date.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotAbsButton->setIcon(icon2);
        slotAbsButton->setIconSize(QSize(36, 36));
        slotAbsButton->setCheckable(true);

        gridLayout->addWidget(slotAbsButton, 0, 0, 1, 1);

        slotPauseButton = new QPushButton(AudioSlotWidget);
        slotPauseButton->setObjectName(QString::fromUtf8("slotPauseButton"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/gfx/icons/audio_pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotPauseButton->setIcon(icon3);
        slotPauseButton->setIconSize(QSize(36, 36));

        gridLayout->addWidget(slotPauseButton, 0, 1, 1, 1);


        mainLayout->addLayout(gridLayout);

        meterWidget = new PsVuMeter(AudioSlotWidget);
        meterWidget->setObjectName(QString::fromUtf8("meterWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(meterWidget->sizePolicy().hasHeightForWidth());
        meterWidget->setSizePolicy(sizePolicy1);

        mainLayout->addWidget(meterWidget);

        fftFrame = new QFrame(AudioSlotWidget);
        fftFrame->setObjectName(QString::fromUtf8("fftFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(fftFrame->sizePolicy().hasHeightForWidth());
        fftFrame->setSizePolicy(sizePolicy2);
        fftFrame->setMaximumSize(QSize(16777215, 40));
        fftFrame->setFrameShape(QFrame::Panel);
        fftFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout = new QVBoxLayout(fftFrame);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        fftWidget = new PsSpectrometer(fftFrame);
        fftWidget->setObjectName(QString::fromUtf8("fftWidget"));
        sizePolicy.setHeightForWidth(fftWidget->sizePolicy().hasHeightForWidth());
        fftWidget->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(fftWidget);


        mainLayout->addWidget(fftFrame);

        slotVolumeDial = new qsynthKnob(AudioSlotWidget);
        slotVolumeDial->setObjectName(QString::fromUtf8("slotVolumeDial"));
        sizePolicy.setHeightForWidth(slotVolumeDial->sizePolicy().hasHeightForWidth());
        slotVolumeDial->setSizePolicy(sizePolicy);

        mainLayout->addWidget(slotVolumeDial);


        retranslateUi(AudioSlotWidget);

        QMetaObject::connectSlotsByName(AudioSlotWidget);
    } // setupUi

    void retranslateUi(QGroupBox *AudioSlotWidget)
    {
        AudioSlotWidget->setTitle(QCoreApplication::translate("AudioSlotWidget", "Slot", nullptr));
        slotStopButton->setText(QString());
        slotPlayButton->setText(QString());
        slotAbsButton->setText(QString());
#if QT_CONFIG(tooltip)
        slotVolumeDial->setToolTip(QCoreApplication::translate("AudioSlotWidget", "<html><head/><body><p>Adjust play back volume of current audio fx</p><p><span style=\" font-weight:600;\">Double click </span>to fade out audio</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class AudioSlotWidget: public Ui_AudioSlotWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOSLOTWIDGET_H

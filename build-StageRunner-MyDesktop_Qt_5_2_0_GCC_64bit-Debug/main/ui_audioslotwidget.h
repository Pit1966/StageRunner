/********************************************************************************
** Form generated from reading UI file 'audioslotwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOSLOTWIDGET_H
#define UI_AUDIOSLOTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "thirdparty/widget/qsynthknob.h"
#include "thirdparty/widget/qsynthmeter.h"

QT_BEGIN_NAMESPACE

class Ui_AudioSlotWidget
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *slotPlayButton;
    QPushButton *slotStopButton;
    qsynthMeter *meterWidget;
    qsynthKnob *slotVolumeDial;

    void setupUi(QGroupBox *AudioSlotWidget)
    {
        if (AudioSlotWidget->objectName().isEmpty())
            AudioSlotWidget->setObjectName(QStringLiteral("AudioSlotWidget"));
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
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(4, 0, 4, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, -1);
        slotPlayButton = new QPushButton(AudioSlotWidget);
        slotPlayButton->setObjectName(QStringLiteral("slotPlayButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/gfx/icons/audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotPlayButton->setIcon(icon);
        slotPlayButton->setIconSize(QSize(36, 36));
        slotPlayButton->setFlat(true);

        horizontalLayout->addWidget(slotPlayButton);

        slotStopButton = new QPushButton(AudioSlotWidget);
        slotStopButton->setObjectName(QStringLiteral("slotStopButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/gfx/icons/audio_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        slotStopButton->setIcon(icon1);
        slotStopButton->setIconSize(QSize(36, 36));
        slotStopButton->setFlat(true);

        horizontalLayout->addWidget(slotStopButton);


        mainLayout->addLayout(horizontalLayout);

        meterWidget = new qsynthMeter(AudioSlotWidget);
        meterWidget->setObjectName(QStringLiteral("meterWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(meterWidget->sizePolicy().hasHeightForWidth());
        meterWidget->setSizePolicy(sizePolicy1);

        mainLayout->addWidget(meterWidget);

        slotVolumeDial = new qsynthKnob(AudioSlotWidget);
        slotVolumeDial->setObjectName(QStringLiteral("slotVolumeDial"));
        sizePolicy.setHeightForWidth(slotVolumeDial->sizePolicy().hasHeightForWidth());
        slotVolumeDial->setSizePolicy(sizePolicy);

        mainLayout->addWidget(slotVolumeDial);


        retranslateUi(AudioSlotWidget);

        QMetaObject::connectSlotsByName(AudioSlotWidget);
    } // setupUi

    void retranslateUi(QGroupBox *AudioSlotWidget)
    {
        AudioSlotWidget->setTitle(QApplication::translate("AudioSlotWidget", "Slot", 0));
        slotPlayButton->setText(QString());
        slotStopButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        slotVolumeDial->setToolTip(QApplication::translate("AudioSlotWidget", "<html><head/><body><p>Adjust play back volume of current audio fx</p><p><span style=\" font-weight:600;\">Double click </span>to fade out audio</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class AudioSlotWidget: public Ui_AudioSlotWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOSLOTWIDGET_H

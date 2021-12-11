/********************************************************************************
** Form generated from reading UI file 'sequencecontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEQUENCECONTROLWIDGET_H
#define UI_SEQUENCECONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "thirdparty/widget/qsynthknob.h"

QT_BEGIN_NAMESPACE

class Ui_SequenceControlWidget
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *ctrlButtonLayout;
    qsynthKnob *masterVolDial;
    QPushButton *moveToPrevButton;
    QPushButton *ctrlStopButton;
    QPushButton *ctrlPlayButton;
    QPushButton *moveToNextButton;
    QGroupBox *nextSequenceGroup;
    QVBoxLayout *verticalLayout;
    QLabel *nextInSeqLabel;

    void setupUi(QGroupBox *SequenceControlWidget)
    {
        if (SequenceControlWidget->objectName().isEmpty())
            SequenceControlWidget->setObjectName(QString::fromUtf8("SequenceControlWidget"));
        SequenceControlWidget->resize(489, 235);
        mainLayout = new QVBoxLayout(SequenceControlWidget);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        ctrlButtonLayout = new QHBoxLayout();
        ctrlButtonLayout->setObjectName(QString::fromUtf8("ctrlButtonLayout"));
        masterVolDial = new qsynthKnob(SequenceControlWidget);
        masterVolDial->setObjectName(QString::fromUtf8("masterVolDial"));
        masterVolDial->setNotchesVisible(true);

        ctrlButtonLayout->addWidget(masterVolDial);

        moveToPrevButton = new QPushButton(SequenceControlWidget);
        moveToPrevButton->setObjectName(QString::fromUtf8("moveToPrevButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/gfx/icons/audio_back.png"), QSize(), QIcon::Normal, QIcon::Off);
        moveToPrevButton->setIcon(icon);
        moveToPrevButton->setIconSize(QSize(48, 48));
        moveToPrevButton->setFlat(true);

        ctrlButtonLayout->addWidget(moveToPrevButton);

        ctrlStopButton = new QPushButton(SequenceControlWidget);
        ctrlStopButton->setObjectName(QString::fromUtf8("ctrlStopButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/gfx/icons/audio_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ctrlStopButton->setIcon(icon1);
        ctrlStopButton->setIconSize(QSize(48, 48));
        ctrlStopButton->setFlat(true);

        ctrlButtonLayout->addWidget(ctrlStopButton);

        ctrlPlayButton = new QPushButton(SequenceControlWidget);
        ctrlPlayButton->setObjectName(QString::fromUtf8("ctrlPlayButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/gfx/icons/audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        ctrlPlayButton->setIcon(icon2);
        ctrlPlayButton->setIconSize(QSize(64, 64));
        ctrlPlayButton->setFlat(true);

        ctrlButtonLayout->addWidget(ctrlPlayButton);

        moveToNextButton = new QPushButton(SequenceControlWidget);
        moveToNextButton->setObjectName(QString::fromUtf8("moveToNextButton"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/gfx/icons/audio_skip.png"), QSize(), QIcon::Normal, QIcon::Off);
        moveToNextButton->setIcon(icon3);
        moveToNextButton->setIconSize(QSize(48, 48));
        moveToNextButton->setFlat(true);

        ctrlButtonLayout->addWidget(moveToNextButton);


        mainLayout->addLayout(ctrlButtonLayout);

        nextSequenceGroup = new QGroupBox(SequenceControlWidget);
        nextSequenceGroup->setObjectName(QString::fromUtf8("nextSequenceGroup"));
        verticalLayout = new QVBoxLayout(nextSequenceGroup);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        nextInSeqLabel = new QLabel(nextSequenceGroup);
        nextInSeqLabel->setObjectName(QString::fromUtf8("nextInSeqLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(nextInSeqLabel->sizePolicy().hasHeightForWidth());
        nextInSeqLabel->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(25, 255, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(144, 141, 139, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        nextInSeqLabel->setPalette(palette);
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        nextInSeqLabel->setFont(font);

        verticalLayout->addWidget(nextInSeqLabel);


        mainLayout->addWidget(nextSequenceGroup);


        retranslateUi(SequenceControlWidget);

        QMetaObject::connectSlotsByName(SequenceControlWidget);
    } // setupUi

    void retranslateUi(QGroupBox *SequenceControlWidget)
    {
        SequenceControlWidget->setWindowTitle(QCoreApplication::translate("SequenceControlWidget", "Form", nullptr));
#if QT_CONFIG(tooltip)
        masterVolDial->setToolTip(QCoreApplication::translate("SequenceControlWidget", "<html><head/><body><p><span style=\" font-size:10pt; color:#aa007f;\">Master volume</span></p><p><span style=\" color:#000000;\">Use this to adjust the output volume.</span></p><p><span style=\" color:#000000;\">This setting is not affected by any FX command </span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        moveToPrevButton->setText(QString());
        ctrlStopButton->setText(QString());
#if QT_CONFIG(tooltip)
        ctrlPlayButton->setToolTip(QCoreApplication::translate("SequenceControlWidget", "<html><head/><body><p><span style=\" font-size:10pt; color:#aa007f;\">Start next FX</span></p><p><span style=\" color:#000000;\">This starts the next FX item in sequence. At the same time the current active FX item is stopped.</span></p><p><span style=\" color:#000000;\">This will apply to the next item in the FX list. (blue selection)</span></p><p><span style=\" color:#000000;\">If a FX item is marked (green selection), this one will be started instead.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        ctrlPlayButton->setText(QString());
        moveToNextButton->setText(QString());
        nextSequenceGroup->setTitle(QCoreApplication::translate("SequenceControlWidget", "Next in sequence", nullptr));
        nextInSeqLabel->setText(QCoreApplication::translate("SequenceControlWidget", "---", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SequenceControlWidget: public Ui_SequenceControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEQUENCECONTROLWIDGET_H
